#include "BayesianNetwork.h"

BayesianNetwork::BayesianNetwork() {
  epsilon = 10e-12;
  numThreads = 3;
}

bool BayesianNetwork::input(const char *path) {
  //INIZIO PARTE INPUT AUTOMATICO
  int num_vertices = 0;
  const char *inputXdslPath = path;
  pugi::xml_parse_result parse_result = inputXdsl.load_file(inputXdslPath); // parsing del file xdsl

  if (!parse_result) {
    return false;
  }

  xdsl_nodes = inputXdsl.child("smile").child("nodes"); //prende i nodi tramite il tag 'nodes'

  for (pugi::xml_node cpt: xdsl_nodes.children("cpt")) { //iterazione su tutti i nodi
    pugi::xml_attribute cptLabel = cpt.first_attribute(); //label del nodo
    std::vector<std::string> state_labels;
    std::vector<std::string> probabilities_strings;
    std::vector<double> probabilities_double;
    boost::split(probabilities_strings,
                 cpt.child("probabilities").text().get(),
                 [](char c) { //prende l'array delle probabilità da mettere nella matrice
                   return c == ' ';
                 });
    probabilities_double.reserve(probabilities_strings.size());
    for (const std::string &s: probabilities_strings) {
      probabilities_double.push_back(boost::lexical_cast<double>(s));
    }
    std::vector<std::string> labels;
    std::shared_ptr<Node> node_to_insert = std::make_shared<Node>(Node(cptLabel.value(), num_vertices++, 2));
    vertex_array.push_back(node_to_insert);
    std::string parents_string = cpt.child("parents").text().get(); //prendimao i nomi dei genitori
    std::vector<std::string> parents_labels;

    if (!parents_string.empty()) // necessary to avoid empty string in parents_labels. stateLabel and probabilities can't
      // be empty so it is not necessary for them
      boost::split(parents_labels, parents_string, [](char c) { return c == ' '; }); //vettore di nomi dei genitori

    std::vector<std::shared_ptr<Node>> parents;
    int num_rows = 1;
    std::shared_ptr<Node> parent;
    for (const std::string &s: parents_labels) {
      for (const std::shared_ptr<Node> &n: vertex_array) {
        if (n->getLabel() == s)
          parent = n;
      }
      parent->addChild(node_to_insert);
      node_to_insert->addParent(parent);

      num_rows *= 2;
    }
    std::shared_ptr<Matrix<double>> M(new Matrix<double>(num_rows, 2, probabilities_double));
    matrixCombination(parents_labels.size(), parents_labels, M);
    std::vector<std::string> colLabel;
    colLabel.push_back(node_to_insert->getLabel() + "=y");
    M->setColLabels(0, colLabel);
    colLabel.clear();
    colLabel.push_back(node_to_insert->getLabel() + "=n");
    M->setColLabels(1, colLabel);
    colLabel.clear();
    colLabel.push_back(node_to_insert->getLabel() + "=y");
    node_to_insert->setMx_wAll(*M);
  }
  //FINE PARTE INPUT AUTOMATICO

  //ordinamento nei nodi in modo che i nodi foglia vengano prima
  std::sort(vertex_array.begin(),
            vertex_array.end(),
            [](const std::shared_ptr<Node> &l, const std::shared_ptr<Node> &r) {
              return l->getChildren().size() < r->getChildren().size();
            });
  //inizializzazioni
  for (const std::shared_ptr<Node> &n: vertex_array) {
    //RealVector* v;
    if (n->getChildren().empty()) {
      n->getLambda().setValue(0, 1);
      n->getLambda().setValue(1, 1);

    }
    if (n->getParents().empty()) {
      n->getPi().setValue(0, n->getMx_wAll()->getValue(0, 0));
      n->getPi().setValue(1, n->getMx_wAll()->getValue(0, 1));
    }
  }
  return true;
}

