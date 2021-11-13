#ifndef PROJECTPDS__BAYESIANNETWORK_H_
#define PROJECTPDS__BAYESIANNETWORK_H_

#include <iostream>                  // for std::cout
#include <algorithm>                 // for std::for_each
#include <utility>
#include <vector>
#include <thread>
#include <memory_resource>
#include <boost/lexical_cast.hpp>
#include "Node.h"
#include "ThreadPool.h"

class BayesianNetwork {
 private :
  double epsilon;
  int numThreads;
  pugi::xml_document inputXdsl;
  pugi::xml_node xdsl_nodes;
  std::deque<std::thread> listaThread;
  std::vector<std::shared_ptr<Node>> vertex_array;

 public:
  BayesianNetwork();

  const std::vector<std::shared_ptr<Node>> &getVertexArray() const;

  bool input(const char *path);
  void output(std::ostream& outputTarget);
  void compute();
  void inference(std::vector<std::string> inferences, std::vector<std::string> evidences);
  //usata da funzione per settare etichette righe
  void recursiveLabelSet(std::vector<std::string>::size_type nPadri,
                         int k,
                         std::vector<std::string> list,
                         std::vector<std::string> parentLabels,
                         const std::shared_ptr<Matrix<double>> &m,
                         int *j);
  //per settare etichette righe
  void matrixCombination(std::vector<std::string>::size_type nPadri,
                         std::vector<std::string> parentLabels,
                         const std::shared_ptr<Matrix<double>> &m);

};

#endif //PROJECTPDS__BAYESIANNETWORK_H_
