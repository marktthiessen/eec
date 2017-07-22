#ifndef VARIABLE_SET_H
#define VARIABLE_SET_H

#include <map>
#include <string>
#include <vector>

typedef long double Element;
typedef uint8_t Index;
typedef Index Dimensions;
static Index const maximumDimensions = 255;

struct Variable
{
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
    Index indexFor( std::string const & name );
    Element & operator[]( Index const index );
    std::string asName( Index const index );
    std::vector< Variable > asVector();

private:
    std::vector< Variable > variableSet;
    std::map< std::string, Index > indexOf; // for faster index lookups than searching variableSet
};

bool operator<( Variable lhs, Variable rhs );

#endif
