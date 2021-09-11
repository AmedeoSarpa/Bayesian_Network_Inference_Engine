//
// Created by proto on 13/08/2021.
//

#ifndef PDS_NODE_HPP
#define PDS_NODE_HPP

#include <string>
#include <vector>
#include <memory>
#include <map>
#include "RealVector.hpp"
#include "Matrix.hpp"
class Node {
private:
    std::string label;
    int id;
    std::vector<std::string> valueLabes;
    RealVector bel,pi,lambda;
    std::vector<Node*> parents,children;
    Matrix _priorTable;
    std::map<int,RealVector> pi_zi_x;
    std::map<int,RealVector> lambda_x_wi;

public:

    Node() {};


    Node(const Node& source){
        id = source.id;
        label = source.label;
        valueLabes = source.valueLabes;
        bel = source.bel;
        lambda = source.lambda;
        pi = source.pi;
        parents = source.parents;
        children = source.children;
        _priorTable = source._priorTable;
        pi_zi_x = source.pi_zi_x;
        lambda_x_wi = source.lambda_x_wi;
    }

    Node(std::string label,int id, int states, std::vector<std::string> labels) : id(id){
            bel(states);
            pi(states);
            lambda(states);
            lambda.toAllOnes();
            for (int i = 0; i < states; i++) {
                valueLabes.push_back(labels.at(i));
            }
            _priorTable();
            this->label = label;
            bel.setLabels(label,labels);
            pi.setLabels(label,labels);
            lambda.setLabels(label,labels);
    }

    RealVector* getBel() {
        return &bel;
    }

    RealVector* getPi() {
        return &pi;
    }



    RealVector* getLambda() {
        return &lambda;
    }

    void setPi(RealVector v){
        pi = v;
    }

    void setLambda(RealVector v){
        lambda = v;
    }

    void setBel(RealVector v){
        bel = v;
    }

    std::string getLabel(){
        return label;
    }

    std::vector<Node*> getChildren() { return children; }

    std::vector<Node*> getParents() { return parents; }

    Matrix* getMx_wAll() {
        return &_priorTable;
    }

    void setMx_wAll(Matrix m) {
        _priorTable = m;
    }

    bool isParent(Node node) {
        for (int i = 0; i < parents.size(); i++) {
            if (parents.at(i)->id == node.id) return true;
        }
        return false;
    }


    bool isChild(Node node) {
        for (int i = 0; i < children.size(); i++) {
            if (children.at(i)->id == node.id) return true;
        }
        return false;
    }



    bool operator< (const Node &rhs) const { return id <  rhs.id; }
    bool operator==(const Node &rhs) const { return id == rhs.id; }
    bool operator!=(const Node &rhs) const { return id != rhs.id; }



    int getId() const {
        return id;
    }


    RealVector* getPi_zi_x(Node child) {
        return &pi_zi_x.at(child.id);
    }

    RealVector* getLambda_x_wi(Node parent) {
        return &lambda_x_wi.at(parent.id);

    }

    void addParent(Node &node) {
        if (!isParent(node)) {
            parents.push_back(&node);
        }
    }

    void addChild(Node &node) {
        if (!isChild(node)) {
            children.push_back(&node);
        }
    }

    void updateBEL() { //prodotto con normalizzazione
        try {
            bel.termProduct(pi, lambda);
            bel.normalise();
        }
        catch(std::exception e){
            std::cout<< "errore nell'aggiornamento BEL " << std::endl;
        }
    }


    void updatePi() {
        if (parents.size() == 0) { return; }
        pi.toAllZeros();
        Node* parent;
        RealVector* pi_z;
        try {
            for (int j = 0; j < _priorTable.getRowDimension(); j++) {
                double *tmp = new double[_priorTable.getColDimension()];

                for (int m = 0; m < _priorTable.getColDimension(); m++) {
                    tmp[m] = _priorTable.getValue(j, m);
                }

                //fin qui mi sono calcolato P(x|w)
                for (int i = 0; i < parents.size(); i++) { //produttoria sui padri
                    parent = parents.at(i);

                    pi_z = parent->getPi_zi_x(*this);
                    for (int k = 0; k < pi_z->getDimension(); k++) {

                        if (_priorTable.partOf(pi_z->getLabel(k), _priorTable.getRowLabels(j))) {
                            for (int m = 0; m < _priorTable.getColDimension(); m++) {
                                tmp[m] *= pi_z->getValue(k);
                            }

                        }

                    }
                }
                for (int m = 0; m < _priorTable.getColDimension(); m++) {
                    pi.setValue(m, pi.getValue(m) + tmp[m]);
                }
                //aggiornameto del pi
                delete[] tmp;
            }
            pi.normalise();

        }
        catch (std::exception e){
            std::cout << "errore nel calcolo del PI" << std::endl;
        }
    }


