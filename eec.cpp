#include <string>

#include "input-equation-vector.h"
#include "linear-system.h"

class eec
{
public:
    void getEquations( std::string const & fileName );
    void solveEquations();
    void printInputEquations();

private:
    InputEquationVector equation;
    LinearSystem system;
};

void eec::getEquations( std::string const & fileName )
{
    equation.loadInputFile( fileName );
    equation.checkEquations();
}

void eec::solveEquations()
{
    system.convertInputToMatrix( equation );
#ifdef SHOW_CALCULATIONS
    system.printMatrix();
#endif
    system.solveByGaussianElimination();
#ifdef SHOW_CALCULATIONS
    system.printMatrix();
#endif
    system.convertMatrixToResults();
    system.outputResults();
}

void eec::printInputEquations()
{
    equation.printAllEquations();
}

int main( int argc, char * argv[] )
{
    if ( argc < 2 ) // no input file
        return 0;

    std::string const fileName( argv[ 1 ] );
    eec evaluator;

    try
    {
        evaluator.getEquations( fileName );
        evaluator.solveEquations();
    }

    catch( std::exception )
    {
        evaluator.printInputEquations();
    }

    return 0;
}
