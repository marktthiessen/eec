#include <algorithm> // min
#include <cctype> // isalpha, isdigit
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
    typedef std::vector< std::string > string_vector;
    typedef string_vector::size_type size_type;
    typedef boost::function< int( int ) > int_f_int;
    typedef boost::tokenizer< boost::char_separator< char > > tokenizer;

    void load_input_file( std::string const & file_name );
    bool equations_are_valid();
    void print_all_equations();
    void convert_input();

private:
    void gaussian_elimination_wikipedia();
    bool is_valid_string( std::string const &, int_f_int is_valid_char );
    bool is_valid_variable_name( std::string const & );
    bool is_valid_unsigned_integer( std::string const & );
    bool is_valid_variable_name_or_unsigned_integer( std::string const & );
    bool is_equal_sign( std::string const & );
    bool is_plus_sign( std::string const & );
    bool is_equation( tokenizer const & equation );
    tokenizer string_to_tokens( std::string const & equation );
    size_type note_variable( std::string const & variable_name );
    void add_left_side_variable( size_type va_index );
    void add_right_side_variable( size_type va_index );
    void add_constant( std::string const & value );

    string_vector input_system;
    boost_matrix co;
    string_vector va;
    std::map< std::string, size_type > va_check;
};

void system_of_equations::convert_input()
{
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
                va_index = note_variable( token );
                add_left_side_variable( va_index );
            }
            else if ( quantity )
            {
                if ( isdigit( token[ 0 ] ) )
                {
                    add_constant( token );
                }
                else
                {
                    va_index = note_variable( token );
                    add_right_side_variable( va_index );
                }
            }

            quantity = ! quantity;
        }
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

void system_of_equations::add_left_side_variable( size_type va_index )
{
    // +1 coefficient at this equation row and va_index column
}

void system_of_equations::add_right_side_variable( size_type va_index )
{
    // -1 coefficient at this equation row and va_index column
}

void system_of_equations::add_constant( std::string const & value )
{
    // +value constant at this equation row and constant column
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
    input_file.open( file_name );

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

    return 0;
}
