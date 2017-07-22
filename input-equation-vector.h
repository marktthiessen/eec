#ifndef INPUT_EQUATION_VECTOR_H
#define INPUT_EQUATION_VECTOR_H

#include <string>
#include <vector>

typedef boost::tokenizer< boost::char_separator< char > > Tokenizer;
Tokenizer stringToTokens( std::string const & equationAsString );

class InputEquationVector
{
    typedef std::vector< std::string > VectorOfStrings;
    static unsigned const maxVariableNameLength;

public:
    typedef VectorOfStrings::size_type Size;

    void loadInputFile( std::string const & fileName );
    bool equationsAreValid() const;
    void printAllEquations() const;
    VectorOfStrings const & asVector() const;

private:
    typedef boost::function< int( int ) > IntFunctionInt;

    static bool isEquation( Tokenizer const & equation );
    static bool isValidVariableNameOrUnsignedInteger( std::string const & str );
    static bool isValidVariableName( std::string const & str );
    static bool isValidUnsignedInteger( std::string const & str );
    static bool isValidString( std::string const & str, IntFunctionInt isValidChar );
    static bool isEqualSign( std::string const & token );
    static bool isPlusSign( std::string const & token );

    VectorOfStrings inputEquationVector;
};

#endif
