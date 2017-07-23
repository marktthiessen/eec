#ifndef LINEAR_SYSTEM_H
#define LINEAR_SYSTEM_H

#include <boost/numeric/ublas/matrix.hpp>
#include <boost/numeric/ublas/matrix_proxy.hpp>

#include "input-equation-vector.h"
#include "variable-set.h"

// Our System of Linear Equations
class LinearSystem
{
public:
    typedef boost::numeric::ublas::matrix< Element > AugmentedMatrix;
    typedef boost::numeric::ublas::matrix_row< AugmentedMatrix > AugmentedMatrixRow;
    typedef boost::numeric::ublas::matrix_column< AugmentedMatrix > AugmentedMatrixColumn;

    void convertInputToMatrix( InputEquationVector const & inputEquation );
    void printMatrix();
    void solveByGaussianElimination();
    void convertMatrixToResults();
    void outputResults();

private:
    typedef Index Dimensions;

    void incrementHeight();
    void addVariable( int side, Dimensions row, Dimensions column );
    void increaseWidth( Dimensions newWidth );
    void addConstant( Dimensions row, Element constant );

    AugmentedMatrix augMatrix; // augmented matrix of coefficients and constants
    VariableSet variable;

    static int const left_side;
    static int const right_side;
};

#endif
