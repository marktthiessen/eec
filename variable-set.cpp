#include <exception> // length_error

#include "variable-set.h"

Index VariableSet::indexFor( std::string const & name )
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
        indexOf[ name ] = index; // add name and its index
    }
    else
    {
        index = indexOf[ name ];
    }

    return index;
}

Element & VariableSet::operator[]( Index const index )
{
    return variableSet[ index ].value;
}

std::string VariableSet::asName( Index const index )
{
    return variableSet[ index ].name;
}

std::vector< Variable > VariableSet::asVector()
{
    return variableSet;
}

bool operator<( Variable lhs, Variable rhs )
{
    return lhs.name < rhs.name;
}
