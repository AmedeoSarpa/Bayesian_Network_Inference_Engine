//
// Created by proto on 12/08/2021.
//

#define BOOST_DISABLE_PRAGMA_MESSAGE //to disable some warning at compile time

#include <iostream>                  // for std::cout
#include <utility>                   // for std::pair
#include <algorithm>                 // for std::for_each
#include "Node.hpp"
#include <boost/graph/graph_traits.hpp>
#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/graphviz.hpp>
#include <vector>
#include <ctime>
#include <algorithm>
#include <boost/graph/depth_first_search.hpp>
#include <boost/pending/indirect_cmp.hpp>

#define PATH "./.."


using namespace boost;
// create a typedef for the Graph type
typedef adjacency_list<vecS, vecS, directedS> Graph;
typedef adjacency_list<vecS, vecS, undirectedS> GraphUndir;
typedef graph_traits<Graph>::vertex_descriptor Vertex;
typedef graph_traits<Graph>::edge_descriptor EdgeDir;
typedef graph_traits<GraphUndir>::edge_descriptor EdgeUndir;

template <typename G>
void printGraph(G g, const char* name, std::string title){
    std::ofstream fout(std::string(PATH)+"/"+title+".dot");

    if(!fout){
        std::cout << "Error opening file output_graph.dot\n";
        exit(1);
    }
    write_graphviz(fout, g, make_label_writer(name));
    /* ON TERMINAL (WINDOWS)
     * dot -Tsvg output_graph.dot > output_graph.svg
     */

    fout.close();
}

template <typename G>
void printEdges(G g, std::string name){
    auto es = boost::edges(g);
    for (auto eit = es.first; eit != es.second; ++eit) {
        std::cout << name[boost::source(*eit, g)] << ' ' << name[boost::target(*eit, g)] << std::endl;
    }
}

template <typename G>
void printNodes(G g, std::string name){
    auto vs = boost::vertices(g);
    for (auto vit = vs.first; vit != vs.second; ++vit) {
        auto neighbors = boost::adjacent_vertices(*vit, g);
        for (auto nit = neighbors.first; nit != neighbors.second; ++nit)
            std::cout << name[*vit] << ' ' << name[*nit] << std::endl;
    }
}

template <typename TimeMap>
class dfs_time_visitor : public default_dfs_visitor
{
    typedef typename property_traits<TimeMap>::value_type T;

public:
    dfs_time_visitor(TimeMap dmap, TimeMap fmap, T& t)
            : m_dtimemap(dmap), m_ftimemap(fmap), m_time(t)
    {
    }
    template < typename Vertex, typename Graph >
    void discover_vertex(Vertex u, const Graph& g) const
    {
        put(m_dtimemap, u, m_time++);
    }
    template < typename Vertex, typename Graph >
    void finish_vertex(Vertex u, const Graph& g) const
    {
        put(m_ftimemap, u, m_time++);
    }
    TimeMap m_dtimemap;
    TimeMap m_ftimemap;
    T& m_time;
};

void dfsTimeVisitor(Graph g, int N, int num_vertices, std::string name){
    typedef graph_traits<Graph>::vertices_size_type size_type;

    // discover time and finish time properties
    std::vector< size_type > dtime(num_vertices);
    std::vector< size_type > ftime(num_vertices);
    typedef iterator_property_map< std::vector< size_type >::iterator,
            property_map< Graph, vertex_index_t >::const_type >
            time_pm_type;
    time_pm_type dtime_pm(dtime.begin(), get(vertex_index, g));
    time_pm_type ftime_pm(ftime.begin(), get(vertex_index, g));
    size_type t = 0;

    dfs_time_visitor< time_pm_type > vis(dtime_pm, ftime_pm, t);

    depth_first_search(g, visitor(vis));

    // use std::sort to order the vertices by their discover time
    std::vector< size_type > discover_order(N);
    integer_range< size_type > r(0, N);
    std::copy(r.begin(), r.end(), discover_order.begin());
    std::sort(discover_order.begin(), discover_order.end(),
              indirect_cmp< time_pm_type, std::less< size_type > >(dtime_pm));
    std::cout << "order of discovery: ";
    int i;
    for (i = 0; i < N; ++i)
        std::cout << name[discover_order[i]] << " ";

    std::vector< size_type > finish_order(N);
    std::copy(r.begin(), r.end(), finish_order.begin());
    std::sort(finish_order.begin(), finish_order.end(),
              indirect_cmp< time_pm_type, std::less< size_type > >(ftime_pm));

    std::cout << std::endl << "order of finish: ";
    for (i = 0; i < N; ++i)
        std::cout << name[finish_order[i]] << " ";
    std::cout << std::endl;
}

