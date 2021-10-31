#include "BayesianNetwork.hpp"

int main()
{
    BayesianNetwork bn;
    bn.input("./../Asia.xdsl");
    bn.start();
    //bn.inference("Smoker",0);
    //bn.inference("H",0);
    //bn.inference("W",0);
    bn.output();
}

/*COMMENTI INFERENZA
 * - nodo foglia errato se c'è un graph not singly connected (es. Asia)
 * - da implementare: doppia (o più) inferenza contemporanea
 * - errori printati
 * - a volte il run fallisce proprio
 */