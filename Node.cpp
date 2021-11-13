#include "Node.h"

Node::Node(const Node &source) {
  nData = source.nData;
}

Node::Node(std::string label, int id, int states) {
  nData = std::make_shared<NodeData>();
  nData->id = id;
  nData->bel(states);
  nData->pi(states);
  nData->lambda(states);
  nData->lambda.toAllOnes();
  nData->label = label;
  std::vector<std::string> labels;
  labels.push_back(label + "=y");
  labels.push_back(label + "=n");
  for (int i = 0; i < states; i++) {
    nData->valueLabels.push_back(labels.at(i));
  }
  nData->priorTable();
  nData->bel.setLabels(label, labels);
  nData->pi.setLabels(label, labels);
  nData->lambda.setLabels(label, labels);
}

Node::~Node() {
  if (nData.operator bool() == true) nData.reset();
}

Node &Node::operator=(const Node &source) {
  if (this != &source) {
    /*
    id = source.id;
    label = source.label;
    valueLabels = source.valueLabels;
    bel = source.bel;
    lambda = source.lambda;
    pi = source.pi;
    parents = source.parents;
    children = source.children;
    priorTable = source.priorTable;
    pi_zi_x = source.pi_zi_x;
    lambda_x_wi = source.lambda_x_wi;
     */
    nData = source.nData;
  }
  return *this;
}

std::shared_ptr<RealVector<double>> Node::getBel() {
  return std::make_shared<RealVector<double>>(nData->bel);
}

RealVector<double> &Node::getPi() {
  return nData->pi;
}

RealVector<double> &Node::getLambda() {
  return (nData->lambda);
}

std::string Node::getLabel() {
  return nData->label;
}
std::vector<std::shared_ptr<Node>> Node::getChildren() {
  return nData->children;
}

std::vector<std::shared_ptr<Node>> Node::getParents() {
  return nData->parents;
}

std::shared_ptr<Matrix<double>> Node::getMx_wAll() {
  return std::make_shared<Matrix<double>>(nData->priorTable);
}

void Node::setMx_wAll(Matrix<double> m) {
  nData->priorTable = m;
}

bool Node::isParent(const Node &node) {
  for (std::shared_ptr<Node> n: nData->parents) {
    if (node.nData->id == n->nData->id)
      return true;
  }

  return false;
}

bool Node::isChild(const Node &node) {
  for (std::shared_ptr<Node> n: nData->children) {
    if (node.nData->id == n->nData->id)
      return true;
  }

  return false;
}

bool Node::operator<(const Node &rhs) const {
  return nData->id < rhs.nData->id;
}
bool Node::operator==(const Node &rhs) const {
  return nData->id == rhs.nData->id;
}
bool Node::operator!=(const Node &rhs) const {
  return nData->id != rhs.nData->id;
}

int Node::getId() const {
  return nData->id;
}

std::shared_ptr<RealVector<double>> Node::getPi_zi_x(Node child) {
  return nData->pi_zi_x.at(child.nData->id);
}

std::shared_ptr<RealVector<double>> Node::getLambda_x_wi(Node parent) {
  return nData->lambda_x_wi.at(parent.nData->id);

}

void Node::addParent(std::shared_ptr<Node> node) {
  if (!isParent(*node)) {

    if (nData.operator bool() == false || nData.unique() == false) {
      std::shared_ptr<NodeData> source = nData;
      nData = std::make_shared<NodeData>();
      nData->id = source->id;
      nData->label = source->label;
      nData->valueLabels = source->valueLabels;
      nData->bel = source->bel;
      nData->lambda = source->lambda;
      nData->pi = source->pi;
      nData->parents = source->parents;
      nData->children = source->children;
      nData->priorTable = source->priorTable;
      nData->pi_zi_x = source->pi_zi_x;
      nData->lambda_x_wi = source->lambda_x_wi;

    }

    nData->parents.push_back(node);
  }
}

