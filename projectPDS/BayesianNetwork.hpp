#include <iostream>                  // for std::cout
#include <algorithm>                 // for std::for_each
#include "Node.hpp"
#include <vector>
#include <ctime>
#include <thread>
#include "ThreadPool.hpp"


template <typename T> class BayesianNetwork{
public:

    void start(char *path){
        double epsilon = 10e-11;
        std::deque<std::thread> listaThread ;
        std::mutex mt0,mt1,mt2,mt3,mt4,mt5,mt6,mt7;
        std::vector<std::shared_ptr<Node<T>>> vertex_array,original_vertex_array ;

        //INIZIO PARTE INPUT AUTOMATICO
        int num_vertices = 0;
        const char *inputXdslPath = path;
        pugi::xml_document inputXdsl;
        pugi::xml_parse_result parse_result = inputXdsl.load_file(inputXdslPath); // parsing del file xdsl

        if (!parse_result) {
            std::cout << "XML [" << inputXdslPath << "] parsed with errors\n";
            std::cout << "Error description: " << parse_result.description() << "\n";
            std::cout << "Error offset: " << parse_result.offset << " (error at [..." << (inputXdslPath + parse_result.offset)
                      << "])\n\n";
            return ;
        }

        pugi::xml_node xdsl_nodes = inputXdsl.child("smile").child("nodes"); //prende i nodi tramite il tag 'nodes'

        for (pugi::xml_node cpt: xdsl_nodes.children("cpt")) { //iterazione su tutti i nodi
            pugi::xml_attribute cptLabel = cpt.first_attribute(); //label del nodo
            std::vector<std::string> state_labels;
            std::vector<std::string> probabilities_strings;
            std::vector<double> probabilities_double;
            boost::split(probabilities_strings, cpt.child("probabilities").text().get(), [](char c) { //prende l'array delle probabilità da mettere nella matrice
                return c == ' ';
            });
            for (const std::string &s: probabilities_strings) {
                probabilities_double.push_back(boost::lexical_cast<double>(s));
            }
            std::vector<std::string> labels;
            std::shared_ptr<Node<T>>
                    node_to_insert(new Node(cptLabel.value(), num_vertices++,2));
            vertex_array.push_back(node_to_insert);
            std::string parents_string = cpt.child("parents").text().get(); //prendimao i nomi dei genitori
            std::vector<std::string> parents_labels;

            if (!parents_string.empty()) // necessary to avoid empty string in parents_labels. stateLabel and probabilities can't
                // be empty so it is not necessary for them
                boost::split(parents_labels, parents_string, [](char c) { return c == ' '; }); //vettore di nomi dei genitori

            std::vector<std::shared_ptr<Node<T>>> parents;
            int num_rows = 1;
            std::shared_ptr<Node<T>> parent ;
            for (const std::string &s: parents_labels) {
                for (std::shared_ptr<Node<T>> n : vertex_array) {
                    if (n->getLabel() == s) parent = n;
                }
                parent->addChild(node_to_insert);
                node_to_insert->addParent(parent);

                num_rows *= 2;
            }
            std::shared_ptr<Matrix> M ( new Matrix(num_rows, 2, probabilities_double));
            matriceCominazioni(parents_labels.size(),parents_labels,M);
            std::vector<std::string> colLabel;
            colLabel.push_back(node_to_insert->getLabel()+"=y");
            M->setColLabels(0,colLabel);
            colLabel.clear();
            colLabel.push_back(node_to_insert->getLabel()+"=n");
            M->setColLabels(1,colLabel);
            colLabel.clear();
            colLabel.push_back(node_to_insert->getLabel()+"=y");
            node_to_insert->setMx_wAll(*M);

        }

        //FINE PARTE INPUT AUTOMATICO


        //ordinamento nei nodi in modo che i nodi foglia vengano prima
        original_vertex_array = vertex_array;
        std::sort(vertex_array.begin(),vertex_array.end(),[](std::shared_ptr<Node<T>> l,std::shared_ptr<Node<T>> r){ return  l->getChildren().size() < r->getChildren().size();});

        std::vector<Node<T>> nodesCopy;

        //inizializzazioni
        for (int i = 0 ; i < vertex_array.size() ; i++){
            //RealVector* v;
            if (vertex_array.at(i)->getChildren().size() == 0){
                vertex_array.at(i)->getLambda()->setValue(0,1);
                vertex_array.at(i)->getLambda()->setValue(1,1);

            }
            if (vertex_array.at(i)->getParents().size() == 0){
                vertex_array.at(i)->getPi()->setValue(0,vertex_array.at(i)->getMx_wAll()->getValue(0,0));
                vertex_array.at(i)->getPi()->setValue(1,vertex_array.at(i)->getMx_wAll()->getValue(0,1));

            }
        }


        for (std::shared_ptr<Node<T>> node : vertex_array) nodesCopy.push_back(*node);

        int it = 0;
        bool found = false;
        double maxDiff = -1,diff;


        //ciclo dei calcoli
        while(true){ // nella costruzione del grafo , i nodi fogli hanno precedenza
            ThreadPool lambdaPool,piPool,belPool,lambdaXPool,piZPool;


            //inizio calcoli (suddivisi per ogni operazione)

            for (int i = 0; i < 3 ; i++){
                listaThread.push_back(std::thread ([&](){ lambdaXPool.runThread();}));
            }
            for ( std::shared_ptr<Node<T>> node : vertex_array) {
                for (std::shared_ptr<Node<T>> parent : node->getParents()){

                    lambdaXPool.submit(std::bind(&Node<T>::updateLambdaX,node,*parent.get()));

                }}
            lambdaXPool.quit();
            for (std::thread &t : listaThread){
                if (t.joinable()) t.join();
            }
            listaThread.clear();



            //versione parallelizzata updatePiZ

            for (int i = 0; i < 3 ; i++){
                listaThread.push_back(std::thread ([&](){ piZPool.runThread();}));
            }
            for ( std::shared_ptr<Node<T>> node : vertex_array) {
                for (std::shared_ptr<Node<T>> child : node->getChildren()){

                    piZPool.submit(std::bind(&Node<T>::updatePiZ,node,*child.get()));

                }}
            piZPool.quit();
            for (std::thread &t : listaThread){
                if (t.joinable()) t.join();
            }
            listaThread.clear();





            //Verisone parallelizzata updatePi

            for (int i = 0; i < 3 ; i++){
                listaThread.push_back(std::thread ([&](){ piPool.runThread();}));
            }
            for ( std::shared_ptr<Node<T>> node : vertex_array) {
                piPool.submit(std::bind(&Node<T>::updatePi,node));
                //node->updateBEL();
            }
            piPool.quit();

            for (std::thread &t : listaThread){
                if (t.joinable()) {
                    t.join();
                }
            }
            listaThread.clear();


            //Verisone parallelizzata updateLAMBDA

            for (int i = 0; i < 3 ; i++){
                listaThread.push_back(std::thread ([&](){ lambdaPool.runThread();}));
            }
            for ( std::shared_ptr<Node<T>> node : vertex_array) {
                lambdaPool.submit(std::bind(&Node<T>::updateLambda,node));
                //node->updateBEL();
            }
            lambdaPool.quit();

            for (std::thread &t : listaThread){
                if (t.joinable()) t.join();
            }
            listaThread.clear();

            //Versione parallelizzata updateBEL

            for (int i = 0; i < 3 ; i++){
                listaThread.push_back(std::thread ([&](){ belPool.runThread();}));
            }

            for ( std::shared_ptr<Node<T>> node : vertex_array) {
                belPool.submit(std::bind(&Node<T>::updateBEL,node));
                //node->updateBEL();
            }
            belPool.quit();
            for (std::thread &t : listaThread){
                if (t.joinable()) t.join();
            }
            listaThread.clear();





            //Calcolo della differenza

            for ( int i = 0 ; i < vertex_array.size() ; i++) {

                if (vertex_array.at(i)->getBel()->getValue(0) == 0 &&
                    vertex_array.at(i)->getBel()->getValue(1) == 0) { maxDiff = -1;found = true; }
            }


            for ( int i = 0 ; i < vertex_array.size() && found == false; i++) {

                for (std::shared_ptr<Node<T>> child : vertex_array.at(i)->getChildren()) {
                    try {
                        diff = std::abs(vertex_array.at(i)->getPi_zi_x(*child).get()->getValue(0) -
                                        nodesCopy.at(i).getPi_zi_x(*child).get()->getValue(0));
                    }
                    catch (std::exception e) {diff=-1;}
                    if (diff > maxDiff) maxDiff = diff;

                    try {
                        diff = std::abs(vertex_array.at(i)->getPi_zi_x(*child).get()->getValue(1) -
                                        nodesCopy.at(i).getPi_zi_x(*child).get()->getValue(1));
                    }
                    catch (std::exception e) {diff=-1;}

                    if (diff > maxDiff) maxDiff = diff;
                }

                for (std::shared_ptr<Node<T>> parent : vertex_array.at(i)->getParents()) {
                    try {
                        diff = std::abs(vertex_array.at(i)->getLambda_x_wi(*parent).get()->getValue(0) -
                                        nodesCopy.at(i).getLambda_x_wi(*parent).get()->getValue(0));
                    } catch (std::exception e) {diff=-1;}

                    if (diff > maxDiff) maxDiff = diff;
                    try {
                        diff = std::abs(vertex_array.at(i)->getLambda_x_wi(*parent).get()->getValue(1) -
                                        nodesCopy.at(i).getLambda_x_wi(*parent).get()->getValue(1));
                    } catch (std::exception e) {diff=-1;}

                    if (diff > maxDiff) maxDiff = diff;
                }
            }

            it++;
            if (std::abs(maxDiff) < epsilon ){
                break;
            }
            maxDiff = -1;
            found = false;
            nodesCopy.clear();

            for (std::shared_ptr<Node<T>> node : vertex_array) nodesCopy.push_back(*node);
            //fine calcolo differenza
        }
        std::for_each_n(vertex_array.begin(), vertex_array.size(),[](std::shared_ptr<Node<T>> n) {n->printValues();});

        //XDSL output
        int i = 0;
        for (pugi::xml_node cpt: xdsl_nodes.children("cpt")) {
            std::string beliefs_string = original_vertex_array[i++]->getBel()->GetValuesString(); // extract beliefs from the

            cpt.child("probabilities").text().set(beliefs_string.c_str());
        }
        inputXdsl.save_file("./../outputAsia.xdsl");

        //Fine output XDSL

        return ;

    }


    void inference(std::vector<std::shared_ptr<Node<T>>> vertex_array){

    }

//usata da funzione per settare etichette riche
    void ricorsione( int nPadri,int k,std::vector<std::string> list,std::vector<std::string> parentLabels,std::shared_ptr<Matrix> m,int *j){
        if (k == nPadri) {
            m->setRowLabels(*j,list);
            (*j)++;
            //std::for_each_n(list.begin(),list.size(),[](std::string s ) { std::cout << s<< " " ;});
            //std::cout << std::endl;
            return;
        }
        for (int i=0 ; i < 2 ; i++){
            if (i == 0) {
                std::string s = parentLabels.at(k);
                s+= "=y";
                list.push_back(s);
            }
            else
            {
                std::string s = parentLabels.at(k);
                s+= "=n";
                list.push_back(s);
            }
            ricorsione(nPadri,k+1,list,parentLabels,m,j);
            list.pop_back();
        }
        return ;
    }

//per settare etichette riche
    void matriceCominazioni(int nPadri,std::vector<std::string> parentLabels,std::shared_ptr<Matrix> m){
        int nRow = 1 << nPadri;
        std::vector<std::string> list;
        int j = 0;
        ricorsione(nPadri,0,list,parentLabels,m,&j);

    }


};