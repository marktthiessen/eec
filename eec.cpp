#include <cctype> // isalpha, isdigit
#include <fstream> // open, close
#include <iostream> // cout, endl
#include <string>
#include <vector>

#include <boost/bind.hpp>
#include <boost/function.hpp>
#include <boost/numeric/ublas/matrix.hpp>
#include <boost/numeric/ublas/vector.hpp>
#include <boost/numeric/ublas/io.hpp>
#include <boost/tokenizer.hpp>

typedef std::vector< std::string > string_vector;

class system_of_equations
{
public:
    typedef std::vector< std::string >::size_type Size;

    system_of_equations( string_vector const & system )
    : coefficient( system.size(), system.size() )
    , variable( system.size() )
    , constant( system.size() )
    {
    }

private:
    boost::numeric::ublas::matrix< long > coefficient;
    boost::numeric::ublas::vector< std::string > variable;
    boost::numeric::ublas::vector< long > constant;
};

typedef boost::function< int( int ) > int_f_int;

bool is_valid_string( std::string const & str, int_f_int is_valid_char )
{
    bool valid = true;

    for ( std::string::size_type i = 0; valid && i < str.size(); ++i )
    {
        valid = is_valid_char( str[ i ] );

        if ( ! valid )
            break;
    }

    return valid;
}

bool is_valid_variable( std::string const & str )
{
   return is_valid_string( str, boost::bind( isalpha, _1 ) );
}

bool is_valid_unsigned_integer( std::string const & str )
{
   return is_valid_string( str, boost::bind( isdigit, _1 ) );
}

bool is_variable_or_unsigned_integer( std::string const & str )
{
    return( is_valid_variable( str ) || is_valid_unsigned_integer( str ) );
}

bool is_equal_sign( std::string token )
{
    return( "=" == token );
}

bool is_plus_sign( std::string token )
{
    return( "+" == token );
}

typedef boost::tokenizer< boost::char_separator< char > > tokenizer;

bool is_equation( tokenizer equation )
{
    bool valid = true;

    tokenizer::iterator token = equation.begin();

    // first token: variable
    valid = ( token != equation.end() );
    
    if ( ! valid )
        return valid;

    valid = is_valid_variable( *token );

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

    valid = is_variable_or_unsigned_integer( *token );

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

        valid = is_variable_or_unsigned_integer( *token );

        if ( ! valid )
            break;
    }

    return valid;
}

void load_input_file( std::string const & file_name, string_vector & system_of_equations )
{
    std::ifstream input_file;
    input_file.open( file_name );

    if ( input_file.is_open() )
    {
        std::string input_equation;

        while ( std::getline( input_file, input_equation ) )
        {
            system_of_equations.push_back( input_equation );
        }

        input_file.close();
    }
}

tokenizer string_to_tokens( std::string equation_as_string )
{
    boost::char_separator< char > separator( " " );
    tokenizer equation_as_tokens( equation_as_string, separator );
    return equation_as_tokens;
}

bool equations_are_valid( string_vector const & system_of_equations )
{
    bool valid = true;

    for ( string_vector::const_iterator equation_as_string = system_of_equations.begin();
          equation_as_string != system_of_equations.end();
          ++equation_as_string )
    {
        tokenizer equation_as_tokens = string_to_tokens( *equation_as_string );
        valid = is_equation( equation_as_tokens );

        if ( ! valid )
            break;
    }

    return valid;
}

void print_all_equations( string_vector const & system_of_equations )
{
    for ( string_vector::const_iterator equation_as_string = system_of_equations.begin();
         equation_as_string != system_of_equations.end();
         ++equation_as_string )
    {
        std::cout << *equation_as_string << std::endl;
    }
}

int main( int argc, char * argv[] )
{
    std::string const file_name( argv[ 1 ] );
    std::vector< std::string > input_equations;
    load_input_file( file_name, input_equations );

    // If any input equation is not valid, print all input equations.
    if ( ! equations_are_valid( input_equations ) )
    {
        print_all_equations( input_equations );
        return 0;
    }

    return 0;
}