void Node::addChild(std::shared_ptr<Node> node) {
  if (!isChild(*node)) {
    if (nData.operator bool() == false || nData.unique() == false) {
      std::shared_ptr<NodeData> source = nData;
      nData = std::make_shared<NodeData>();
      nData->id = source->id;
      nData->label = source->label;
      nData->valueLabels = source->valueLabels;
      nData->bel = source->bel;
      nData->lambda = source->lambda;
      nData->pi = source->pi;
      nData->parents = source->parents;
      nData->children = source->children;
      nData->priorTable = source->priorTable;
      nData->pi_zi_x = source->pi_zi_x;
      nData->lambda_x_wi = source->lambda_x_wi;

    }
    nData->children.push_back(node);
  }
}

void Node::updateBEL() { //prodotto con normalizzazione
  if (nData.operator bool() == false || nData.unique() == false) {
    std::shared_ptr<NodeData> source = nData;
    nData = std::make_shared<NodeData>();
    nData->id = source->id;
    nData->label = source->label;
    nData->valueLabels = source->valueLabels;
    nData->bel = source->bel;
    nData->lambda = source->lambda;
    nData->pi = source->pi;
    nData->parents = source->parents;
    nData->children = source->children;
    nData->priorTable = source->priorTable;
    nData->pi_zi_x = source->pi_zi_x;
    nData->lambda_x_wi = source->lambda_x_wi;

  }
  try {
    nData->bel.termProduct(nData->pi, nData->lambda);
    nData->bel.normalise();
  } catch (std::exception &e) {
    nData->bel.setValue(0, 0);
    nData->bel.setValue(1, 0);
    //std::cout<< "errore nell'aggiornamento BEL " << std::endl;
  }
}

void Node::updatePi() {
  if (nData.operator bool() == false || nData.unique() == false) {
    std::shared_ptr<NodeData> source = nData;
    nData = std::make_shared<NodeData>();
    nData->id = source->id;
    nData->label = source->label;
    nData->valueLabels = source->valueLabels;
    nData->bel = source->bel;
    nData->lambda = source->lambda;
    nData->pi = source->pi;
    nData->parents = source->parents;
    nData->children = source->children;
    nData->priorTable = source->priorTable;
    nData->pi_zi_x = source->pi_zi_x;
    nData->lambda_x_wi = source->lambda_x_wi;

  }
  if (nData->parents.size() == 0) {
    return;
  }
  nData->pi.toAllZeros();
  //std::shared_ptr<Node> parent ;
  std::shared_ptr<RealVector<double>> pi_z;
  try {
    for (int j = 0; j < nData->priorTable.getRowDimension(); j++) {
      std::shared_ptr<double[]> tmp = std::shared_ptr<double[]>(new double[nData->priorTable.getColDimension()]);
      for (int m = 0; m < nData->priorTable.getColDimension(); m++) {
        tmp[m] = nData->priorTable.getValue(j, m);
      }

      //fin qui mi sono calcolato P(x|w)
      for (int i = 0; i < nData->parents.size(); i++) { //produttoria sui padri
        //parent = nData->parents.at(i);
        pi_z = nData->parents.at(i)->getPi_zi_x(*this);
        for (int k = 0; k < pi_z->getDimension(); k++) {

          if (nData->priorTable.partOf(pi_z->getLabel(k), nData->priorTable.getRowLabels(j))) {
            for (int m = 0; m < nData->priorTable.getColDimension(); m++) {
              tmp[m] *= pi_z->getValue(k);
            }

          }

        }
      }
      for (int m = 0; m < nData->priorTable.getColDimension(); m++) {
        nData->pi.setValue(m, nData->pi.getValue(m) + tmp[m]);
      }
      //aggiornameto del pi
      tmp.reset();
    }
    nData->pi.normalise();

  } catch (std::exception &e) {
    nData->pi.setValue(0, 0);
    nData->pi.setValue(1, 0);
    //std::cout << "errore nell aggiornamento del PI" << std::endl;
  }
}

