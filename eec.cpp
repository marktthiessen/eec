#include <algorithm> // min
#include <cctype> // isalpha, isdigit
#include <cstdlib> // atol
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

class system_of_equations
{
public:
    typedef long double element_type;
    typedef boost::numeric::ublas::matrix< element_type > boost_matrix;
    typedef boost_matrix::size_type boost_matrix_size_type;
    typedef boost::numeric::ublas::vector< std::string > boost_vector;
    typedef boost_vector::size_type boost_vector_size_type;
    typedef boost::numeric::ublas::matrix_row< boost_matrix > boost_matrix_row;
    typedef boost::numeric::ublas::matrix_column< boost_matrix > boost_matrix_column;
    typedef std::vector< std::string > string_vector;
    typedef string_vector::size_type size_type;
    typedef boost::function< int( int ) > int_f_int;
    typedef boost::tokenizer< boost::char_separator< char > > tokenizer;

    void load_input_file( std::string const & file_name );
    bool equations_are_valid();
    void print_all_equations();
    void convert_input();
    void print_matrix();
    void gaussian_elimination_wikipedia();

private:
    bool is_valid_string( std::string const &, int_f_int is_valid_char );
    bool is_valid_variable_name( std::string const & );
    bool is_valid_unsigned_integer( std::string const & );
    bool is_valid_variable_name_or_unsigned_integer( std::string const & );
    bool is_equal_sign( std::string const & );
    bool is_plus_sign( std::string const & );
    bool is_equation( tokenizer const & equation );
    tokenizer string_to_tokens( std::string const & equation );
    size_type note_variable( std::string const & variable_name );
    void increment_height();
    void add_variable( int side, size_type row, size_type column );
    void increase_width( size_type new_width );
    void add_constant( size_type row, element_type constant );

    string_vector input_system;
    boost_matrix co;
    string_vector va;
    std::map< std::string, size_type > va_check;

    static int const left_side;
    static int const right_side;
};

int const system_of_equations::left_side = 1;
int const system_of_equations::right_side = -1;

void system_of_equations::convert_input()
{
    size_type row = 0;

    BOOST_FOREACH ( std::string equation_as_string, input_system )
    {
        tokenizer equation_as_tokens = string_to_tokens( equation_as_string );
        bool first_token = true;
        bool quantity = true;

        BOOST_FOREACH ( std::string token, equation_as_tokens )
        {
            size_type va_index = 0;

            if ( first_token )
            {
                // this must be a variable on the left side of this input equation
                va_index = note_variable( token );
                increment_height();
                add_variable( left_side, row, va_index );
            }
            else if ( quantity )
            {
                // this must be a variable or constant on the right side of this input equation
                if ( isdigit( token[ 0 ] ) )
                {
                    add_constant( row, atol( token.c_str() ) );
                }
                else
                {
                    va_index = note_variable( token );
                    add_variable( right_side, row, va_index );
                }
            }
            // else, this must be the equal sign or a plus sign

            first_token = false;
            quantity = ! quantity;
        }

        ++row;
    }
}

system_of_equations::size_type system_of_equations::note_variable( std::string const & variable_name )
{
    size_type va_index = 0;

    if ( 0 == va_check.count( variable_name ) )
    {
        va.push_back( variable_name );
        va_index = va.size() - 1;
        va_check[ variable_name ] = va_index;
    }
    else
    {
        va_index = va_check[ variable_name ];
    }

    return va_index;
}

void system_of_equations::increment_height()
{
    co.resize( co.size1() + 1, co.size2() );
    boost_matrix_row row( co, co.size1() - 1 );

    for ( size_type i = 0; i < co.size2(); ++i )
        row( i ) = 0;

    std::cout << std::endl;
}

void system_of_equations::add_variable( int side, size_type row, size_type column )
{
    // Subtract 1 from the coefficient at this equation index (row) and variable index (column).
    if ( column + 1 >= co.size2() ) // no room for the constant column?
    {
        increase_width( column + 2 ); // make room for this column and the constant column
        co( row, column ) = side;
    }
    
    else if ( row >= co.size1() )
        co( row, column ) = side;
    
    else
        co( row, column ) += side;
}

void system_of_equations::increase_width( size_type new_width )
{
    size_type height = co.size1();
    size_type old_width = co.size2();
    assert( old_width != 1 );
    assert( new_width >= 2 ); // minimal non-empty width due to the constant column

    if ( height == 0 )
        height = 1;

    co.resize( height, new_width );
    co( 0, old_width ) = 0; // in case old_width is 0 and new_width is 2
    co( 0, new_width - 1 ) = 0;

    if ( 0 < old_width )
    {
        boost_matrix_column old_constants( co, old_width - 1 );
        boost_matrix_column new_constants( co, new_width - 1 );
        new_constants = old_constants;
        old_constants -= old_constants;
    }
}

void system_of_equations::add_constant( size_type row, element_type constant )
{
    // Add constant at this equation index (row) and constant (last) column.
    if ( row >= co.size1() )
        co( row, co.size2() - 1 ) = constant;

    else
        co( row, co.size2() - 1 ) += constant;
}

void system_of_equations::print_matrix()
{
    for( size_type i = 0; i < co.size1(); ++i )
    {
        boost_matrix_row row( co, i );

        BOOST_FOREACH( element_type coefficient, row )
        {
            std::cout << coefficient << " ";
        }

        std::cout << std::endl;
    }

    std::cout << std::endl;
}

