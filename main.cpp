#include "BayesianNetwork.cpp"
#include <exception>

int main() {
  BayesianNetwork bn;
  bn.input("./../IOFiles/Coma.xdsl");
  bn.compute();
  bn.output();
  std::vector<std::string> inferences;
  std::vector<std::string> evidences;
  inferences.push_back("MetastCancer");
  //inferences.push_back("Asia");
  evidences.push_back("yes");
  //evidences.push_back("no");
  bn.inference(inferences,evidences);
  bn.output();
}