void Node::updateLambda() { //produttoria dei lamda_z_j (formula 1 del sito)
  if (nData.operator bool() == false || nData.unique() == false) {
    std::shared_ptr<NodeData> source = nData;
    nData = std::make_shared<NodeData>();
    nData->id = source->id;
    nData->label = source->label;
    nData->valueLabels = source->valueLabels;
    nData->bel = source->bel;
    nData->lambda = source->lambda;
    nData->pi = source->pi;
    nData->parents = source->parents;
    nData->children = source->children;
    nData->priorTable = source->priorTable;
    nData->pi_zi_x = source->pi_zi_x;
    nData->lambda_x_wi = source->lambda_x_wi;

  }
  if (nData->children.size() == 0) {
    return;
  }
  nData->lambda.setValues(nData->children.at(0)->getLambda_x_wi(*this)->getValues());
  try {
    for (int i = 1; i < nData->children.size(); i++) {
      std::shared_ptr<Node> child = nData->children.at(i);
      nData->lambda.termProduct(nData->lambda, *(child->getLambda_x_wi(*this)));
    }
  } catch (std::exception &e) {
    nData->lambda.setValue(0, 0);
    nData->lambda.setValue(1, 0);
    //std::cout << "errore nell  update lambda" << std::endl;
  }
}

//terminated
void Node::updatePiZ(Node &child) {
  if (nData.operator bool() == false || nData.unique() == false) {
    std::shared_ptr<NodeData> source = nData;
    nData = std::make_shared<NodeData>();
    nData->id = source->id;
    nData->label = source->label;
    nData->valueLabels = source->valueLabels;
    nData->bel = source->bel;
    nData->lambda = source->lambda;
    nData->pi = source->pi;
    nData->parents = source->parents;
    nData->children = source->children;
    nData->priorTable = source->priorTable;
    nData->pi_zi_x = source->pi_zi_x;
    nData->lambda_x_wi = source->lambda_x_wi;

  }

  if (!isChild(child)) {
    return;
  }

  std::shared_ptr<RealVector<double>> pi_z;

  try {
    if (!(nData->pi_zi_x.contains(child.nData->id))) {
      pi_z = std::make_shared<RealVector<double>>(nData->pi); //creare operatore di assegnazione
    } else {
      pi_z = nData->pi_zi_x.at(child.nData->id);
    }
  } catch (std::exception &e) {
    pi_z = std::make_shared<RealVector<double>>(nData->pi);
    //std::cout << "errore in updatePiz : non viene ritornato nessun elemento con quel child.id  " << std::endl;
  }

  if (nData->children.size() == 1) {
    pi_z = std::make_shared<RealVector<double>>(nData->pi);
    pi_z->normalise();
    nData->pi_zi_x.insert_or_assign(child.nData->id, pi_z);
    return;
  }

  try {
    pi_z->divide(nData->bel, *(child.getLambda_x_wi(*this)));
  } catch (std::exception &e) {
    pi_z = std::make_shared<RealVector<double>>(nData->pi);
    pi_z->normalise();
    //std::cout << "errore in updatePiZ , divisione non valida" << std::endl ;
  }
  pi_z->normalise();
  nData->pi_zi_x.insert_or_assign(child.nData->id, pi_z);
}

bool Node::operator==(const Node &sx) {
  return this->nData->id == sx.nData->id;
}

void Node::printValues(std::ostream &outputTarget) {
  if (nData.operator bool() == false || nData.unique() == false) {
    std::shared_ptr<NodeData> source = nData;
    nData = std::make_shared<NodeData>();
    nData->id = source->id;
    nData->label = source->label;
    nData->valueLabels = source->valueLabels;
    nData->bel = source->bel;
    nData->lambda = source->lambda;
    nData->pi = source->pi;
    nData->parents = source->parents;
    nData->children = source->children;
    nData->priorTable = source->priorTable;
    nData->pi_zi_x = source->pi_zi_x;
    nData->lambda_x_wi = source->lambda_x_wi;

  }

  //outputTarget << this->nData->label << std::string((20-this->nData->label.size())," ");
    outputTarget << this->nData->label <<  std::setw(30-this->nData->label.size());

  this->nData->bel.printTest(outputTarget);


  /*
  std::cout << "\tPI : ";
  this->pi.printTest();
  std::cout << "\tLAMBDA : ";
  this->lambda.printTest();
   */
}

