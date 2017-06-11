#include <cctype>
#include <iostream>
#include <string>

#include <boost/bind.hpp>
#include <boost/function.hpp>
#include <boost/numeric/ublas/matrix.hpp>
#include <boost/numeric/ublas/vector.hpp>
#include <boost/numeric/ublas/io.hpp>
#include <boost/tokenizer.hpp>

class system_of_equations
{
public:
private:
    boost::numeric::ublas::matrix<long> coefficient(long size1, long size2);
};

typedef boost::function< int( int ) > int_f_int;

bool is_valid_string( std::string const & str, int_f_int is_char )
{
    bool is_variable = true;

    for ( std::string::size_type i = 0; is_variable && i < str.size(); ++i )
    {
        is_variable = isalpha( str[ i ] );
    }

    return is_variable;
}

bool is_variable( std::string const & str )
{
   return is_valid_string( str, boost::bind( isalpha, _1 ) );
}

bool is_unsigned( std::string const & str )
{
   return is_valid_string( str, boost::bind( isdigit, _1 ) );
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
    for (tokenizer::iterator token = equation.begin(); token != equation.end(); ++token)
    {
    }
}

tokenizer string_to_tokens( std::string equation_as_string )
{
    boost::char_separator< char > separator( " " );
    tokenizer equation_as_tokens( equation_as_string, separator );
    return equation_as_tokens;
}

int main()
{
    std::string equation_as_string( "green = 3 + red + blue + 2" );
    tokenizer equation_as_tokens = string_to_tokens( equation_as_string );
    for (tokenizer::iterator token = equation_as_tokens.begin(); token != equation_as_tokens.end(); ++token)
    {
        std::cout << *token << std::endl;
    }
    return 0;
}
