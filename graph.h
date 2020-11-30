#ifndef __GRAPH_H
#define __GRAPH_H

#include <bits/stdc++.h>


const int NUMV = 21;


typedef std::bitset<NUMV> Vertex;
typedef std::vector<Vertex> Graph;
typedef long long int lli;


struct Face {
	int t[3];
	
	Face(std::vector<int> v);
	Face();
	
	bool equals(const Face &f) const;
};


Graph emptyGraph(int n);
void putEdge(int a, int b, Graph &g);
void deleteEdge(int a, int b, Graph &g);
void contract(int a, int b, Graph &g);

Graph glueTorus(const Graph &g);
void printGraph(const Graph &g);

lli graphHash(const Graph &g);

bool isSimpleEqual(const Graph &g1, const Graph &g2);
bool isFaceEqual(const Graph &g1, const std::vector<Face> &f1, const Graph &g2, const std::vector<Face> &f2);
bool isSquareMapEqual(const Graph &g1, const Graph &g2);
bool isConnected(const Graph &g);
bool isTriangulated(const Graph &g);

#endif /* __GRAPH_H */ 
