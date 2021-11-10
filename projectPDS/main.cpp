#include "BayesianNetwork.cpp"
#include <exception>



int main()
{
    BayesianNetwork bn;
    bn.input("./../Asia.xdsl");
    bn.compute();

    std::vector<std::string> inferences;
    std::vector<std::string> evidences;
    inferences.push_back("Smoker");
    //inferences.push_back("Asia");
    evidences.push_back("yes");
    //evidences.push_back("no");
    bn.inference(inferences,evidences);
    bn.output();
}