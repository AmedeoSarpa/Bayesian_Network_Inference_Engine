#include "BayesianNetwork.hpp"



int main()
{
    BayesianNetwork<int> bn;
    bn.input("./../Asia.xdsl");
    bn.start();
    bn.inference("Cancer",0);
    //bn.inference("W",0);
    //bn.print();
    bn.output();
}