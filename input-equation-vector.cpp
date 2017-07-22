#include <fstream> // open, close
#include <iostream> // cout, endl

#include <boost/bind.hpp>
#include <boost/foreach.hpp>
#include <boost/function.hpp>
#include <boost/tokenizer.hpp>

#include "input-equation-vector.h"

Tokenizer stringToTokens( std::string const & equationAsString )
{
    boost::char_separator< char > separator( " " );
    Tokenizer equationAsTokens( equationAsString, separator );
    return equationAsTokens;
}

const unsigned InputEquationVector::maxVariableNameLength = 10;

void InputEquationVector::loadInputFile( std::string const & fileName )
{
    std::ifstream input_file;
    input_file.open( fileName.c_str() );
    
    if ( input_file.is_open() )
    {
        std::string input_equation;
        
        while ( std::getline( input_file, input_equation ) )
        {
            inputEquationVector.push_back( input_equation );
        }
        
        input_file.close();
    }
}

bool InputEquationVector::equationsAreValid() const
{
    bool valid = true;
    
    BOOST_FOREACH ( std::string equationAsString, inputEquationVector )
    {
        Tokenizer equationAsTokens = stringToTokens( equationAsString );
        valid = isEquation( equationAsTokens );
        
        if ( ! valid )
            break;
    }
    
    return valid;
}

void InputEquationVector::printAllEquations() const
{
    BOOST_FOREACH ( std::string equationAsString, inputEquationVector )
    {
        std::cout << equationAsString << std::endl;
    }
}

InputEquationVector::VectorOfStrings const & InputEquationVector::asVector() const
{
    return inputEquationVector;
}

bool InputEquationVector::isEquation( Tokenizer const & equation )
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

bool InputEquationVector::isValidVariableNameOrUnsignedInteger( std::string const & str )
{
    return( isValidVariableName( str ) || isValidUnsignedInteger( str ) );
}

bool InputEquationVector::isValidVariableName( std::string const & str )
{
    return isValidString( str, boost::bind( isalpha, _1 ) );
}

bool InputEquationVector::isValidUnsignedInteger( std::string const & str )
{
    return isValidString( str, boost::bind( isdigit, _1 ) );
}

bool InputEquationVector::isValidString( std::string const & str, IntFunctionInt isValidChar )
{
    if ( str.size() > maxVariableNameLength )
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

bool InputEquationVector::isEqualSign( std::string const & token )
{
    return( "=" == token );
}

bool InputEquationVector::isPlusSign( std::string const & token )
{
    return( "+" == token );
}