void BayesianNetwork::output(std::ostream& outputTarget) {
  outputTarget << "node" << std::setw(36) << "BEL(yes)\tBEL(no)\n";
  std::for_each_n(vertex_array.begin(), vertex_array.size(), [&outputTarget](const std::shared_ptr<Node> &n) {
    n->printValues(outputTarget);
  });
  //XDSL output
  /*        int i = 0;
          for (pugi::xml_node cpt: xdsl_nodes.children("cpt")) {
              std::string beliefs_string = vertex_array[i++]->getBel()->GetValuesString(); // extract beliefs from the

              cpt.child("BEL").text().set(beliefs_string.c_str());
          }
          inputXdsl.save_file("./../IOFiles/output.xdsl");*/
  //Fine output XDSL

}
void BayesianNetwork::compute() {

  //allocatore polimorfico , nodesCopy ha dimensione pari alla lunghezza di  vertex_array e la liberaizone deve avvenire tutta in una volta quando nodesCopy non serve piu
  std::pmr::monotonic_buffer_resource pool{vertex_array.size()};
  std::pmr::vector<Node> nodesCopy{&pool};

  for (const std::shared_ptr<Node> &node: vertex_array)
    nodesCopy.push_back(*node);
  bool found = false;
  double maxDiff = -1, diff;
  ThreadPool lambdaPool, piPool, belPool, lambdaXPool, piZPool;
  //ciclo dei calcoli
  while (true) { // nella costruzione del grafo, i nodi fogli hanno precedenza
    lambdaPool.start();
    piZPool.start();
    belPool.start();
    lambdaXPool.start();
    piPool.start();

    //inizio calcoli (suddivisi per ogni operazione)

    for (int i = 0; i < numThreads; i++) {
      listaThread.emplace_back([&]() { lambdaXPool.runThread(); });
    }
    for (std::shared_ptr<Node> node: vertex_array) {
      for (std::shared_ptr<Node> parent: node->getParents()) {

        lambdaXPool.submit(std::bind(&Node::updateLambdaX, node, *parent));

      }
    }
    lambdaXPool.quit();
    for (std::thread &t: listaThread) {
      if (t.joinable())
        t.join();
    }
    listaThread.clear();


    for (int i = 0; i < numThreads; i++) {
      listaThread.emplace_back([&]() { piZPool.runThread(); });
    }
    for (std::shared_ptr<Node> node: vertex_array) {
      for (std::shared_ptr<Node> child: node->getChildren()) {

        piZPool.submit(std::bind(&Node::updatePiZ, node, *child));

      }
    }
    piZPool.quit();
    for (std::thread &t: listaThread) {
      if (t.joinable())
        t.join();
    }
    listaThread.clear();


    for (int i = 0; i < numThreads; i++) {
      listaThread.emplace_back([&]() { piPool.runThread(); });
    }
    for (std::shared_ptr<Node> node: vertex_array) {
      piPool.submit(std::bind(&Node::updatePi, node));
      //node->updateBEL();
    }
    piPool.quit();

    for (std::thread &t: listaThread) {
      if (t.joinable()) {
        t.join();
      }
    }
    listaThread.clear();


    for (int i = 0; i < numThreads; i++) {
      listaThread.emplace_back([&]() { lambdaPool.runThread(); });
    }
    for (std::shared_ptr<Node> node: vertex_array) {
      lambdaPool.submit(std::bind(&Node::updateLambda, node));
      //node->updateBEL();
    }
    lambdaPool.quit();

    for (std::thread &t: listaThread) {
      if (t.joinable())
        t.join();
    }
    listaThread.clear();


    for (int i = 0; i < numThreads; i++) {
      listaThread.emplace_back([&]() { belPool.runThread(); });
    }

    for (std::shared_ptr<Node> node: vertex_array) {
      belPool.submit(std::bind(&Node::updateBEL, node));
      //node->updateBEL();
    }
    belPool.quit();
    for (std::thread &t: listaThread) {
      if (t.joinable())
        t.join();
    }
    listaThread.clear();

    //Calcolo della differenza
    for (const std::shared_ptr<Node> &node: vertex_array) {

      if (node->getBel()->getValue(0) == 0 && node->getBel()->getValue(1) == 0) {
        maxDiff = -1;
        found = true;
      }
    }

    for (int i = 0; i < vertex_array.size() && !found; i++) {

      for (std::shared_ptr<Node> child: vertex_array.at(i)->getChildren()) {
        try {
          diff = std::abs(
              vertex_array.at(i)->getPi_zi_x(*child)->getValue(0) - nodesCopy.at(i).getPi_zi_x(*child)->getValue(0));
        } catch (std::exception &e) {
          diff = -1;
        }
        if (diff > maxDiff)
          maxDiff = diff;

        try {
          diff = std::abs(
              vertex_array.at(i)->getPi_zi_x(*child)->getValue(1) - nodesCopy.at(i).getPi_zi_x(*child)->getValue(1));
        } catch (std::exception &e) {
          diff = -1;
        }

        if (diff > maxDiff)
          maxDiff = diff;
      }

      for (std::shared_ptr<Node> parent: vertex_array.at(i)->getParents()) {
        try {
          diff = std::abs(vertex_array.at(i)->getLambda_x_wi(*parent)->getValue(0)
                              - nodesCopy.at(i).getLambda_x_wi(*parent)->getValue(0));
        } catch (std::exception &e) {
          diff = -1;
        }

        if (diff > maxDiff)
          maxDiff = diff;
        try {
          diff = std::abs(vertex_array.at(i)->getLambda_x_wi(*parent)->getValue(1)
                              - nodesCopy.at(i).getLambda_x_wi(*parent)->getValue(1));
        } catch (std::exception &e) {
          diff = -1;
        }

        if (diff > maxDiff)
          maxDiff = diff;
      }
    }

    if (std::abs(maxDiff) < epsilon) {
      break;
    }
    maxDiff = -1;
    found = false;
    nodesCopy.clear();

    for (std::shared_ptr<Node> node: vertex_array)
      nodesCopy.push_back(*node);
    //fine calcolo differenza
  }
  //output();

}
void BayesianNetwork::inference(std::vector<std::string> inferences, std::vector<std::string> evidences) {
  int evidence;
  std::shared_ptr<Node> nodeInf;

  for (int i = 0; i < inferences.size(); i++) {
    for (std::shared_ptr<Node> n: vertex_array) {
      if (n->getLabel() == inferences.at(i))
        nodeInf = n;

    }

    //nodeInf = std::find_if(vertex_array.begin(),vertex_array.end(),[&](std::shared_ptr<Node> n){return n->getLabel()== inferences.at(i); });

    evidence = evidences.at(i) == "yes" ? 0 : 1;

    nodeInf->getLambda().setValue(evidence, 1);
    nodeInf->getLambda().setValue(std::abs(evidence - 1), 0);
  }

  ThreadPool lambdaPool, piPool, belPool, lambdaXPool, piZPool;
  int it = 0;
  while (it < vertex_array.size()) { //asssumiamo che il diametro sia pari al numero dei nodi, poichè per convergere
    // l'algortimo richiede al massimo un numero di iterazioni pari al diametro del grafo
    it++;
    lambdaPool.start();
    piZPool.start();
    belPool.start();
    lambdaXPool.start();
    piPool.start();
    for (int i = 0; i < numThreads; i++) {
      listaThread.emplace_back([&]() { lambdaXPool.runThread(); });
    }
    for (std::shared_ptr<Node> node: vertex_array) {
      for (std::shared_ptr<Node> parent: node->getParents()) {
        lambdaXPool.submit(std::bind(&Node::updateLambdaX, node, *parent));
      }
    }

    lambdaXPool.quit();
    for (std::thread &t: listaThread) {
      if (t.joinable())
        t.join();
    }
    listaThread.clear();


    for (int i = 0; i < numThreads; i++) {
      listaThread.emplace_back([&]() { piZPool.runThread(); });
    }
    for (std::shared_ptr<Node> node: vertex_array) {
      for (std::shared_ptr<Node> child: node->getChildren()) {

        piZPool.submit(std::bind(&Node::updatePiZ, node, *child));

      }
    }
    piZPool.quit();
    for (std::thread &t: listaThread) {
      if (t.joinable())
        t.join();
    }
    listaThread.clear();


    for (int i = 0; i < numThreads; i++) {
      listaThread.emplace_back([&]() { piPool.runThread(); });
    }
    for (std::shared_ptr<Node> node: vertex_array) {
      //if (nodeInf->getLabel().compare(node->getLabel())!=0) {
      piPool.submit(std::bind(&Node::updatePi, node));
      //}
      //node->updateBEL();
    }
    piPool.quit();

    for (std::thread &t: listaThread) {
      if (t.joinable()) {
        t.join();
      }
    }
    listaThread.clear();


    for (int i = 0; i < numThreads; i++) {
      listaThread.emplace_back([&]() { lambdaPool.runThread(); });
    }

    for (std::shared_ptr<Node> node: vertex_array) {
      if (!(std::any_of(inferences.begin(),
                        inferences.end(),
                        [&](const std::string &s) { return s == node->getLabel(); }))) {

        lambdaPool.submit(std::bind(&Node::updateLambda, node));
      }
      //node->updateBEL();
    }
    lambdaPool.quit();

    for (std::thread &t: listaThread) {
      if (t.joinable())
        t.join();
    }
    listaThread.clear();


    for (int i = 0; i < numThreads; i++) {
      listaThread.emplace_back([&]() { belPool.runThread(); });
    }

    for (std::shared_ptr<Node> node: vertex_array) {
      belPool.submit(std::bind(&Node::updateBEL, node));
      //node->updateBEL();
    }
    belPool.quit();
    for (std::thread &t: listaThread) {
      if (t.joinable())
        t.join();
    }
  }
}

