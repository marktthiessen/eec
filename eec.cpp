#include <string>

#include "input-equation-vector.h"
#include "linear-system.h"

int main( int argc, char * argv[] )
{
    if ( argc < 2 ) // no input file
        return 0;

    InputEquationVector inputEquation;
    std::string const fileName( argv[ 1 ] );
    inputEquation.loadInputFile( fileName );

    // If any input equation is not valid, print all input equations.
    if ( ! inputEquation.equationsAreValid() )
    {
        inputEquation.printAllEquations();
        return 0;
    }

    LinearSystem system;
    system.convertInputToMatrix( inputEquation );
#ifdef SHOW_CALCULATIONS
    system.printMatrix();
#endif
    system.solveByGaussianElimination();
#ifdef SHOW_CALCULATIONS
    system.printMatrix();
#endif
    system.convertMatrixToResults();
    system.outputResults();

    return 0;
}
