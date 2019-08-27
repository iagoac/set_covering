#include <stdio.h>
#include <stdlib.h>
#include "scp-model.hpp"

/* Substituir pela função de arthur */
std::vector<std::vector<int> > read(std::string filename) {
  std::vector<std::vector<int> > m(10, std::vector<int>(10));
  m[2][3] = 5;
  return (m);
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
  #endif

  /* Get the instance matrix */
  std::vector<std::vector<int> > instance = read(args.get<std::string>("-input"));

  /* Initialize the Set Covering model */
  SCP scp(instance, &args);

  scp.solve();

  std::cout << args.get<std::string>("-input") << "," << std::endl;
  std::cout << args.get<int>("-time") << "," << std::endl;
  std::cout << args.get<int>("-switch") << "," << std::endl;
  // std::cout << << "," << std::endl;
  // std::cout << << "," << std::endl;
  // std::cout << << "," << std::endl;
  // std::cout << << "," << std::endl;
  return (0);
}
