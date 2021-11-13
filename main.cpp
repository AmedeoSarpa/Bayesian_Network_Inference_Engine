#include <fstream>
#include "BayesianNetwork.h"

int main(int argc, char* argv[]) {
  BayesianNetwork bn;
  if (argc > 1) {
    if (!bn.input(argv[1])) {
      std::cout << "Path do not exist\n";
      return -1;
    }
  }
  else{
    std::cout << "Usage: ./projectPDS <xdsl_file_path>\n";
    return 0;
  }
  bn.compute();
  bn.output(std::cout);

  std::vector<std::string> inferences;
  std::vector<std::string> evidences;

  int choice = 0;
  std::string selectedNode;
  std::string selectedEvidence;
  while (choice != 3) {
    std::cout << "\nSelect operation:\n1: New inference\n2: Extract result to file\n3: Exit\n";
    std::cin >> choice;
    std::shared_ptr<Node> nodeInf = nullptr;
    std::string outputFilename;
    std::ofstream outputFile;
    switch (choice) {
      case 1:std::cout << "Select the node:\n";
        for (const std::shared_ptr<Node> &node: bn.getVertexArray()) {
          std::cout << node->getLabel() << '\t';
        }
        std::cout << '\n';
        std::cin >> selectedNode;
        for (const std::shared_ptr<Node> &node: bn.getVertexArray()) {
          if (selectedNode == node->getLabel())
            nodeInf = node;
        }
            if (nodeInf == nullptr) {
                std::cout << "Inference failed: incorrect node selected.\n";
                break;
            }
            std::cout << "Select the evidence:\n";
            std::cout << "yes\tno\n";
            nodeInf->getBel()->printTest(std::cout);

            std::cin >> selectedEvidence;
            if(selectedEvidence!="yes" && selectedEvidence!="no"){
                std::cout << "Inference failed: incorrect evidence selected.\n";
                break;
            }

            inferences.emplace_back(selectedNode);
            evidences.emplace_back(selectedEvidence);
        /*      inferences.emplace_back("MetastCancer");
                evidences.emplace_back("yes");*/

        bn.inference(inferences, evidences);
        bn.output(std::cout);
        break;
      case 2:std::cout << "Insert filename: ";
        std::cin >> outputFilename;
        outputFile.open(outputFilename);
        bn.output(outputFile);
        outputFile.close();
        std::cout << "Data written correctly\n";
        break;
      case 3:break;
      default:break;
    }
  }
}