template <typename EdgeList>
class MyVisitor : public default_dfs_visitor
{
public:
    MyVisitor(EdgeList& eBack, EdgeList& eTree ): _edgesBack(eBack), _edgesTree(eTree){};
    template <typename Vertex, typename Graph>
    void discover_vertex(Vertex v, const Graph& g){
        //std::cout << "D\n";
    }
    template <typename Edge, typename Graph>
    void tree_edge(Edge e, const Graph& g)
    {
        //std::cout << "T\n";
        _edgesTree.push_back(e);
    }
    template <typename Edge, typename Graph>
    void back_edge(Edge e, const Graph& g)
    {
        //std::cout << "B\n";
        _edgesBack.push_back(e);
    }
private:
    EdgeList& _edgesBack;
    EdgeList& _edgesTree;
};

template <typename E,typename G>
std::vector<E> edgesToModify(std::vector<E> edgesBack,std::vector<E> edgesTree,G g, std::string name){
    std::vector<EdgeUndir> finalEdges;

    std::for_each(edgesTree.begin(),edgesTree.end(),[g](auto& e){
        if(source(e,g)>target(e,g))
            e = boost::edge(target(e,g),source(e,g),g).first;
    });

    std::sort(edgesTree.begin(),edgesTree.end(),[g](auto a, auto b){
        return source(a,g)<source(b,g);
    });

    std::for_each(edgesBack.begin(),edgesBack.end(),[g](auto& e){
        if(source(e,g)>target(e,g))
            e = boost::edge(target(e,g),source(e,g),g).first;
    });

    std::sort(edgesBack.begin(),edgesBack.end(),[g](auto a, auto b){
        return source(a,g)<source(b,g);
    });

    std::set_difference(edgesBack.begin(),edgesBack.end(),edgesTree.begin(),edgesTree.end(),std::inserter(finalEdges,finalEdges.begin()),[g](auto a, auto b){
        return source(a,g)!=source(b,g)||target(a,g)!=target(b,g);
    });


    std::cout << "Edges to delete:\n";
    for (int i = 0; i < finalEdges.size(); ++i)
        std::cout << "(" << name[source(finalEdges[i],g)] << ", " << name[target(finalEdges[i],g)] <<")\n";
    std::cout << "\n";

    return finalEdges;
}


void inference(std::vector<std::shared_ptr<Node>> vertex_array){

}

