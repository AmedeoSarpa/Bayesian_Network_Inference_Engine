//
// Created by proto on 13/08/2021.
//

#ifndef PDS_NODE_HPP
#define PDS_NODE_HPP

#include <string>
#include <vector>
#include <math.h> // pow
class Node {
public:
    void computeBel(){
        if (_pi.first ==-1  || _pi.second ==-1   ){
            this->computePi();
        }
        if (_lambda.first ==-1  || _lambda.second ==-1   ){
            this->computeLambda();
        }
        _bel.first = (_lambda.first * _pi.first) * _alpha;
        _bel.second = (_lambda.second * _pi.second) * _alpha;
    }
    void computePi(){
        float productValue = 1;
        for (int i = 0 ; i < parents ; i++){

        }




    };

    void computeLambda(){




    };

    Node(int id,std::string label , int nChildren, int nParents):  _id(id),_label(label), _pi({-1,-1}), _lambda({-1,-1}) , _bel({-1,-1}) , _nChildren(nChildren), _nParents(nParents){
        priorTable = new float*[pow(2,nChildren)];
        for (int i = 0; i < pow(2,nChildren); i++)
            priorTable[i] = new float[2];
    };
    bool operator< (const Node &rhs) const { return _id <  rhs._id; }
    bool operator==(const Node &rhs) const { return _id == rhs._id; }
    bool operator!=(const Node &rhs) const { return _id != rhs._id; }
    ~Node() {
        for (int i = 0 ; i < pow(2,nChildren) ; i++){
            delete [] _priorTable[i];
        }
        delete [] _priorTable;
    }
    static float _alpha ;
    void addParent(Node p){
        parents.push_back(p);
    }
    void addChild(Node c){
        children.push_back(c);
    }

private:
    std::vector<Node> parents;
    std::vector<Node> children;
    std::string _label;
    int _id;
    int _nChildren;
    int _nParents;
    std::pair<float,float> _pi;
    std::pair<float,float> _lambda;
    std::pair<float,float> _bel;
    float** _priorTable;
};

Node::_alpha = 1;

#endif //PDS_NODE_HPP
