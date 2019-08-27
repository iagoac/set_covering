#ifndef CPLEX_INCLUES_HH_
#define CPLEX_INCLUDES_HH_

/* Include of the CPLEX libraries */
#include <ilconcert/ilomodel.h>
#include <ilcplex/ilocplex.h>

/* Definition of CPLEX multi-dimensional vector types */
typedef IloArray<IloNumVarArray> IloNumVarMatrix;
typedef IloArray<IloNumVarMatrix> IloNumVarMatrix3D;
typedef IloArray<IloNumVarMatrix3D> IloNumVarMatrix4D;

typedef IloArray<IloBoolVarArray> IloBoolVarMatrix;
typedef IloArray<IloBoolVarMatrix> IloBoolVarMatrix3D;
typedef IloArray<IloBoolVarMatrix3D> IloBoolVarMatrix4D;

typedef IloArray<IloIntVarArray> IloIntVarMatrix;
typedef IloArray<IloIntVarMatrix> IloIntVarMatrix3D;
typedef IloArray<IloIntVarMatrix3D> IloIntVarMatrix4D;

#endif
