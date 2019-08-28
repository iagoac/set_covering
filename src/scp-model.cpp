#include "scp-model.hpp"

void SCP::initialize(void) {
  #ifdef DEBUG
    std::cout << "Constructing the problem variables: ";
  #endif

  this->create_variables();

  #ifdef DEBUG
    std::cout << "Done!" << std::endl;
    std::cout << "Constructing the objective function: ";
  #endif

  this->add_objective();

  #ifdef DEBUG
    std::cout << "Done!" << std::endl;
    std::cout << "Adding the covering constraints:" << std::endl;
  #endif

  this->covering_constraints();

  #ifdef DEBUG
    std::cout << "Done!" << std::endl;
    std::cout << "Setting CPLEX parameters: ";
  #endif

  this->add_cplex_params();

  #ifdef DEBUG
    std::cout << "Done!" << std::endl;
  #endif

  /* If debug option is on */
  #ifdef DEBUG
    /* Then, export the CPLEX model into file mymodel.lp */
    this->_cplex.exportModel("mymodel.lp");
  #else
    /* Else, do not print the
    algorithm on the std::out */
    std::ofstream logfile("/dev/null");
    this->_cplex.setOut(logfile);
    this->_cplex.setWarning(logfile);
  #endif
}

/* Create and initialize the CPLEX variables */
void SCP::create_variables(void) {
  /* Loop variables */
   uint i;

  /* x_{ij} \in N */
  _x = IloIntVarArray(this->_env, this->cols);
  for (i = 0; i < this->cols; i++) {
    char name[10];
    sprintf(name, "x_%d", i);
    _x[i] = IloIntVar(this->_env, 0, 1, name);
  }
}

/* Create the objective function */
void SCP::add_objective(void) {
  IloNumExpr expr(this->_env);

  for (int s = 0; s < this->cols; s++) {
    expr += _x[s];
  }

  this->_model.add(IloMinimize(this->_env, expr));
  expr.end();
}

/* Add all CPLEX constraints */
void SCP::covering_constraints() {
  for (int i = 0; i < this->rows; i++) {
    IloNumExpr expr(this->_env);
    for (int s = 0; s < this->cols; s++) {
        expr += (this->_instance[i][s] * this->_x[s]);
    }
    this->_model.add(expr >= 1);
    expr.end();
  }
}

void SCP::add_cplex_params(void) {
  this->_cplex.setParam(IloCplex::TiLim, this->_args->get<int>("-time"));
  this->_cplex.setParam(IloCplex::MIPEmphasis, this->_args->get<int>("-switch"));
  this->_cplex.setParam(IloCplex::Threads, 1);
  // this->_cplex.setParam(IloCplex::Cliques, -1);
  // this->_cplex.setParam(IloCplex::Covers, -1);
  // this->_cplex.setParam(IloCplex::DisjCuts, -1);
  // this->_cplex.setParam(IloCplex::FlowCovers, -1);
  // this->_cplex.setParam(IloCplex::FlowPaths, -1);
  // this->_cplex.setParam(IloCplex::GUBCovers, -1);
  // this->_cplex.setParam(IloCplex::ImplBd, -1);
  // this->_cplex.setParam(IloCplex::MIRCuts, -1);
  // this->_cplex.setParam(IloCplex::FracCuts, -1);
  // this->_cplex.setParam(IloCplex::ZeroHalfCuts, -1);
  // this->_cplex.setParam(IloCplex::MIPSearch, CPX_MIPSEARCH_TRADITIONAL);
  // this->_cplex.setParam(IloCplex::MIPInterval, 100);
  // this->_cplex.setParam(IloCplex::PreInd, 0);
  // this->_cplex.setParam(IloCplex::AggInd, 0);
  // this->_cplex.setParam(IloCplex::HeurFreq, -1);
  // this->_cplex.setParam(IloCplex::EpAGap, 0);
}