void Node::updateLambdaX(Node &parent) {
  if (nData.operator bool() == false || nData.unique() == false) {
    std::shared_ptr<NodeData> source = nData;
    nData = std::make_shared<NodeData>();
    nData->id = source->id;
    nData->label = source->label;
    nData->valueLabels = source->valueLabels;
    nData->bel = source->bel;
    nData->lambda = source->lambda;
    nData->pi = source->pi;
    nData->parents = source->parents;
    nData->children = source->children;
    nData->priorTable = source->priorTable;
    nData->pi_zi_x = source->pi_zi_x;
    nData->lambda_x_wi = source->lambda_x_wi;

  }
  if (!isParent(parent)) {
    return;
  }

  std::shared_ptr<RealVector<double>> lambda_wi;
  try {

    if (nData->lambda_x_wi.contains(parent.nData->id)) {
      lambda_wi = getLambda_x_wi(parent);
    } else {
      lambda_wi = std::shared_ptr<RealVector<double>>(new RealVector<double>(parent.getBel()->getDimension()));
      lambda_wi->setLabels(parent.nData->bel.getLabels());
    }

  } catch (std::exception &e) {
    lambda_wi = std::shared_ptr<RealVector<double>>(new RealVector<double>(parent.getBel()->getDimension()));
    lambda_wi->setLabels(parent.nData->bel.getLabels());
    //std::cout << "errore in updateLambdaX: errore settaggio lambda_wi" << "\n";
    return;
  }

  if (nData->lambda.isAllOnes()) {
    lambda_wi->toAllOnes();
    nData->lambda_x_wi.insert_or_assign(parent.nData->id, lambda_wi);
    return;
  }

  lambda_wi->toAllZeros();

  for (int i = 0; i < nData->priorTable.getRowDimension(); i++) {
    double tmp = 0;
    for (int k = 0; k < nData->priorTable.getColDimension(); k++) {
      tmp += this->nData->priorTable.getValue(i, k) * nData->lambda.getValue(k); //prima modifica
    }
    for (int j = 0; j < nData->parents.size(); j++) //sommatoria Wk con k diverso da i
    {
      std::shared_ptr<Node> parent2 = nData->parents.at(j);
      if (parent.getId() != parent2->getId()) {
        try {
          std::shared_ptr<RealVector<double>> pi_z = parent2->getPi_zi_x(*this);
          for (int m = 0; m < pi_z->getDimension(); m++) {
            if (nData->priorTable.partOf(pi_z->getLabel(m), nData->priorTable.getRowLabels(i)))
              tmp *= pi_z->getValue(m);
          }
        } catch (std::exception &e) {
          tmp *= 1;
          //std::cout << "errore in updateLambdaX: errato aggiornamento tmp" << std::endl;
          return;
        }
      }
    }

    for (int n = 0; n < lambda_wi->getDimension(); n++) {
      if (nData->priorTable.partOf(lambda_wi->getLabel(n), nData->priorTable.getRowLabels(i))) {
        lambda_wi->setValue(n, lambda_wi->getValue(n) + tmp);
        break;
      }
    }
  }

  try {
    nData->lambda_x_wi.insert_or_assign(parent.nData->id, lambda_wi);
  } catch (std::exception &e) {
    nData->lambda_x_wi.insert_or_assign(parent.nData->id, std::make_shared<RealVector<double>>(this->nData->lambda));
    //std::cout << "errore in updateLambdaX: errore inserimento nella mappa lambda_x_wi"<< std::endl;
    return;
  }
}