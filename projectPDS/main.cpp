#include "BayesianNetwork.hpp"



int main()
{
    BayesianNetwork bn;
    bn.input("./../Grass.xdsl");
    bn.start();
    //bn.inference("Smoker",0);
    //bn.inference("H",0);
    //bn.inference("W",0);
    //bn.print();
    bn.output();
}