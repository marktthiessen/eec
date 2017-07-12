#include <algorithm> // min, sort
#include <cctype> // isalpha, isdigit
#include <cstdlib> // atol
#include <exception> // length_error
#include <fstream> // open, close
#include <iostream> // cout, endl
#include <map>
#include <string>
#include <vector>

#include <boost/bind.hpp>
#include <boost/foreach.hpp>
#include <boost/function.hpp>
#include <boost/numeric/ublas/matrix.hpp>
#include <boost/numeric/ublas/matrix_proxy.hpp>
#include <boost/numeric/ublas/vector.hpp>
#include <boost/numeric/ublas/io.hpp>
#include <boost/tokenizer.hpp>

typedef long double Element;
typedef uint8_t Index;
static Index const maximumDimensions = 255;

class Variable
{
public:
    Variable( std::string name )
    : name( name )
    , value( 0 )
    {
    }

    std::string name;
    Element value;
};

class VariableSet
{
public:
    Index indexFor( std::string const & name )
    {
        Index index = 0;

        if ( 0 == indexOf.count( name ) )
        {
            // This is a new variable.
            if ( variableSet.size() == maximumDimensions )
            {
                throw std::length_error( "variable set full" );
            }

            variableSet.push_back( name );
            index = variableSet.size() - 1; // get next available index
            indexOf[ name ] = index;
        }
        else
        {
            index = indexOf[ name ];
        }

        return index;
    }

    Element & operator[]( Index const index )
    {
        return variableSet[ index ].value;
    }

    std::string asName( Index const index )
    {
        return variableSet[ index ].name;
    }

    std::vector< Variable > asVector()
    {
        return variableSet;
    }

private:
    std::vector< Variable > variableSet;
    std::map< std::string, Index > indexOf; // for faster index lookups than searching variableSet
};

bool operator<( Variable lhs, Variable rhs )
{
    return lhs.name < rhs.name;
}

typedef boost::tokenizer< boost::char_separator< char > > Tokenizer;

Tokenizer stringToTokens( std::string const & equationAsString )
{
    boost::char_separator< char > separator( " " );
    Tokenizer equationAsTokens( equationAsString, separator );
    return equationAsTokens;
}

// Our System of Linear Equations
class LinearSystem
{
public:
    typedef boost::numeric::ublas::matrix< Element > AugmentedMatrix;
    typedef AugmentedMatrix::size_type AugmentedMatrixSize;
    typedef boost::numeric::ublas::matrix_row< AugmentedMatrix > AugmentedMatrixRow;
    typedef boost::numeric::ublas::matrix_column< AugmentedMatrix > AugmentedMatrixColumn;
    typedef std::vector< std::string > InputEquationVector;
    typedef InputEquationVector::size_type InputEquationVectorSize;
    typedef boost::function< int( int ) > IntFunctionInt;

    void loadInputFile( std::string const & fileName );
    bool equationsAreValid();
    void printAllEquations();
    void convertInputToMatrix();
    void printMatrix();
    void solveByGaussianElimination();
    void convertMatrixToResults();
    void outputResults();

private:
    bool isValidString( std::string const &, IntFunctionInt isValidChar );
    bool isValidVariableName( std::string const & );
    bool isValidUnsignedInteger( std::string const & );
    bool isValidVariableNameOrUnsignedInteger( std::string const & );
    bool isEqualSign( std::string const & );
    bool isPlusSign( std::string const & );
    bool isEquation( Tokenizer const & equation );
    void incrementHeight();
    void addVariable( int side, InputEquationVectorSize row, InputEquationVectorSize column );
    void increaseWidth( InputEquationVectorSize newWidth );
    void addConstant( InputEquationVectorSize row, Element constant );

    InputEquationVector equationVector;
    AugmentedMatrix augMatrix; // augmented matrix of coefficients and constants
    VariableSet variable;

    static int const left_side;
    static int const right_side;
};

int const LinearSystem::left_side = 1;
int const LinearSystem::right_side = -1;

void LinearSystem::convertInputToMatrix()
{
    InputEquationVectorSize row = 0;

    BOOST_FOREACH ( std::string equationAsString, equationVector )
    {
        Tokenizer equationAsTokens = stringToTokens( equationAsString );
        bool first_token = true;
        bool quantity = true;

        BOOST_FOREACH ( std::string token, equationAsTokens )
        {
            InputEquationVectorSize va_index = 0;

            if ( first_token )
            {
                // this must be a variable on the left side of this input equation
                va_index = variable.indexFor( token );
                incrementHeight();
                addVariable( left_side, row, va_index );
            }
            else if ( quantity )
            {
                // this must be a variable or constant on the right side of this input equation
                if ( isdigit( token[ 0 ] ) )
                {
                    addConstant( row, atol( token.c_str() ) );
                }
                else
                {
                    va_index = variable.indexFor( token );
                    addVariable( right_side, row, va_index );
                }
            }
            // else, this must be the equal sign or a plus sign

            first_token = false;
            quantity = ! quantity;
        }

        ++row;
    }
}

