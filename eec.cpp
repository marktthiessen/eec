/*
    compile:
    g++ -Wall -std=c++11 -I../boost_1_64_0/ eec.cpp -o eec

    run:
    eec
*/

#include <iostream>
#include <string>

//#include <boost/numeric/ublas/matrix.hpp>
//#include <boost/numeric/ublas/vector.hpp>
#include <boost/numeric/ublas/io.hpp>
#include <boost/tokenizer.hpp>

class system_of_equations
{
public:
private:
    //matrix<long> coefficient(size1, size2);    
};

typedef boost::tokenizer< boost::char_separator< char > > tokenizer;

tokenizer string_to_tokens( std::string equation_as_string )
{
    boost::char_separator< char > separator( " " );
    tokenizer equation_as_tokens( equation_as_string, separator );
    return equation_as_tokens;
}

bool is_variable( std::string token )
{
    // use isalpha on every character
}

bool is_equal_sign( std::string token )
{
    return( "=" == token );
}

bool is_unsigned( std::string token )
{
    // use isdigit on every character
}

bool is_plus_sign( std::string token )
{
    return( "+" == token );
}

bool is_equation( tokenizer equation_as_tokens )
{
    long i = 0;
    for ( const auto & token : equation_as_tokens )
    {
    }
}

int main()
{
    std::string equation_as_string( "green = 3 + red + blue + 2" );
    tokenizer equation_as_tokens = string_to_tokens( equation_as_string );
    for ( const auto & token : equation_as_tokens )
    {
        std::cout << token << std::endl;
    }
    return 0;
}
