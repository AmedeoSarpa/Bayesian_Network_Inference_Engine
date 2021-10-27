#include "BayesianNetwork.hpp"



int main()
{
    BayesianNetwork<int> bn;
    bn.input("./../EngineFuelSystem.xdsl");
    bn.start();
    bn.inference("LoadTest",0);
    //bn.inference("Radar",1);
    //bn.inference("Coma",0);
    //bn.inference("W",0);
    bn.output();
}

/*COMMENTI INFERENZA
 * - nodo foglia errato se c'è un graph not singly connected (es. Asia)
 * - da implementare: doppia (o più) inferenza contemporanea
 * - errori printati
 * - a volte il run fallisce proprio
 */