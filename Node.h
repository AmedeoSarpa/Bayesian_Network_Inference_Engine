#ifndef PROJECTPDS__NODE_H_
#define PROJECTPDS__NODE_H_

#include <iostream>                  // for std::cout
#include <string>
#include <vector>
#include <map>
#include <algorithm>                 // for std::for_each
#include <condition_variable>
#include <boost/algorithm/string.hpp>
#include "pugixml_library/pugixml.hpp"
#include "RealVector.hpp"
#include "Matrix.hpp"
/*
#include <utility>                   // for std::pair
#include <memory>
#include <mutex>
#include <boost/graph/graph_traits.hpp>
#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/graphviz.hpp>
#include <boost/graph/depth_first_search.hpp>
#include <boost/pending/indirect_cmp.hpp>
*/

class Node {
  struct NodeData {
    std::string label;
    int id;
    std::vector<std::string> valueLabels;
    RealVector<double> bel, pi, lambda;
    std::vector<std::shared_ptr<Node>> parents, children;
    Matrix<double> priorTable;
    std::map<int, std::shared_ptr<RealVector<double>>> pi_zi_x;
    std::map<int, std::shared_ptr<RealVector<double>>> lambda_x_wi;
  };
 private:
  std::shared_ptr<NodeData> nData;

 public:
  Node(const Node &source);
  Node(std::string label, int id, int states);
  ~Node();

  Node &operator=(const Node &source);
  bool operator<(const Node &rhs) const;
  bool operator==(const Node &rhs) const;
  bool operator!=(const Node &rhs) const;
  bool operator==(const Node &sx);

  void printValues(std::ostream &outputTarget);

  int getId() const;
  std::string getLabel();
  std::shared_ptr<RealVector<double>> getBel();
  //Vedere questa cosa
  RealVector<double> &getPi();
  //Vedere questa cosa
  RealVector<double> &getLambda();
  std::vector<std::shared_ptr<Node>> getChildren();
  std::vector<std::shared_ptr<Node>> getParents();
  std::shared_ptr<RealVector<double>> getPi_zi_x(Node child);
  std::shared_ptr<RealVector<double>> getLambda_x_wi(Node parent);
  std::shared_ptr<Matrix<double>> getMx_wAll();
  void setMx_wAll(Matrix<double> m);

  void addParent(std::shared_ptr<Node> node);
  void addChild(std::shared_ptr<Node> node);
  bool isParent(const Node &node);
  bool isChild(const Node &node);

  //terminated
  void updateBEL();
  //terminated
  void updatePi();
  //terminated
  void updateLambda();
  //terminated
  void updatePiZ(Node &child);
  //terminated
  void updateLambdaX(Node &parent);

};

#endif //PROJECTPDS__NODE_H_
