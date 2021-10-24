//
// Created by proto on 13/08/2021.
//

#ifndef PDS_NODE_HPP
#define PDS_NODE_HPP


#include <iostream>                  // for std::cout
#include <utility>                   // for std::pair
#include <algorithm>                 // for std::for_each
#include <string>
#include <vector>
#include <memory>
#include <map>
#include "RealVector.cpp"
#include "Matrix.cpp"
#include <condition_variable>
#include <mutex>
#include <boost/graph/graph_traits.hpp>
#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/graphviz.hpp>
#include <vector>
#include <ctime>
#include <algorithm>
#include <boost/graph/depth_first_search.hpp>
#include <boost/pending/indirect_cmp.hpp>
#include <boost/algorithm/string.hpp>
#include <boost/lexical_cast.hpp>
#include "pugixml.hpp"
#include <boost/lexical_cast.hpp>
#define PATH "./.."



template <typename T> class Node {
    struct  NodeData{

        std::string label;
        int id;
        std::vector<std::string> valueLabes;
        RealVector bel,pi,lambda;
        std::vector<std::shared_ptr<Node>> parents,children;
        Matrix _priorTable;
        std::map<int,std::shared_ptr<RealVector>> pi_zi_x;
        std::map<int,std::shared_ptr<RealVector>> lambda_x_wi;

    };
private:
    /*
    std::string label;
    int id;
    std::vector<std::string> valueLabes;
    RealVector bel,pi,lambda;
    std::vector<std::shared_ptr<Node>> parents,children;
    Matrix _priorTable;
    std::map<int,std::shared_ptr<RealVector>> pi_zi_x;
    std::map<int,std::shared_ptr<RealVector>> lambda_x_wi;
    */

    std::shared_ptr<NodeData> nData;


public:

    Node(const Node& source){
        /*
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
         */
        nData = source.nData;
    }

    Node(std::string label,int id, int states) {
            nData = std::make_shared<NodeData>();
            nData->id = id;
            nData->bel(states);
            nData->pi(states);
            nData->lambda(states);
            nData->lambda.toAllOnes();
            nData->label = label;
            std::vector<std::string> labels ;
            labels.push_back(label+"=y");
            labels.push_back(label+"=n");
            for (int i = 0; i < states; i++) {
                nData->valueLabes.push_back(labels.at(i));
            }
        nData->_priorTable();
        nData->bel.setLabels(label,labels);
        nData->pi.setLabels(label,labels);
        nData->lambda.setLabels(label,labels);
    }

    Node& operator=(const Node& source){
        if(this!= &source){
            /*
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
             */
            nData = source.nData;
        }
        return *this;
    }

    std::shared_ptr<RealVector> getBel() {
        return std::make_shared<RealVector>(nData->bel);
    }

    //Vedere questa cosa
    RealVector* getPi() {
        return &nData->pi;
    }

    //Vedere questa cosa
    RealVector* getLambda() {
        return &(nData->lambda);
    }


    std::string getLabel(){
        return nData->label;
    }
    std::vector<std::shared_ptr<Node>> getChildren() { return nData->children; }

    std::vector<std::shared_ptr<Node>> getParents() { return nData->parents; }

    std::shared_ptr<Matrix> getMx_wAll() {
        return std::make_shared<Matrix>(nData->_priorTable);
    }


    void setMx_wAll(Matrix m) {
        nData->_priorTable = m;
    }

    bool isParent(const Node& node) {
        for (int i = 0; i < nData->parents.size(); i++) {
            if (nData->parents.at(i)->nData->id == node.nData->id) return true;
        }
        return false;
    }


    bool isChild(const Node& node) {
        for (int i = 0; i < nData->children.size(); i++) {
            if (nData->children.at(i)->nData->id == node.nData->id) return true;
        }
        return false;
    }

    bool operator< (const Node &rhs) const { return nData->id <  rhs.nData->id; }
    bool operator==(const Node &rhs) const { return nData->id == rhs.nData->id; }
    bool operator!=(const Node &rhs) const { return nData->id != rhs.nData->id; }

    int getId() const {
        return nData->id;
    }

    std::shared_ptr<RealVector> getPi_zi_x(Node child) {
        return nData->pi_zi_x.at(child.nData->id);
    }

    std::shared_ptr<RealVector> getLambda_x_wi(Node parent) {
        return nData->lambda_x_wi.at(parent.nData->id);

    }

    void addParent(std::shared_ptr<Node> node) {
        if (!isParent(*node)) {

            if (nData.operator bool() == false || nData.unique() == false){
                std::shared_ptr<NodeData> source = nData;
                nData = std::make_shared<NodeData>();
                nData->id = source->id;
                nData->label = source->label;
                nData->valueLabes = source->valueLabes;
                nData->bel = source->bel;
                nData->lambda = source->lambda;
                nData->pi = source->pi;
                nData->parents = source->parents;
                nData->children = source->children;
                nData->_priorTable = source->_priorTable;
                nData->pi_zi_x = source->pi_zi_x;
                nData->lambda_x_wi = source->lambda_x_wi;

            }

            nData->parents.push_back(node);
        }
    }

    void addChild(std::shared_ptr<Node> node) {
        if (!isChild(*node)) {
            if (nData.operator bool() == false || nData.unique() == false){
                std::shared_ptr<NodeData> source = nData;
                nData = std::make_shared<NodeData>();
                nData->id = source->id;
                nData->label = source->label;
                nData->valueLabes = source->valueLabes;
                nData->bel = source->bel;
                nData->lambda = source->lambda;
                nData->pi = source->pi;
                nData->parents = source->parents;
                nData->children = source->children;
                nData->_priorTable = source->_priorTable;
                nData->pi_zi_x = source->pi_zi_x;
                nData->lambda_x_wi = source->lambda_x_wi;

            }
            nData->children.push_back(node);
        }
    }

    //terminated
    void updateBEL() { //prodotto con normalizzazione
        if (nData.operator bool() == false || nData.unique() == false){
            std::shared_ptr<NodeData> source = nData;
            nData = std::make_shared<NodeData>();
            nData->id = source->id;
            nData->label = source->label;
            nData->valueLabes = source->valueLabes;
            nData->bel = source->bel;
            nData->lambda = source->lambda;
            nData->pi = source->pi;
            nData->parents = source->parents;
            nData->children = source->children;
            nData->_priorTable = source->_priorTable;
            nData->pi_zi_x = source->pi_zi_x;
            nData->lambda_x_wi = source->lambda_x_wi;

        }
        try {
            nData->bel.termProduct(nData->pi, nData->lambda);
            nData->bel.normalise();
        }
        catch(std::exception e){
            nData->bel.setValue(0,0);
            nData->bel.setValue(1,0);
            std::cout<< "errore nell'aggiornamento BEL " << std::endl;
        }
    }

    //terminated
    void updatePi() {
        if (nData.operator bool() == false || nData.unique() == false){
            std::shared_ptr<NodeData> source = nData;
            nData = std::make_shared<NodeData>();
            nData->id = source->id;
            nData->label = source->label;
            nData->valueLabes = source->valueLabes;
            nData->bel = source->bel;
            nData->lambda = source->lambda;
            nData->pi = source->pi;
            nData->parents = source->parents;
            nData->children = source->children;
            nData->_priorTable = source->_priorTable;
            nData->pi_zi_x = source->pi_zi_x;
            nData->lambda_x_wi = source->lambda_x_wi;

        }
        if (nData->parents.size() == 0) { return; }
        nData->pi.toAllZeros();
        std::shared_ptr<Node> parent ;
        std::shared_ptr<RealVector> pi_z;
        try {
            for (int j = 0; j < nData->_priorTable.getRowDimension(); j++) {
                std::shared_ptr<double[]> tmp = std::shared_ptr<double[]>(new double[nData->_priorTable.getColDimension()]);
                for (int m = 0; m < nData->_priorTable.getColDimension(); m++) {
                    tmp[m] = nData->_priorTable.getValue(j, m);
                }

                //fin qui mi sono calcolato P(x|w)
                for (int i = 0; i < nData->parents.size(); i++) { //produttoria sui padri
                    parent = nData->parents.at(i);
                    pi_z = parent->getPi_zi_x(*this);
                    for (int k = 0; k < pi_z->getDimension(); k++) {

                        if (nData->_priorTable.partOf(pi_z->getLabel(k), nData->_priorTable.getRowLabels(j))) {
                            for (int m = 0; m < nData->_priorTable.getColDimension(); m++) {
                                tmp[m] *= pi_z->getValue(k);
                            }

                        }

                    }
                }
                for (int m = 0; m < nData->_priorTable.getColDimension(); m++) {
                    nData->pi.setValue(m, nData->pi.getValue(m) + tmp[m]);
                }
                //aggiornameto del pi
                tmp.reset();
            }
            nData->pi.normalise();

        }
        catch (std::exception e){
            nData->pi.setValue(0,0);
            nData->pi.setValue(1,0);
            std::cout << "errore nell aggiornamento del PI" << std::endl;
        }
    }

    //terminated
    void updateLambda(){ //produttoria dei lamda_z_j (formula 1 del sito)
        if (nData.operator bool() == false || nData.unique() == false){
            std::shared_ptr<NodeData> source = nData;
            nData = std::make_shared<NodeData>();
            nData->id = source->id;
            nData->label = source->label;
            nData->valueLabes = source->valueLabes;
            nData->bel = source->bel;
            nData->lambda = source->lambda;
            nData->pi = source->pi;
            nData->parents = source->parents;
            nData->children = source->children;
            nData->_priorTable = source->_priorTable;
            nData->pi_zi_x = source->pi_zi_x;
            nData->lambda_x_wi = source->lambda_x_wi;

        }
        if (nData->children.size() == 0) { return; }
        nData->lambda.setValues(nData->children.at(0)->getLambda_x_wi(*this)->getValues());
        try {
            for (int i = 1; i < nData->children.size(); i++) {
                std::shared_ptr<Node> child = nData->children.at(i);
                nData->lambda.termProduct(nData->lambda, *(child->getLambda_x_wi(*this)));
            }
        }
        catch(std::exception e){
            nData->lambda.setValue(0,0);
            nData->lambda.setValue(1,0);
            std::cout << "errore nell  update lambda" << std::endl;
        }
    }

    //terminated
    void updatePiZ(Node& child)
    {
    if (nData.operator bool() == false || nData.unique() == false){
        std::shared_ptr<NodeData> source = nData;
        nData = std::make_shared<NodeData>();
        nData->id = source->id;
        nData->label = source->label;
        nData->valueLabes = source->valueLabes;
        nData->bel = source->bel;
        nData->lambda = source->lambda;
        nData->pi = source->pi;
        nData->parents = source->parents;
        nData->children = source->children;
        nData->_priorTable = source->_priorTable;
        nData->pi_zi_x = source->pi_zi_x;
        nData->lambda_x_wi = source->lambda_x_wi;

    }

        if (!isChild(child))
        {
            return;
        }

        std::shared_ptr<RealVector> pi_z ;

        try {
            if (!(nData->pi_zi_x.contains(child.nData->id))) {
                pi_z = std::make_shared<RealVector>(nData->pi); //creare operatore di assegnazione
            }
            else{
                pi_z = nData->pi_zi_x.at(child.nData->id);
            }
        }
        catch(std::exception e){
            pi_z = std::make_shared<RealVector>(nData->pi);
            std::cout << "errore in updatePiz : non viene ritornato nessun elemento con quel child.id  " << std::endl;
        }

        if (nData->children.size() == 1)
        {
            pi_z = std::make_shared<RealVector>(nData->pi);
            pi_z->normalise();
            nData->pi_zi_x.insert_or_assign(child.nData->id,pi_z);
            return;
        }

        try {
            pi_z->divide(nData->bel, *(child.getLambda_x_wi(*this)));
        }
        catch(std::exception e){
            pi_z = std::make_shared<RealVector>(nData->pi);
            pi_z->normalise();
            std::cout << "errore in updatePiZ , divisione non valida" << std::endl ;
        }
        pi_z->normalise();
        nData->pi_zi_x.insert_or_assign(child.nData->id, pi_z);
    }

    bool operator==(const Node& sx){
        return this->nData->id == sx.nData->id;
    }

    void printValues(){
        if (nData.operator bool() == false || nData.unique() == false){
            std::shared_ptr<NodeData> source = nData;
            nData = std::make_shared<NodeData>();
            nData->id = source->id;
            nData->label = source->label;
            nData->valueLabes = source->valueLabes;
            nData->bel = source->bel;
            nData->lambda = source->lambda;
            nData->pi = source->pi;
            nData->parents = source->parents;
            nData->children = source->children;
            nData->_priorTable = source->_priorTable;
            nData->pi_zi_x = source->pi_zi_x;
            nData->lambda_x_wi = source->lambda_x_wi;

        }
        std::cout << "nodo : " << this->nData->label << "\n";
        std::cout << "\tBEL : ";
        this->nData->bel.printTest();
        /*
        std::cout << "\tPI : ";
        this->pi.printTest();
        std::cout << "\tLAMBDA : ";
        this->lambda.printTest();
         */
        std::cout << std::endl;
    }


    //terminated
    void updateLambdaX(Node& parent)
    {
        if (nData.operator bool() == false || nData.unique() == false){
            std::shared_ptr<NodeData> source = nData;
            nData = std::make_shared<NodeData>();
            nData->id = source->id;
            nData->label = source->label;
            nData->valueLabes = source->valueLabes;
            nData->bel = source->bel;
            nData->lambda = source->lambda;
            nData->pi = source->pi;
            nData->parents = source->parents;
            nData->children = source->children;
            nData->_priorTable = source->_priorTable;
            nData->pi_zi_x = source->pi_zi_x;
            nData->lambda_x_wi = source->lambda_x_wi;

        }
        if (!isParent(parent))
        {
            return;
        }

        std::shared_ptr<RealVector> lambda_wi;
        try
        {

            if (nData->lambda_x_wi.contains(parent.nData->id)){
                lambda_wi = getLambda_x_wi(parent);
            }
            else {
                lambda_wi = std::shared_ptr<RealVector>(new RealVector(parent.getBel()->getDimension()));
                lambda_wi->setLabels(parent.nData->bel.getLabels());
            }

        }
        catch (std::exception e)
        {
            lambda_wi = std::shared_ptr<RealVector>(new RealVector(parent.getBel()->getDimension()));
            lambda_wi->setLabels(parent.nData->bel.getLabels());
            std::cout << "errore in updateLambdaX: errore settaggio lambda_wi" << "\n";
            return;
        }

        if (nData->lambda.isAllOnes())
        {
            lambda_wi->toAllOnes();
            nData->lambda_x_wi.insert_or_assign(parent.nData->id, lambda_wi);
            return;
        }

        lambda_wi->toAllZeros();

        for (int i = 0; i < nData->_priorTable.getRowDimension(); i++)
        {
            double tmp = 0;
            for (int k = 0; k < nData->_priorTable.getColDimension(); k++)
            {
                tmp += this->nData->_priorTable.getValue(i, k) * nData->lambda.getValue(k); //prima modifica
            }
            for (int j = 0; j < nData->parents.size(); j++) //sommatoria Wk con k diverso da i
            {
                std::shared_ptr<Node> parent2 = nData->parents.at(j);
                if (parent.getId() != parent2->getId())
                {
                    try
                    {
                        std::shared_ptr<RealVector> pi_z = parent2->getPi_zi_x(*this);
                        for (int m = 0; m < pi_z->getDimension(); m++)
                        {
                            if (nData->_priorTable.partOf(pi_z->getLabel(m), nData->_priorTable.getRowLabels(i)))
                                tmp *= pi_z->getValue(m);
                        }
                    }
                    catch (std::exception e)
                    {
                        tmp*=1;
                        std::cout << "errore in updateLambdaX: errato aggiornamento tmp" << std::endl;
                        return;
                    }
                }
            }

            for (int n = 0; n < lambda_wi->getDimension(); n++)
            {
                if (nData->_priorTable.partOf(lambda_wi->getLabel(n), nData->_priorTable.getRowLabels(i)))
                {
                    lambda_wi->setValue(n, lambda_wi->getValue(n) + tmp);
                    break;
                }
            }
        }

        try
        {
            nData->lambda_x_wi.insert_or_assign(parent.nData->id, lambda_wi);
        }
        catch (std::exception e)
        {
            nData->lambda_x_wi.insert_or_assign(parent.nData->id,std::make_shared<RealVector>(this->nData->lambda));
            std::cout << "errore in updateLambdaX: errore inserimento nella mappa lambda_x_wi"<< std::endl;
            return;
        }

    }
};

#endif //PDS_NODE_HPP