int main()
{
    // Make convenient labels for the vertices
    enum { A, S, T, L, E , B, X ,D, N };
    double epsilon = 10e-11;
    //enum {R , S , W , H, N};
//    int num_vertices = N;
//    std::string name = "ABCDEFGH";
    std::vector<std::shared_ptr<Node>> vertex_array ;
//            {
//            Node(A,"A",2,0), Node(B,"B",1,1), Node(C,"C",2,0), Node(D,"D",1,1),
//            Node(E,"E",1,1), Node(F,"F",2,2), Node(G,"G",0,2), Node(H,"H",0,1)
//            };
//
//    // writing out the edges in the graph
//
//    typedef std::pair<int, int> Edge;
//    std::vector<Edge> edge_array =
//            { Edge(A,B), Edge(A,C), Edge(B,G), Edge(C,D),
//              Edge(C,E), Edge(D,F), Edge(E,F), Edge(F,G), Edge(F,H) };
//    const int num_edges = sizeof(edge_array)/sizeof(edge_array[0]);

    int num_vertices = N;
    std::string name = "ASTLEBXD";
    std::vector<std::string> yn1;
    yn1.push_back("A=y");yn1.push_back("A=n");
    std::vector<std::string> yn2;
    yn2.push_back("S=y");yn2.push_back("S=n");
    std::vector<std::string> yn3;
    yn3.push_back("T=y");yn3.push_back("T=n");
    std::vector<std::string> yn4;
    yn4.push_back("L=y");yn4.push_back("L=n");

    std::vector<std::string> yn5;
    yn5.push_back("E=y");yn5.push_back("E=n");
    std::vector<std::string> yn6;
    yn6.push_back("B=y");yn6.push_back("B=n");
    std::vector<std::string> yn7;
    yn7.push_back("X=y");yn7.push_back("X=n");
    std::vector<std::string> yn8;
    yn8.push_back("D=y");yn8.push_back("D=n");

    std::shared_ptr<Node> nodoA(new Node("A",0,2,yn1)),
    nodoS(new Node("S",1,2,yn2)),nodoT(new Node("T",2,2,yn3)),
    nodoL(new Node("L",3,2,yn4)),nodoE(new Node("E",4,2,yn5)),
    nodoB(new Node("B",5,2,yn6)),nodoX(new Node("X",6,2,yn7)),nodoD(new Node("D",7,2,yn8));

    double arrayA[2] = {0.01,0.99};
    double arrayT[4] = {0.05,0.95,0.01,0.99};
    double arrayS[2] = {0.5,0.5};
    double arrayL[4] = {0.1,0.9, 0.01,0.99};
    double arrayB[4] = {0.6,0.4,0.3,0.7};
    double arrayE[8] = {1,0,1,0,1,0,0,1};
    double arrayX[4] = {0.98,0.2,0.05,0.95};
    double arrayD[8] = {0.9,0.1,0.7,0.3,0.8,0.2,0.1,0.9};


    Matrix Md(4,2,arrayD);
    std::vector<std::string> mh1;
    mh1.push_back("B=y");mh1.push_back("E=y");
    std::vector<std::string> mh2;
    mh2.push_back("B=n");mh2.push_back("E=y");
    std::vector<std::string> mh3;
    mh3.push_back("B=y");mh3.push_back("E=n");
    std::vector<std::string> mh4;
    mh4.push_back("B=n");mh4.push_back("E=n");
    std::vector<std::string> mh5;
    mh5.push_back("D=y");
    std::vector<std::string> mh6;
    mh6.push_back("D=n");

    Md.setColLabels(0, mh5);
    Md.setColLabels(1, mh6);
    Md.setRowLabels(0, mh1);
    Md.setRowLabels(1, mh2);
    Md.setRowLabels(2, mh3);
    Md.setRowLabels(3, mh4);
    nodoD->setMx_wAll(Md);

    Matrix Me(4,2,arrayE);
    std::vector<std::string> m1;
    m1.push_back("L=y");m1.push_back("T=y");
    std::vector<std::string> m2;
    m2.push_back("L=n");m2.push_back("T=y");
    std::vector<std::string> m3;
    m3.push_back("L=y");m3.push_back("T=n");
    std::vector<std::string> m4;
    m4.push_back("L=n");m4.push_back("T=n");
    std::vector<std::string> m5;
    m5.push_back("E=y");
    std::vector<std::string> m6;
    m6.push_back("E=n");

    Me.setColLabels(0, m5);
    Me.setColLabels(1, m6);
    Me.setRowLabels(0, m1);
    Me.setRowLabels(1, m2);
    Me.setRowLabels(2, m3);
    Me.setRowLabels(3, m4);
    Me.setLabel("matriceE");
    nodoE->setMx_wAll(Me);




    Matrix Ma(1,2,arrayA);
    nodoA->setMx_wAll(Ma);
    Matrix Ms(1,2,arrayS);
    nodoS->setMx_wAll(Ms);





    Matrix Mt(2,2,arrayT);
    std::vector<std::string> m1t;
    m1t.push_back("T=y");
    std::vector<std::string> m2t;
    m2t.push_back("T=n");
    std::vector<std::string> m3t;
    m3t.push_back("A=y");
    std::vector<std::string> m4t;
    m4t.push_back("A=n");
    Mt.setColLabels(0, m1t);
    Mt.setColLabels(1, m2t);
    Mt.setRowLabels(0, m3t);
    Mt.setRowLabels(1, m4t);
    nodoT->setMx_wAll(Mt);

    Matrix Ml(2,2,arrayL);
    std::vector<std::string> m1l;
    m1l.push_back("L=y");
    std::vector<std::string> m2l;
    m2l.push_back("L=n");
    std::vector<std::string> m3l;
    m3l.push_back("S=y");
    std::vector<std::string> m4l;
    m4l.push_back("S=n");
    Ml.setColLabels(0, m1l);
    Ml.setColLabels(1, m2l);
    Ml.setRowLabels(0, m3l);
    Ml.setRowLabels(1, m4l);
    nodoL->setMx_wAll(Ml);


    Matrix Mb(2,2,arrayB);
    std::vector<std::string> m1b;
    m1b.push_back("B=y");
    std::vector<std::string> m2b;
    m2b.push_back("B=n");
    std::vector<std::string> m3b;
    m3b.push_back("S=y");
    std::vector<std::string> m4b;
    m4b.push_back("S=n");
    Mb.setColLabels(0, m1b);
    Mb.setColLabels(1, m2b);
    Mb.setRowLabels(0, m3b);
    Mb.setRowLabels(1, m4b);
    nodoB->setMx_wAll(Mb);



    Matrix Mx(2,2,arrayX);
    std::vector<std::string> m1x;
    m1x.push_back("X=y");
    std::vector<std::string> m2x;
    m2x.push_back("X=n");
    std::vector<std::string> m3x;
    m3x.push_back("E=y");
    std::vector<std::string> m4x;
    m4x.push_back("E=n");
    Mx.setColLabels(0, m1x);
    Mx.setColLabels(1, m2x);
    Mx.setRowLabels(0, m3x);
    Mx.setRowLabels(1, m4x);
    nodoX->setMx_wAll(Mx);

    nodoA->addChild(nodoT);
    nodoT->addParent(nodoA);
    nodoS->addChild(nodoL);
    nodoL->addParent(nodoS);
    nodoS->addChild(nodoB);
    nodoB->addParent(nodoS);
    nodoT->addChild(nodoE);
    nodoE->addParent(nodoT);
    nodoL->addChild(nodoE);
    nodoE->addParent(nodoL);
    nodoB->addChild(nodoD);
    nodoD->addParent(nodoB);
    nodoE->addChild(nodoD);
    nodoD->addParent(nodoE);
    nodoE->addChild(nodoX);
    nodoX->addParent(nodoE);

    vertex_array.push_back(nodoX);
    vertex_array.push_back((nodoD));
    vertex_array.push_back((nodoE));
    vertex_array.push_back((nodoB));
    vertex_array.push_back((nodoL));
    vertex_array.push_back((nodoT));
    vertex_array.push_back((nodoS));
    vertex_array.push_back((nodoA));


    std::sort(vertex_array.begin(),vertex_array.end(),[](std::shared_ptr<Node> l,std::shared_ptr<Node> r){ return  l->getChildren().size() < r->getChildren().size();});



    // writing out the edges in the graph
    typedef std::pair<int, int> Edge;

    Edge edge_array[] =
            { Edge(A,T), Edge(T,E), Edge(S,L), Edge(L,E),
              Edge(S,B), Edge(B,D), Edge(E,D), Edge(E,X) };
    //Edge edge_array[]=
     //       { Edge(A,B), Edge(B,G), Edge(A,C) , Edge(C,D) , Edge(C,E) , Edge(E,F) , Edge(D,F) , Edge(F,G) , Edge(F,H) };
    const int num_edges = sizeof(edge_array)/sizeof(edge_array[0]);


//    // declare a graph object
//    Graph g(num_vertices);
//
//    // add the edges to the graph object
//    for (int i = 0; i < num_edges; ++i)
//        add_edge(edge_array[i].first, edge_array[i].second, g);

    // using the edge iterator constructor of the graph. This is typically more efficient than using add_edge()
    Graph graph(edge_array, edge_array + sizeof(edge_array) / sizeof(Edge), num_vertices);
    GraphUndir g(edge_array, edge_array + sizeof(edge_array) / sizeof(Edge), num_vertices);

    printGraph(graph,name.c_str(),"output_graph");
    printGraph(g,name.c_str(),"output_graphUndir");

    //dfsTimeVisitor(g,N, num_vertices, name);

    std::vector<EdgeUndir> edgesBack, edgesTree;


    /*
    MyVisitor<std::vector<EdgeUndir>> vis(edgesBack, edgesTree);

    boost::depth_first_search(g, visitor(vis));

    std::vector<EdgeUndir> finalEdges = edgesToModify(edgesBack, edgesTree,g,  name);

    Node nodeToPut;

    if(!finalEdges.empty())
        for(auto& e : finalEdges){
            std::string yes,no,lb;
            std::vector<std::string> labels;
            add_vertex(graph);
            num_vertices++;
            no+=(name[source(e,graph)]);
            yes+=(name[source(e,graph)]);
            lb+=(name[source(e,graph)]);
            yes+="=y";
            no+="=n";
            labels.clear();
            labels.push_back(yes);labels.push_back(no);
            name.append(1,(name[source(e,graph)]));

            Node newNode(lb,num_vertices-1,2,labels);
            lb="";
            yes=no="";

            for (int i = 0; i < vertex_array.size() ; i++){
                if (vertex_array.at(i)->getId() == source(e,graph)) { newNode.setMx_wAll(*vertex_array.at(i)->getMx_wAll()); }

            }
            Node* node;
            Node * oldNode;
            for (int i = 0; i < vertex_array.size() ; i++){
                if (vertex_array.at(i)->getId() == target(e,graph)) { node = vertex_array.at(i); }

            }
            for (int i = 0; i < vertex_array.size() ; i++){
                if (vertex_array.at(i)->getId() == source(e,graph)) { oldNode = vertex_array.at(i); }

            }

            newNode.addChild(*node);

            nodeToPut = newNode;

            node->addParent(nodeToPut);
            node->removeParent(source(e,graph));
            oldNode->removeChild(node->getId());
            //come aggiungo i figli a questo nuovo nodo?

            vertex_array.push_back(&nodeToPut);

            add_edge(num_vertices-1,target(e,graph),graph);
            remove_edge(source(e,graph),target(e,graph),graph);
        }
        */

    printNodes(graph,name);
    printGraph(graph,name.c_str(),"associated_tree");
    std::vector<Node> nodesCopy;



    for (int i = 0 ; i < vertex_array.size() ; i++){
        RealVector* v;
        if (vertex_array.at(i)->getChildren().size() == 0){
            v = vertex_array.at(i)->getLambda();
            v->setValue(0,1);
            v->setValue(1,1);


        }
        if (vertex_array.at(i)->getParents().size() == 0){
            v = vertex_array.at(i)->getPi();
            v->setValue(0,vertex_array.at(i)->getMx_wAll()->getValue(0,0));
            v->setValue(1,vertex_array.at(i)->getMx_wAll()->getValue(0,1));
        }
        v = nullptr;
    }

    RealVector *v;

    for(int j = vertex_array.size()-1 ; j >=0 ; j--){
        nodesCopy.push_back(*(vertex_array.at(j)));
    }


    int it = 0;
    bool found = false;
    double maxDiff = -1,diff;
    std::clock_t  begin = clock();
    while(true){ /* nella costruzione del grafo , i nodi fogli hanno precedenza */


        for ( std::shared_ptr<Node> node : vertex_array){
            for (std::shared_ptr<Node> parent : node->getParents()){
                node->updateLambdaX(*parent.get());
            }
            for (std::shared_ptr<Node> child : node->getChildren()){
                node->updatePiZ(*child.get());
            }
        }



        for ( std::shared_ptr<Node> node : vertex_array){
            node->updatePi();
            node->updateLambda();
        }


        for ( std::shared_ptr<Node> node : vertex_array) {
                node->updateBEL();
        }
        for ( int i = 0 ; i < vertex_array.size() ; i++) {

            if (vertex_array.at(i)->getBel()->getValue(0) == 0 &&
                vertex_array.at(i)->getBel()->getValue(1) == 0) { maxDiff = -1;found = true; }
        }
        for ( int i = 0 ; i < vertex_array.size() && found == false; i++) {

            for (std::shared_ptr<Node> child : vertex_array.at(i)->getChildren()) {
                try {
                    diff = std::abs(vertex_array.at(i)->getPi_zi_x(*child).get()->getValue(0) -
                                    nodesCopy.at(i).getPi_zi_x(*child).get()->getValue(0));
                }
                catch (std::exception e) {}
                if (diff > maxDiff) maxDiff = diff;

                try {
                    diff = std::abs(vertex_array.at(i)->getPi_zi_x(*child).get()->getValue(1) -
                                    nodesCopy.at(i).getPi_zi_x(*child).get()->getValue(1));
                }
                catch (std::exception e) {}

                if (diff > maxDiff) maxDiff = diff;
            }

            for (std::shared_ptr<Node> parent : vertex_array.at(i)->getParents()) {
                try {
                    diff = std::abs(vertex_array.at(i)->getLambda_x_wi(*parent).get()->getValue(0) -
                                    nodesCopy.at(i).getLambda_x_wi(*parent).get()->getValue(0));
                } catch (std::exception e) {}

                if (diff > maxDiff) maxDiff = diff;
                try {
                    diff = std::abs(vertex_array.at(i)->getLambda_x_wi(*parent).get()->getValue(1) -
                                    nodesCopy.at(i).getLambda_x_wi(*parent).get()->getValue(1));
                } catch (std::exception e) {}

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
        for(int j = 0 ; j <= vertex_array.size()-1 ; j++){
            nodesCopy.push_back(*(vertex_array.at(j)));
        }
    }
    v = nullptr;
    std::clock_t  end = clock();
    std::cout << "numero iterazioni :" << it << " max Diff " << maxDiff <<  " tempo esecuzione (ms) " << double(end-begin) << std::endl;
    std::for_each_n(vertex_array.begin(), vertex_array.size(),[](std::shared_ptr<Node> n) {n->printValues();});
    return 1;
}