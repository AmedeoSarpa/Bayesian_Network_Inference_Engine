# Bayesian Networks e parallelismo
Studenti :<br>
Protopapa Andrea 286302
<br>Sarpa Amedeo 281638
<br>Sattolo Francesco 290158


**Requisiti Progetto**
<br>*Algoritmo scelto* : "Pearl's Belief Propagation Algorithm" , algoritmo esatto  che converge e fornisce sempre un risultato esatto
per reti singly connected. In caso di grafi multiply connected, questo algoritmo fornisce un risultato approssimato.
Tale algoritmo è basato sullo scambio di messaggi tra padri e figli , una volta che un nodo riceve tali valori è in grado di calcolare
il 'belief' cioè la probabilità a posteriori.
Una limitazione nel nostro algoritmo è che funziona solo per nodi con due stati.
<br> Referenze : https://arxiv.org/ftp/arxiv/papers/1301/1301.6725.pdf (qui viene spiegato come funziona algoritmo e l'approssimazione che si ha in caso di grafi con loops)

*COW* : La copy-on-write è stata realizzata inserendo gli attributi delle classi 'Node','Matrix' e 'RealVector' in una struttura dati. Si è inserito poi in ciascuna classe , come unico attributo,
uno shared pointer a tale struttura. Solo in caso di operazioni di scrittura  viene effettuata la copia, duplicando tale shared pointer per due diverse ( a seguito della scrittura) referenze.

*Parallelizzazione* : implementata tramite l'uso di thread , vengono utilizzati diversi thread pool ognuno per ogni operazione da svolgere in ogni nodo (in questa configurazione vengono sganciati un numero di thread indicato dalla variabile 'numThreads' della classe 'BayesianNetworks' ).

*Template* : le classi generiche sono 'RealVector' e 'Matrix' , una versione da noi customizzata di vettori e matrici. Nel nostro caso il tipo generico è 'double'

*Allocatore polimorfico* : 'monotonic_buffer_resource' utilizzato  nel vettore 'nodesCopy' che serve a calcolare la differenza dei calcoli con l'iterazione precedente. Tale vettore ha una lunghezza che è pari a quella dei nodi del grafo e deve essere liberato una volta che non serve più. 

*I/O formato xdsl* : la lettura e il parsing del file xdsl sfruttano i metodi provenienti dalla libreria pugixml. Tutti i nodi e le loro strutture dati vengono inizializzati con le informazioni presenti nel file.

**Utilizzo**
<br>L'algoritmo è  invocato da linea di comando : Dopo aver 'buildato' il programma, aprire col terminale la directory 'cmake-build-debug' , runnare il 
file eseguibile 'projectPDS' specificando il path del file input in formato xdsl.<br> 
Esempio :  .\cmake-build-debug\projectPDS.exe ./IOFiles/Asia.xdsl<br>
Il programma è interattivo , all'avvio vengono mostrati i calcoli iniziali eseguiti con in dati in input, digitando 1 è possibile specificare il nodo su cui fare inferenza.
Con il tasto 2 si può stampare su un file di cui il nome viene specificato dall'utente , l'output corrente. Con il tasto 3 si termina.


**Esempi**
<br>Per verificare il funzionamento dell'algoritmo abbiamo utilizzato diversi esempi , alcuni dei quali disponibili ai link seguenti :<br>
https://www.bayesserver.com/examples/networks/asia
<br>http://www.cse.unsw.edu.au/~cs9417ml/Bayes/Pages/PearlExamples.html
