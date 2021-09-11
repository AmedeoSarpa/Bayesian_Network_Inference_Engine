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

/*
    std::cout << "Edges tree: \n";
    for (int i = 0; i < edgesTree.size(); ++i)
        std::cout << "(" << name[source(edgesTree[i],g)] << ", " << name[target(edgesTree[i],g)] <<")\n";
    std::cout << "\n";

    std::cout << "Edges back: \n";
    for (int i = 0; i < edgesBack.size(); ++i)
        std::cout << "(" << name[source(edgesBack[i],g)] << ", " << name[target(edgesBack[i],g)] <<")\n";
    std::cout << "\n";
*/

    std::cout << "Edges to delete:\n";
    for (int i = 0; i < finalEdges.size(); ++i)
        std::cout << "(" << name[source(finalEdges[i],g)] << ", " << name[target(finalEdges[i],g)] <<")\n";
    std::cout << "\n";

    return finalEdges;
}

int main()
{
    // Make convenient labels for the vertices
    //enum { A, B, C, D, E, F, G, H, N };
    double epsilon = 0.0001;
    enum {R , S , H , W, N};
//    int num_vertices = N;
//    std::string name = "ABCDEFGH";
//    std::vector<Node> vertex_array =
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
    std::string name = "RSHW";

    float v1[2] = {0.1,0.9};
    float v2[2] = {0.2,0.9};
    float v3[4] = {1,0,0.2,0.8};
    float v4[8] = {1,0,0.9,0.1,1,0,0,1};
    std::vector<std::shared_ptr<Node>> vertex_array =
            {
                    std::shared_ptr<Node>(new Node(R, "R", 2, 0, v2)),
                    std::shared_ptr<Node>(new Node(S, "S", 1, 0, v1)),
                    std::shared_ptr<Node>(new Node(H, "H", 0, 2, v4)),
                    std::shared_ptr<Node>(new Node(W, "W", 0, 1, v3))
            };




    // writing out the edges in the graph

    typedef std::pair<int, int> Edge;
    Edge edge_array[]=
            { Edge(S,H), Edge(R,H), Edge(R,W) };
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

    MyVisitor<std::vector<EdgeUndir>> vis(edgesBack, edgesTree);

    boost::depth_first_search(g, visitor(vis));

    std::vector<EdgeUndir> finalEdges = edgesToModify(edgesBack, edgesTree,g,  name);

    if(!finalEdges.empty())
        for(auto& e : finalEdges){
            add_vertex(graph);
            num_vertices++;
            name.append(1,std::tolower(name[source(e,graph)]));
            vertex_array.push_back(std::shared_ptr<Node>(new Node(num_vertices,std::to_string(std::tolower(name[source(e,graph)])),0,1,vertex_array[source(e,graph)]->getPriorTable()[0])));
            add_edge(num_vertices-1,target(e,graph),graph);
            remove_edge(source(e,graph),target(e,graph),graph);
        }

    printNodes(graph,name);

    auto vs = boost::vertices(graph);
    for (auto vit = vs.first; vit != vs.second; ++vit) {
        auto neighbors = boost::adjacent_vertices(*vit, graph);
        for (auto nit = neighbors.first; nit != neighbors.second; ++nit){
            vertex_array[*vit]->addChild(vertex_array[*nit]);
            vertex_array[*nit]->addParent(vertex_array[*vit]);
        }
    }

    printGraph(graph,name.c_str(),"associated_tree");


    return 0;
}