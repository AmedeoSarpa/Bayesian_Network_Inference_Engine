# Modifiche fatte/cose da vedere

## Main

## Node

- Informazioni piazzate nel nodo:
    - int id;
    - std::string label;
    - std::vector\<std::string> states_labels; // State labels to use as the columns of the Matrix ("present", "absent")
    - std::vector<std::shared_ptr<Node>> parents, children; // Riferimenti a nodi genitori e figli
    - Matrix _priorTable; matrice di probabilita' con numero di colonne=numero di stati e numero di righe=2\*numero di
      genitori (solo se tutti i nodi possono avere massimo 2 stati), altrimenti numero di righe=numero di stati
      genitore1\*numero di stati genitore2\*...!
    - bel, pi, lambda, pi_zi_x, lambda_x_wi //no modifiche
    - std::vector\<std::string> valueLabes; // Insieme a rowLabels e colLabels in Matrix valutare se servono davvero.
      Non e' necessario inserirli nel file xdsl, nel caso servano comunque per debugging si possono aggiungere
      ricavandole dai nodi padri

## Matrix

- std::string label; non viene usata
- std::shared_ptr<std::shared_ptr<double[]>[]> values ?
- rowLabels ((braintumor=present)|(padre1=present, padre2=absent), ricavabili dalla matrice per costuzione) e
  colLabels (stateLabels del nodo) decidere cosa fare
- printGraph non funziona piu' perche' voleva singoli caratteri per identificare i nodi, giusto? ora identificati da id
  numerico e label stringa

## RealVector

- farlo funzionare decidendo cosa fare per le labels

# TO-DO:

- Capire a cosa e' dovuto memory leak std::vector<std::shared_ptr<Node>> parents; parents.push_back(*
  parent_node_iterator);
- Togliere input dal main e metterlo in un'unica funzione.
- warning unsigned long o int, cosa usare?
- capire come passare il file il file xdsl di input ( per ora hardcoded const char *inputXdslPath = "./../Coma.xdsl";)