#ifndef SCP_HH_
#define SCP_HH_

#include "cplex_includes.hpp"
#include "argparse.hpp"

class SCP {
private:
  /* Model building methods */
  void create_variables(void);
  void add_objective(void);
  void covering_constraints(void);
  void add_cplex_params(void);

public:
  /* Class variables declaration */
  std::vector<std::vector<int> > _instance;  // The input instance
  const Args *_args;                          // The arguments class
  int rows, cols;

  /* CPLEX common structures */
  IloEnv _env;
  IloModel _model;
  IloCplex _cplex;

  /* CPLEX variables */
  IloIntVarArray _x;

  /* Class constructor */
  SCP (std::vector<std::vector<int> > instance, const Args* arg)
  : _instance(instance), _args(arg), _model(this->_env), _cplex(this->_model) {
    this->initialize();
  }

  /* Method used by the constructor:
  calls other methods to initialize
  the mathematical model               */
  void initialize(void);

  /* Problem-solving functions */
  void solve(void) { this->_cplex.solve(); }

};
#endif // SCP_HH_
