//
// Created by proto on 13/08/2021.
//

#ifndef PDS_NODE_HPP
#define PDS_NODE_HPP

#include <string>
#include <vector>
#include <cmath> // pow
#include <memory>
class Node {


public:
    void computeBel(){
        if (_pi.first ==-1  || _pi.second ==-1   ){
            this->computePi();
        }
        if (_lambda.first ==-1  || _lambda.second ==-1   ){
            this->computeLambda();
        }
        _bel.first = (_lambda.first * _pi.first) ;
        _bel.second = (_lambda.second * _pi.second);
    }
    void computePi(){
        float productValue = 1;
        //vector = product by product computePi_i(children of this node).       iterare con un for sui figli ed invocare tale funzione
        // product vector per matrix (_priorTable,vector)




    };

    void computePi_i(Node yj,Node x) //dove se invocata dal calcolo di pi , x è il nodo corrente mentre yj è il padre
    {
        //


        //valore finale è pi di x per computeLambda_i di tutti i figli k di node yj escluso il nodo corrente, fare anche qui un for

    };

    void computeLambda_i(Node yj,Node x){ //se invoacata dal calcolo di pi_i , x è  è il padre corrente  (yj di pi_i) mentre yj è il fratello k del nodo corrente

        //lamda di yj per la prior table di yj



    }

    void computeLambda(){




    };

    Node(int id,std::string label , int nChildren, int nParents, float * values):  _id(id),_label(label), _pi({-1,-1}), _lambda({-1,-1}) , _bel({-1,-1}) , _nChildren(nChildren), _nParents(nParents){
        _priorTable = new float*[static_cast<int>(pow(2,nChildren))];
        for (int i = 0; i < pow(2,_nChildren); i++)
            _priorTable[i] = new float[2];

        int j = 0;
        for (int i = 0 ; i  < pow(2,_nChildren); i++){
            _priorTable[i][0] = values[j++];
            _priorTable[i][1] = values[j++];
        }
    };

    bool operator< (const Node &rhs) const { return _id <  rhs._id; }
    bool operator==(const Node &rhs) const { return _id == rhs._id; }
    bool operator!=(const Node &rhs) const { return _id != rhs._id; }
    ~Node() {
        for (int i = 0 ; i < pow(2,_nChildren) ; i++){
            delete [] _priorTable[i];
        }
        delete [] _priorTable;
    }

    void addParent(std::shared_ptr<Node> p){
        parents.push_back(p);
    }
    void addChild(std::shared_ptr<Node> c){
        children.push_back(c);
    }

    float** getPriorTable(){
        return _priorTable;
    }


    int getId() const {
        return _id;
    }

private:
    std::vector<std::shared_ptr<Node>> parents;
    std::vector<std::shared_ptr<Node>> children;
    std::string _label;
    int _id;
    int _nChildren;
    int _nParents;
    std::pair<float,float> _pi;
    std::pair<float,float> _lambda;
    std::pair<float,float> _bel;
    float** _priorTable;
};

#endif //PDS_NODE_HPP
