#ifndef _GRAPH_H_
#define _GRAPH_H_

#include <iostream>
#include <list>

class Graph
{

  public: //leaving it public for now
    size_t n_vertices;
    size_t n_edges;
    double** matrix; //weighted adjacency matrix

  public:
    Graph(size_t vertices); //just vertices with no edges
    Graph(const char* file_name);
    Graph(const Graph& g);//copy constructor

    ~Graph();
    void addEdge(size_t u, size_t v, double w); //u,v -> vertices; w -> weight
    void removeEdge(size_t u,size_t v);
    friend std::ostream &operator<<(std::ostream &out, const Graph &G);
};

#endif