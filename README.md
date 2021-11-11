# Bayesian Networks e parallelismo
Protopapa Andrea 286302
<br>Sarpa Amedeo 281638
<br>Sattolo Francesco 290158

Repository gitHub : https://github.com/AmedeoSarpa/progettoPDS/tree/finalVersion

**Requisiti Progetto**
<br>*Algortimo scelto* : "Pearl's Belief Propagation Algorithm" , algortimo esatto  che converge e fornisce sempre un risultato esatto
per reti tree-structured. In caso di grafi con cicli , questo algoritmo fornisce un risultato approssimato.
Tale algortimo è basato sullo scambio di messaggi tra padri e figli , una volta che un nodo riceve tali valori è in grado di calcolare
il 'belief' cioè la probabilità a posteriori.
Una limitazione nel nostro algoritmo è che funziona solo per nodi con due stati.
<br> Referenze : https://arxiv.org/ftp/arxiv/papers/1301/1301.6725.pdf (qui viene spiegato come funziona l'algortimo e l'approssimazione che si ha in caso di grafi con loops)

*COW* : La copy-on-write è stata realizzata inserendo gli attributi delle classi 'Node','Matrix' e 'RealVector' in una struttura dati. Si è inserito poi in ciascuna classe , come unico attributo,
uno shared pointer a tale struttura. Soloo in caso di operazioni di scrittura  viene effettuata la copia, duplicando tale shared pointer per due diverse ( a seguito della scrittura) referenze.

*Parallelizzazione* : implementata tramite l'uso di thread , vengono utilizzati diversi thread pool ognuno per ogni operazione da svolgere in ogni nodo (in questa configurazione vengono sganciati 3 thread).

*Template* : le classi generiche sono 'RealVector' e 'Matrix' , una versione da noi customizzata di vettori e matrici. Nel nostro caso il tipo geenrico è 'double'

*Allocatore polimorfico* : 'monotonic_buffer_resource' utilizzato  nel vettore 'nodesCopy' che serve a calcolare la differenza dei calcoli con l'iterazione precedente. Tale vettore ha una lunghezza che è pari a quella dei nodi del grafo e deve essere liberato una volta che non serve più. 

*I/O formato xdsl* : 

**Utilizzo**
<br>Tutte le operazioni, quali il calcolo iniziale , inferenza e i/o , sono fornite dalla classe 'BayesianNetwork'.
Basta istanziare un ogetto di tale classe nel main (costruttore non richiede parametri) ed invocare i seguenti metodi da tale oggetto :
- input : riceve come parametro il path del file xdsl e ne esegue la lettura
- compute : una volta eseguito l'input , esegue le propagazioni dei messaggi e calcola per ogni nodo la probababilità distribuita (BEL)
- inference : invocato dopo 'compute' riceve come parametri due vettori e riesegue il calcolo del BEL a seguito dell'inferenza.<br>
I due vettori sono di stringhe , il primo indica i nomi dei nodi per i quali voglio fare inferenza ed il secondo indica per ciascuno nodo l'evidenza ('yes' o 'no').
- output : si può invocare a seguito di inferenze o anche dopo i calcoli iniziali , mostra su console e memorizza in un file xdsl l'output dei calcoli , cioè il BEL di ogni nodo

**Esempi**
<br>Per verificare il funzionamento dell'algoritmo abbiamo utilizzato diversi esempi , alcuni dei quali disponibili ai link seguenti :<br>
https://www.bayesserver.com/examples/networks/asia
<br>http://www.cse.unsw.edu.au/~cs9417ml/Bayes/Pages/PearlExamples.html
