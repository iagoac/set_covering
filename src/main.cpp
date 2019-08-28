#include <stdio.h>
#include <stdlib.h>
#include "scp-model.hpp"

/* Read the OR-LIBRARY instance file */
std::vector<std::vector<int> > read(std::string filename) {
  std::ifstream myfile;
  int j = 0;
  int n_rows, n_columns, n_covered, aux;

  myfile.open(filename, std::ifstream::in);

  if (!myfile.is_open()) {
      std::cout << "Error. File " << filename << " not found. Aborting." << std::endl;
      exit(1);
  }
  myfile >> n_rows >> n_columns;
  std::vector<std::vector<int> > matrix(n_rows, std::vector<int> (n_columns, 0));

  for (int i = 0; i < n_columns; i++) {
      myfile >> aux;
  }

  for (int i = 0; i < n_rows; i++) {
    myfile >> n_covered;
    for (int c = 0; c < n_covered; c++) {
        myfile >> j;
        matrix[i][j] = 1;
    }
  }

  return matrix;
}

int main(int argc, char* const* argv) {
  /* Declare the arguments parser */
  ArgParse argparser;

  /* Get the arguments */
  argparser.add("-input", "");
  argparser.add("-time", "");
  argparser.add("-switch", "");

  Args args = argparser.parse(argc, argv);

  /* Debug initial message */
  #ifdef DEBUG
    std::cout << "Debug is activated!" << std::endl;
    std::cout << "Solving instance " << args.get<std::string>("-input") << std::endl;
    std::cout << "Reading the instance file: ";
  #endif

  /* Get the instance matrix */
  std::vector<std::vector<int> > instance = read(args.get<std::string>("-input"));

  #ifdef DEBUG
    std::cout << "Done!" << std::endl;
  #endif

  /* Initialize the Set Covering model */
  SCP scp(instance, &args);

  std::cout << "Montei o modelo" << std::endl;

  scp.solve();

  std::cout << args.get<std::string>("-input") << ",";
  std::cout << scp.rows << ",";
  std::cout << scp.cols << ",";
  std::cout << scp.rows * scp.cols << ",";
  std::cout << args.get<int>("-time") << ",";
  std::cout << args.get<int>("-switch") << ",";
  std::cout << scp._cplex.getBestObjValue() << ",";
  std::cout << scp._cplex.getObjValue() << ",";
  std::cout << scp._cplex.getMIPRelativeGap()*100 << ",";
  std::cout << scp._cplex.getTime() << ",";
  std::cout << scp._cplex.getStatus() << std::endl;
  return (0);
}
