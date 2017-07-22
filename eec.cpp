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

#include "input-equation-vector.h"
#include "variable-set.h"

// Our System of Linear Equations
class LinearSystem
{
public:
    typedef boost::numeric::ublas::matrix< Element > AugmentedMatrix;
    typedef boost::numeric::ublas::matrix_row< AugmentedMatrix > AugmentedMatrixRow;
    typedef boost::numeric::ublas::matrix_column< AugmentedMatrix > AugmentedMatrixColumn;

    void convertInputToMatrix( InputEquationVector const & inputEquation );
    void printMatrix();
    void solveByGaussianElimination();
    void convertMatrixToResults();
    void outputResults();

private:
    void incrementHeight();
    void addVariable( int side, Dimensions row, Dimensions column );
    void increaseWidth( Dimensions newWidth );
    void addConstant( Dimensions row, Element constant );

    AugmentedMatrix augMatrix; // augmented matrix of coefficients and constants
    VariableSet variable;

    static int const left_side;
    static int const right_side;
};

int const LinearSystem::left_side = 1;
int const LinearSystem::right_side = -1;

void LinearSystem::convertInputToMatrix( InputEquationVector const & inputEquation )
{
    Dimensions row = 0;

    BOOST_FOREACH ( std::string equationAsString, inputEquation.asVector() )
    {
        Tokenizer equationAsTokens = stringToTokens( equationAsString );
        bool first_token = true;
        bool quantity = true;

        BOOST_FOREACH ( std::string token, equationAsTokens )
        {
            Dimensions va_index = 0;

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

    for ( Dimensions i = 0; i < augMatrix.size2(); ++i )
        row( i ) = 0;

#ifdef SHOW_CALCULATIONS
    std::cout << std::endl;
#endif
}

void LinearSystem::addVariable( int side, Dimensions row, Dimensions column )
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

void LinearSystem::increaseWidth( Dimensions newWidth )
{
    Dimensions height = augMatrix.size1();
    Dimensions old_width = augMatrix.size2();
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

void LinearSystem::addConstant( Dimensions row, Element constant )
{
    // Add constant at this equation index (row) and constant (last) column.
    if ( row >= augMatrix.size1() )
        augMatrix( row, augMatrix.size2() - 1 ) = constant;

    else
        augMatrix( row, augMatrix.size2() - 1 ) += constant;
}

void LinearSystem::printMatrix()
{
    for( Dimensions i = 0; i < augMatrix.size1(); ++i )
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
    Dimensions m = augMatrix.size1();
    Dimensions n = augMatrix.size2();

    for ( Dimensions k = 0; k < std::min( m, n ); ++k )
    {
        // find the k-th pivot:
        Element max_co = 0;
        Dimensions i_of_max_co = 0;

        for ( Dimensions i = k; i < m; ++i )
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
        for ( Dimensions i = k + 1; i < m; ++i )
        {
            Element f = augMatrix( i, k ) / augMatrix( k, k );

            // for all remaining elements in current row:
            for ( Dimensions j = k + 1; j < n; ++j )
            {
                augMatrix( i, j ) = augMatrix( i, j ) - augMatrix( k, j ) * f;
            }

            // fill lower triangular matrix with zeros:
            augMatrix( i, k ) = 0;
        }
    }
}

// This function uses back substitution to solve all the variables.
void LinearSystem::convertMatrixToResults()
{
    for ( Dimensions e = augMatrix.size1() - 1; ; --e )
    {
        Element answer = augMatrix( e, augMatrix.size2() - 1 );

#ifdef SHOW_CALCULATIONS
        std::cout << "augMatrix(" << e << "," << augMatrix.size2() - 1 << ") = " << augMatrix( e, augMatrix.size2() - 1 ) << std::endl;
        std::cout << "answer <- augMatrix(" << e << "," << augMatrix.size2() - 1 << ")" << std::endl;
        std::cout << "answer = " << answer << std::endl;
#endif

        for ( Dimensions v = augMatrix.size2() - 2; ; --v )
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
    if ( argc < 2 ) // no input file
        return 0;

    InputEquationVector inputEquation;
    std::string const fileName( argv[ 1 ] );
    inputEquation.loadInputFile( fileName );

    // If any input equation is not valid, print all input equations.
    if ( ! inputEquation.equationsAreValid() )
    {
        inputEquation.printAllEquations();
        return 0;
    }

    LinearSystem system;
    system.convertInputToMatrix( inputEquation );
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
