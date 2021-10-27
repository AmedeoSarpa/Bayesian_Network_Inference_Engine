#include "BayesianNetwork.hpp"



int main()
{
    BayesianNetwork<int> bn;
    bn.input("./../NoisyOR.xdsl");
    bn.start();
    bn.inference("FuelPressHigh",0);
    //bn.inference("Coma",0);
    //bn.inference("W",0);
    //bn.print();
    bn.output();
}