void LinearSystem::incrementHeight()
{
    augMatrix.resize( augMatrix.size1() + 1, augMatrix.size2() );
    AugmentedMatrixRow row( augMatrix, augMatrix.size1() - 1 );

    for ( InputEquationVectorSize i = 0; i < augMatrix.size2(); ++i )
        row( i ) = 0;

#ifdef SHOW_CALCULATIONS
    std::cout << std::endl;
#endif
}

void LinearSystem::addVariable( int side, InputEquationVectorSize row, InputEquationVectorSize column )
{
    // Subtract 1 from the coefficient at this equation index (row) and variable index (column).
    if ( column + 1 >= augMatrix.size2() ) // no room for the constant column?
    {
        increaseWidth( column + 2 ); // make room for this column and the constant column
        augMatrix( row, column ) = side;
    }

    else if ( row >= augMatrix.size1() )
        augMatrix( row, column ) = side;

    else
        augMatrix( row, column ) += side;
}

void LinearSystem::increaseWidth( InputEquationVectorSize newWidth )
{
    InputEquationVectorSize height = augMatrix.size1();
    InputEquationVectorSize old_width = augMatrix.size2();
    assert( old_width != 1 );
    assert( newWidth >= 2 ); // minimal non-empty width due to the constant column

    if ( height == 0 )
        height = 1;

    augMatrix.resize( height, newWidth );
    augMatrix( 0, old_width ) = 0; // in case old_width is 0 and newWidth is 2
    augMatrix( 0, newWidth - 1 ) = 0;

    if ( 0 < old_width )
    {
        AugmentedMatrixColumn old_constants( augMatrix, old_width - 1 );
        AugmentedMatrixColumn new_constants( augMatrix, newWidth - 1 );
        new_constants = old_constants;
        old_constants -= old_constants;
    }
}

void LinearSystem::addConstant( InputEquationVectorSize row, Element constant )
{
    // Add constant at this equation index (row) and constant (last) column.
    if ( row >= augMatrix.size1() )
        augMatrix( row, augMatrix.size2() - 1 ) = constant;

    else
        augMatrix( row, augMatrix.size2() - 1 ) += constant;
}

void LinearSystem::printMatrix()
{
    for( InputEquationVectorSize i = 0; i < augMatrix.size1(); ++i )
    {
        AugmentedMatrixRow row( augMatrix, i );

        BOOST_FOREACH( Element coefficient, row )
        {
            std::cout << coefficient << " ";
        }

        std::cout << std::endl;
    }

    std::cout << std::endl;
}

void LinearSystem::solveByGaussianElimination()
{
    AugmentedMatrixSize m = augMatrix.size1();
    AugmentedMatrixSize n = augMatrix.size2();

    for ( AugmentedMatrixSize k = 0; k < std::min( m, n ); ++k )
    {
        // find the k-th pivot:
        Element max_co = 0;
        AugmentedMatrixSize i_of_max_co = 0;

        for ( AugmentedMatrixSize i = k; i < m; ++i )
        {
            Element abs_co = augMatrix( i, k );

            if ( abs_co < 0 )
                abs_co = -abs_co;

            if ( abs_co > max_co )
            {
                max_co = abs_co;
                i_of_max_co = i;
            }
        }

        if ( augMatrix( i_of_max_co, k ) == 0 )
        {
            // matrix is singular
            // indicate an error
            break;
        }

        // swap rows k and i_max:
        AugmentedMatrixRow q( augMatrix, k );
        AugmentedMatrixRow r( augMatrix, i_of_max_co );
        r.swap( q );

        // for all rows below pivot:
        for ( AugmentedMatrixSize i = k + 1; i < m; ++i )
        {
            Element f = augMatrix( i, k ) / augMatrix( k, k );

            // for all remaining elements in current row:
            for ( AugmentedMatrixSize j = k + 1; j < n; ++j )
            {
                augMatrix( i, j ) = augMatrix( i, j ) - augMatrix( k, j ) * f;
            }

            // fill lower triangular matrix with zeros:
            augMatrix( i, k ) = 0;
        }
    }
}

bool LinearSystem::isValidString( std::string const & str, IntFunctionInt isValidChar )
{
    if ( str.size() > 10 )
        return false;

    bool valid = true;

    BOOST_FOREACH ( char c, str )
    {
        valid = isValidChar( c );

        if ( ! valid )
            break;
    }

    return valid;
}

bool LinearSystem::isValidVariableName( std::string const & str )
{
   return isValidString( str, boost::bind( isalpha, _1 ) );
}

bool LinearSystem::isValidUnsignedInteger( std::string const & str )
{
   return isValidString( str, boost::bind( isdigit, _1 ) );
}

bool LinearSystem::isValidVariableNameOrUnsignedInteger( std::string const & str )
{
    return( isValidVariableName( str ) || isValidUnsignedInteger( str ) );
}

bool LinearSystem::isEqualSign( std::string const &token )
{
    return( "=" == token );
}

bool LinearSystem::isPlusSign( std::string const & token )
{
    return( "+" == token );
}

