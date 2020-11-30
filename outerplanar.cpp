#include "graph.h"
#include "outerplanar.h"

#include <bits/stdc++.h>
using namespace std;


vector<Graph> triangulations[MAXN+10];
vector<Graph> outerplanars[MAXN+10];

// Lists all the possible triangulations of n-gons.
// Stores the result on vector 'triangulations'.
void getTriangulations(int n) {
	if (n < 2) {
		triangulations[n].push_back(emptyGraph(n));
		return;
	}
	
	if (n == 2) {
		auto g = emptyGraph(n);
		putEdge(0, 1, g);
		triangulations[n].push_back(g);
		return;
	}
	
	if (n == 3) {
		auto g = emptyGraph(3);
		for (int a=0; a<3; a++) {
			for (int b=0; b<a; b++) {
				putEdge(a, b, g);
			}
		}
		
		triangulations[3].push_back(g);
		return;
	}
	
	for (int i=1; i<n-1; i++) {
		for (const auto &t1 : triangulations[i+1]) {
			for (const auto &t2 : triangulations[n-i]) {
				auto g = t1;
				g.resize(n);
				for (int a=0; a<n-i; a++) {
					for (int b=0; b<a; b++) {
						if (t2[a][b])
							putEdge(a+i, b+i, g);
					}
				}
				putEdge(0, n-1, g);
				putEdge(0, i, g);
				putEdge(n-1, i, g);
				triangulations[n].push_back(g);
			}
		}
	}
}

// Computes all the connected subgraphs of the given graph.
// Stores the resulting set on the list 'out'.
// This function does not try to avoid duplicates.
void getSubgraphs(const Graph &graph, vector<Graph> &out) {
	int n = graph.size();
	int m = 0;
	
	for (int a=0; a<n; a++) {
		for (int b=0; b<a; b++) {
			if (graph[a][b])
				m++;
		}
	}
	
	for (int mas=0; mas<(1<<m); mas++) {
		auto g = graph;
		int it = 0;
		
		for (int a=0; a<n; a++) {
			for (int b=0; b<a; b++) {
				if (!graph[a][b])
					continue;
				if (mas&(1<<it))
					deleteEdge(a, b, g);
				it++;
			}
		}
		
		if (!isConnected(g))
			continue;
		out.push_back(g);
	}
}

// Deduplicates graphs on the given vector.
// Compares every pair of given graphs.
// Firstly tries to use simple heuristics, such as the set of degrees.
// If required, checks all the bijections to find isomorphism.
// This works fast provided that the checked graphs are small enough,
// which is the case in this project.
void deduplicate(vector<Graph> &out) {
	const int divs = 10007;
	vector<Graph> res[divs];
	
	for (const auto &g1 : out) {
		lli h = graphHash(g1)%divs;
		bool duplicate = 0;
		
		for (const auto &g2 : res[h]) {
			if (isSimpleEqual(g1, g2)) {
				duplicate = 1;
				break;
			}
		}
		
		if (!duplicate)
			res[h].push_back(g1);
	}
	
	out.clear();
	for (int i=0; i<divs; i++) {
		for (const auto &g : res[i]) {
			if (isTriangulated(g))
				out.push_back(g);
		}
	}
}

// Computes the border of the given outerplanar graph,
// i.e. the list of consecutive vertices on its outer face.
// Note that a single vertex can appear there several times.
vector<int> border(const Graph &g) {
	int n = g.size();
	vector<int> res = {};
	
	for (int i=0; i<n; i++) {
		int tmp = i;
		
		while (true) {
			for (int j=(i+1)%n; j!=i; j=(j+1)%n) {
				if (!g[tmp][j])
					continue;
				tmp = j;
				res.push_back(j);
				break;
			}
			
			if (tmp == (i+1)%n)
				break;
		}
	}
	
	return res;
}

// Prepares lists of all the outerplanar graphs.
// Groups the result by the number of vertices and store in the list 'outerplanars'.
// After computation, all the graphs are deduplicated wrt. graph isomorphism.
void getOuterplanars() {
	for (int i=0; i<=MAXN; i++) {
		getTriangulations(i);
		for (const auto &g : triangulations[i])
			// All the n-outerplanar graphs are subgraphs of triangulated n-gons.
			getSubgraphs(g, outerplanars[i]);
		deduplicate(outerplanars[i]);
		
		printf("outerplanars[%d]: %d\n", i, (int)outerplanars[i].size());
	}
	printf("\n");
}