void system_of_equations::gaussian_elimination_wikipedia()
{
    boost_matrix_size_type m = co.size1();
    boost_matrix_size_type n = co.size2();

    for ( boost_matrix_size_type k = 0; k < std::min( m, n ); ++k )
    {
        // find the k-th pivot:
        element_type max_co = 0;
        boost_matrix_size_type i_of_max_co = 0;

        for ( boost_matrix_size_type i = k; i < m; ++i )
        {
            element_type abs_co = co( i, k );

            if ( abs_co < 0 )
                abs_co = -abs_co;

            if ( abs_co > max_co )
            {
                max_co = abs_co;
                i_of_max_co = i;
            }
        }

        if ( co( i_of_max_co, k ) == 0 )
        {
            // matrix is singular
            // indicate an error
            break;
        }

        // swap rows k and i_max:
        boost_matrix_row q( co, k );
        boost_matrix_row r( co, i_of_max_co );
        r.swap( q );

        // for all rows below pivot:
        for ( boost_matrix_size_type i = k + 1; i < m; ++i )
        {
            element_type f = co( i, k ) / co( k, k );

            // for all remaining elements in current row:
            for ( boost_matrix_size_type j = k + 1; j < n; ++j )
            {
                co( i, j ) = co( i, j ) - co( k, j ) * f;
            }

            // fill lower triangular matrix with zeros:
            co( i, k ) = 0;
        }
    }
}

bool system_of_equations::is_valid_string( std::string const & str, int_f_int is_valid_char )
{
    if ( str.size() > 10 )
        return false;

    bool valid = true;

    BOOST_FOREACH ( char c, str )
    {
        valid = is_valid_char( c );

        if ( ! valid )
            break;
    }

    return valid;
}

bool system_of_equations::is_valid_variable_name( std::string const & str )
{
   return is_valid_string( str, boost::bind( isalpha, _1 ) );
}

bool system_of_equations::is_valid_unsigned_integer( std::string const & str )
{
   return is_valid_string( str, boost::bind( isdigit, _1 ) );
}

bool system_of_equations::is_valid_variable_name_or_unsigned_integer( std::string const & str )
{
    return( is_valid_variable_name( str ) || is_valid_unsigned_integer( str ) );
}

bool system_of_equations::is_equal_sign( std::string const &token )
{
    return( "=" == token );
}

bool system_of_equations::is_plus_sign( std::string const & token )
{
    return( "+" == token );
}

bool system_of_equations::is_equation( tokenizer const & equation )
{
    bool valid = true;

    tokenizer::const_iterator token = equation.begin();

    // first token: variable
    valid = ( token != equation.end() );

    if ( ! valid )
        return valid;

    valid = is_valid_variable_name( *token );

    if ( ! valid )
        return valid;

    ++token;

    // second token: equal sign
    valid = ( token != equation.end() );

    if ( ! valid )
        return valid;

    valid = is_equal_sign( *token );

    if ( ! valid )
        return valid;

    ++token;

    // third token: variable or unsigned integer
    valid = ( token != equation.end() );

    if ( ! valid )
        return valid;

    valid = is_valid_variable_name_or_unsigned_integer( *token );

    if ( ! valid )
        return valid;

    ++token;

    // remaining pairs tokens: plus sign then either variable or unsigned integer
    for ( ; token != equation.end(); ++token)
    {
        valid = is_plus_sign( *token );

        if ( ! valid )
            break;

        ++token;
        valid = ( token != equation.end() );

        if ( ! valid )
            break;

        valid = is_valid_variable_name_or_unsigned_integer( *token );

        if ( ! valid )
            break;
    }

    return valid;
}

void system_of_equations::load_input_file( std::string const & file_name )
{
    std::ifstream input_file;
    input_file.open( file_name.c_str() );

    if ( input_file.is_open() )
    {
        std::string input_equation;

        while ( std::getline( input_file, input_equation ) )
        {
            input_system.push_back( input_equation );
        }

        input_file.close();
    }
}

bool system_of_equations::equations_are_valid()
{
    bool valid = true;

    BOOST_FOREACH ( std::string equation_as_string, input_system )
    {
        tokenizer equation_as_tokens = string_to_tokens( equation_as_string );
        valid = is_equation( equation_as_tokens );

        if ( ! valid )
            break;
    }

    return valid;
}

system_of_equations::tokenizer system_of_equations::string_to_tokens( std::string const & equation_as_string )
{
    boost::char_separator< char > separator( " " );
    tokenizer equation_as_tokens( equation_as_string, separator );
    return equation_as_tokens;
}

void system_of_equations::print_all_equations()
{
    BOOST_FOREACH ( std::string equation_as_string, input_system )
    {
        std::cout << equation_as_string << std::endl;
    }
}

int main( int argc, char * argv[] )
{
    if ( argc < 2 )
        return 0; // no input file

    system_of_equations system;
    std::string const file_name( argv[ 1 ] );
    system.load_input_file( file_name );

    // If any input equation is not valid, print all input equations.
    if ( ! system.equations_are_valid() )
    {
        system.print_all_equations();
        return 0;
    }

    system.convert_input();
    system.print_matrix();
    system.gaussian_elimination_wikipedia();
    system.print_matrix();

    return 0;
}