bool LinearSystem::isEquation( Tokenizer const & equation )
{
    bool valid = true;

    Tokenizer::const_iterator token = equation.begin();

    // first token: variable
    valid = ( token != equation.end() );

    if ( ! valid )
        return valid;

    valid = isValidVariableName( *token );

    if ( ! valid )
        return valid;

    ++token;

    // second token: equal sign
    valid = ( token != equation.end() );

    if ( ! valid )
        return valid;

    valid = isEqualSign( *token );

    if ( ! valid )
        return valid;

    ++token;

    // third token: variable or unsigned integer
    valid = ( token != equation.end() );

    if ( ! valid )
        return valid;

    valid = isValidVariableNameOrUnsignedInteger( *token );

    if ( ! valid )
        return valid;

    ++token;

    // remaining pairs tokens: plus sign then either variable or unsigned integer
    for ( ; token != equation.end(); ++token)
    {
        valid = isPlusSign( *token );

        if ( ! valid )
            break;

        ++token;
        valid = ( token != equation.end() );

        if ( ! valid )
            break;

        valid = isValidVariableNameOrUnsignedInteger( *token );

        if ( ! valid )
            break;
    }

    return valid;
}

void LinearSystem::loadInputFile( std::string const & fileName )
{
    std::ifstream input_file;
    input_file.open( fileName.c_str() );

    if ( input_file.is_open() )
    {
        std::string input_equation;

        while ( std::getline( input_file, input_equation ) )
        {
            equationVector.push_back( input_equation );
        }

        input_file.close();
    }
}

bool LinearSystem::equationsAreValid()
{
    bool valid = true;

    BOOST_FOREACH ( std::string equationAsString, equationVector )
    {
        Tokenizer equationAsTokens = stringToTokens( equationAsString );
        valid = isEquation( equationAsTokens );

        if ( ! valid )
            break;
    }

    return valid;
}

void LinearSystem::printAllEquations()
{
    BOOST_FOREACH ( std::string equationAsString, equationVector )
    {
        std::cout << equationAsString << std::endl;
    }
}

// This function uses back substitution to solve all the variables.
void LinearSystem::convertMatrixToResults()
{
    for ( InputEquationVectorSize e = augMatrix.size1() - 1; ; --e )
    {
        Element answer = augMatrix( e, augMatrix.size2() - 1 );

#ifdef SHOW_CALCULATIONS
        std::cout << "augMatrix(" << e << "," << augMatrix.size2() - 1 << ") = " << augMatrix( e, augMatrix.size2() - 1 ) << std::endl;
        std::cout << "answer <- augMatrix(" << e << "," << augMatrix.size2() - 1 << ")" << std::endl;
        std::cout << "answer = " << answer << std::endl;
#endif

        for ( InputEquationVectorSize v = augMatrix.size2() - 2; ; --v )
        {
            if ( v > e )
            {

#ifdef SHOW_CALCULATIONS
                std::cout << "augMatrix(" << e << "," << v << ") = " << augMatrix( e, v ) << std::endl;
                std::cout << variable.asName( v ) << " = " << variable[ v ] << std::endl;
#endif

                answer -= augMatrix( e, v ) * variable[ v ];

#ifdef SHOW_CALCULATIONS
                std::cout << "answer <- answer - augMatrix(" << e << "," << v << ") * " << variable.asName( v ) << std::endl;
                std::cout << "answer = " << answer << std::endl;
#endif
            }

            else
            {

#ifdef SHOW_CALCULATIONS
                std::cout << "augMatrix(" << e << "," << v << ") = " << augMatrix( e, v ) << std::endl;
#endif

                answer /= augMatrix( e, v );

#ifdef SHOW_CALCULATIONS
                std::cout << "answer <- answer / augMatrix(" << e << "," << v << ")" << std::endl;
                std::cout << "answer = " << answer << std::endl;
#endif

                variable[ v ] = answer;

#ifdef SHOW_CALCULATIONS
                std::cout << variable.asName( v ) << " <- answer" << std::endl;
                std::cout << variable.asName( v ) << " = " << variable[ v ] << std::endl;
#endif
            }

            if ( v == e )
                break;
        }

        if ( e == 0 )
            break;
    }
}

void LinearSystem::outputResults()
{
    std::vector< Variable > variableSet = variable.asVector();
    std::sort( variableSet.begin(), variableSet.end(), operator< );

    BOOST_FOREACH ( Variable v, variableSet )
    {
        std::cout << v.name << " = " << v.value << std::endl;
    }
}

int main( int argc, char * argv[] )
{
    if ( argc < 2 )
        return 0; // no input file

    LinearSystem system;
    std::string const fileName( argv[ 1 ] );
    system.loadInputFile( fileName );

    // If any input equation is not valid, print all input equations.
    if ( ! system.equationsAreValid() )
    {
        system.printAllEquations();
        return 0;
    }

    system.convertInputToMatrix();
#ifdef SHOW_CALCULATIONS
    system.printMatrix();
#endif
    system.solveByGaussianElimination();
#ifdef SHOW_CALCULATIONS
    system.printMatrix();
#endif
    system.convertMatrixToResults();
    system.outputResults();

    return 0;
}