//usata da funzione per settare etichette righe
void BayesianNetwork::recursiveLabelSet(std::vector<std::string>::size_type nPadri,
                                        int k,
                                        std::vector<std::string> list,
                                        std::vector<std::string> parentLabels,
                                        const std::shared_ptr<Matrix<double>> &m,
                                        int *j) {
  if (k == nPadri) {
    m->setRowLabels(*j, list);
    (*j)++;
    //std::for_each_n(list.begin(),list.size(),[](std::string s ) { std::cout << s<< " " ;});
    //std::cout << std::endl;
    return;
  }
  for (int i = 0; i < 2; i++) {
    if (i == 0) {
      std::string s = parentLabels.at(k);
      s += "=y";
      list.push_back(s);
    } else {
      std::string s = parentLabels.at(k);
      s += "=n";
      list.push_back(s);
    }
    recursiveLabelSet(nPadri, k + 1, list, parentLabels, m, j);
    list.pop_back();
  }
}
//per settare etichette righe
void BayesianNetwork::matrixCombination(std::vector<std::string>::size_type nPadri,
                                        std::vector<std::string> parentLabels,
                                        const std::shared_ptr<Matrix<double>> &m) {
  int nRow = 1 << nPadri;
  std::vector<std::string> list;
  int j = 0;
  recursiveLabelSet(nPadri, 0, list, std::move(parentLabels), m, &j);
}
const std::vector<std::shared_ptr<Node>> &BayesianNetwork::getVertexArray() const {
  return vertex_array;
}