    void updateLambda(){ //produttoria dei lamda_z_j (formula 1 del sito)

        if (children.size() == 0) { return; }
        lambda.setValues(children.at(0)->getLambda_x_wi(*this)->getValues());
        try {
            for (int i = 1; i < children.size(); i++) {
                Node* child = children.at(i);
                lambda.termProduct(lambda, *(child->getLambda_x_wi(*this)));
            }
        }
        catch(std::exception e){
            std::cout << "errore nel calcolo dell update lambda" << std::endl;
        }
    }

    void updatePiZ(Node& child)
    {
        if (!isChild(child))
        {
            return;
        }

        RealVector pi_z ;

        try {
            if (!(pi_zi_x.contains(child.id))) {
                pi_z = pi; //creare operatore di assegnazione
            }
            else{
                pi_z = pi_zi_x.at(child.id);
            }
        }
        catch(std::exception e){
            std::cout << "bug in updatePiz 1" << std::endl;
        }

        if (children.size() == 1)
        {
            pi_z = pi;
            pi_z.normalise();
            pi_zi_x.insert_or_assign(child.id,pi_z);
            return;
        }

        try {
            pi_z.divide(bel, *(child.getLambda_x_wi(*this)));
        }
        catch(std::exception e){
            std::cout << "errore in updatePiZ , bug 2" << std::endl ;
        }
        pi_z.normalise();
        pi_zi_x.insert_or_assign(child.id, pi_z);
    }

    bool operator==(const Node& sx){
        return this->id == sx.id;
    }


    void updateLambdaX(Node& parent)
    {
        if (!isParent(parent))
        {
            return;
        }

        RealVector lambda_wi;
        try
        {

            if (lambda_x_wi.contains(parent.id)){
                lambda_wi = *(getLambda_x_wi(parent));
            }
            else {
                lambda_wi = RealVector(parent.getBel()->getDimension());
                lambda_wi.setLabels(parent.bel.getLabels());
            }

        }
        catch (std::exception e)
        {
            std::cout << "PearlNode.updateLambdaX: bug in here (1)!!" << "\n";
            return;
        }

        if (lambda.isAllOnes())
        {
            lambda_wi.toAllOnes();
            lambda_x_wi.insert_or_assign(parent.id, lambda_wi);
            return;
        }

        lambda_wi.toAllZeros();

        for (int i = 0; i < _priorTable.getRowDimension(); i++)
        {
            double tmp = 0;
            for (int k = 0; k < _priorTable.getColDimension(); k++)
            {
                tmp += this->_priorTable.getValue(i, k) * lambda.getValue(k); //prima modifica
            }
            for (int j = 0; j < parents.size(); j++) //sommatoria Wk con k diverso da i
            {
                Node* parent2 = parents.at(j);
                if (parent.getId() != parent2->getId())
                {
                    try
                    {
                        RealVector* pi_z = parent2->getPi_zi_x(*this);
                        for (int m = 0; m < pi_z->getDimension(); m++)
                        {
                            if (_priorTable.partOf(pi_z->getLabel(m), _priorTable.getRowLabels(i)))
                                tmp *= pi_z->getValue(m);
                        }
                    }
                    catch (std::exception e)
                    {
                        std::cout << "PearlNode.updateLambdaX: bug in here (2)!!" << std::endl;
                        return;
                    }
                }
            }

            for (int n = 0; n < lambda_wi.getDimension(); n++)
            {
                if (_priorTable.partOf(lambda_wi.getLabel(n), _priorTable.getRowLabels(i)))
                {
                    lambda_wi.setValue(n, lambda_wi.getValue(n) + tmp);
                    break;
                }
            }
        }

        try
        {
            lambda_x_wi.insert_or_assign(parent.id, lambda_wi);
        }
        catch (std::exception e)
        {
            std::cout << "PearlNode.updateLambdaX: bug in here!! (3)"<< std::endl;
            return;
        }

    }
};

#endif //PDS_NODE_HPP
