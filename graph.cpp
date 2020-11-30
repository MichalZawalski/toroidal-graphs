#include "graph.h"

#include <bits/stdc++.h>
using namespace std;


Face::Face(vector<int> v) {
	for (int i=0; i<3; i++)
		t[i] = v[i];
}

Face::Face() {
	for (int i=0; i<3; i++)
		t[i] = -1;
}

bool Face::equals(const Face &f) const {
	for (int i=0; i<3; i++) {
		if (t[i] == f.t[0] && t[(i+1)%3] == f.t[1] && t[(i+2)%3] == f.t[2])
			return true;
		if (t[i] == f.t[0] && t[(i+1)%3] == f.t[2] && t[(i+2)%3] == f.t[1])
			return true;
	}
	return false;
}


// Returns empty graph.
Graph emptyGraph(int n) {
	Graph x;
	x.resize(n);
	return x;
}

// Adds an edge to the given graph.
void putEdge(int a, int b, Graph &g) {
	g[a][b] = 1;
	g[b][a] = 1;
}

// Deletes the given edge.
void deleteEdge(int a, int b, Graph &g) {
#ifdef LOCAL
	assert(g[a][b] == 1);
	assert(g[b][a] == 1);
#endif
	g[a][b] = 0;
	g[b][a] = 0;
}

// Contracts the given edge,
// i.e. removes the endpoints and replaces with a single
// vertex connected to all their neighbours.
void contract(int a, int b, Graph &g) {
	if (a > b)
		swap(a, b);
#ifdef LOCAL
	assert(a != b);
#endif
	g[a] |= g[b];
	g[b].reset();
	for (int i=0; i<(int)g.size(); i++) {
		g[i][a] = g[i][a] | g[i][b];
		g[i][b] = 0;
	}
}

// Glues the corresponding pairs of border vertices.
Graph glueTorus(const Graph &g) {
	auto res = g;
	
	contract(0, 3, res);
	contract(0, 6, res);
	contract(0, 9, res);
	
	contract(1, 8, res);
	contract(2, 7, res);
	contract(4, 11, res);
	contract(5, 10, res);
	
	return res;
}

void glueFaces(vector<Face> &faces) {
	vector<int> glue;
	for (int i=0; i<30; i++)
		glue.push_back(i);
	glue[3] = 0;
	glue[6] = 0;
	glue[9] = 0;
	
	glue[7] = 2;
	glue[8] = 1;
	glue[10] = 5;
	glue[11] = 4;
	
	for (auto &f : faces)
		for (int i=0; i<3; i++)
			f.t[i] = glue[f.t[i]];
}

// Simple heuristic for checking graph equality.
// Hashes the list of vertices' degrees.
// Note that graphs having different hashes cannot be equal.
lli graphHash(const Graph &g) {
	lli h = 0;
	vector<lli> degs;
	
	for (int i=0; i<(int)g.size(); i++)
		degs.push_back(g[i].count());
	
	sort(degs.begin(), degs.end());
	for (lli d : degs)
		h = h*NUMV + d;
	return h;
}

// Prints the given graph.
void printGraph(const Graph &g) {
	printf("graph\n");
	for (int i=0; i<(int)g.size(); i++) {
		if (!g[i].any())
			continue;
		printf("%d: ", i);
		for (int j=0; j<NUMV; j++) {
			if (g[i][j])
				printf(" %d", j);
		}
		printf("\n");
	}
	printf("\n");
}

// Checks whether given vectors represent equal sets of faces.
bool checkFaces(const vector<Face> &f1, const vector<Face> &f2, vector<int> &p) {
	if (f1.size() != f2.size())
		return false;
	
	for (const auto &f : f1) {
		bool found = false;
		
		for (const auto &g : f2) {
			if (g.equals(Face({p[f.t[0]], p[f.t[1]], p[f.t[2]]}))) {
				found = true;
				break;
			}
		}
		
		if (!found)
			return false;
	}
	
	return true;
}

// Checks isomorphism of the given graphs.
// Iterates over all possible vertices bijections to find an isomorphism.
// This is slow for large graphs and thus the function is used
// only if all the heuristics fail to give an answer.
bool checkPermutations(const Graph &g1, const Graph &g2, const vector<Face> &f1, const vector<Face> &f2, int n, vector<int> &p, vector<bool> &used) {
	if ((int)p.size() == n) {
		bool ans = checkFaces(f1, f2, p);
		return ans;
	}
	
	int id = p.size();
	
	for (int a=0; a<n; a++) {
		if (used[a] || g1[id].count() != g2[a].count())
			continue;
		
		bool ok = true;
		for (int i=0; i<(int)p.size(); i++) {
			if (g1[id][i] != g2[a][p[i]]) {
				ok = false;
				break;
			}
		}
		if (!ok)
			continue;
		
		used[a] = true;
		p.push_back(a);
		if (checkPermutations(g1, g2, f1, f2, n, p, used))
			return true;
		p.pop_back();
		used[a] = false;
	}
	
	return false;
}

// Checks isomorphism of the given graphs.
// Firstly tries simple heuristics, and in case of failure searches
// over all the possible permutations.
bool isSimpleEqual(const Graph &g1, const Graph &g2) {
	vector<int> p;
	if (g1.size() != g2.size())
		return false;
	if (graphHash(g1) != graphHash(g2))
		return false;
	
	int n = g1.size();
	
	for (int i=0; i<n; i++)
		p.push_back(i);
	
	vector<int> permutation;
	vector<bool> used(n, false);
	return checkPermutations(g1, g2, {}, {}, n, permutation, used);
}

// Checks isomorphism of the given graphs.
// Firstly tries simple heuristics, and in case of failure searches
// over all the possible permutations.
bool isFaceEqual(const Graph &g1, const vector<Face> &f1, const Graph &g2, const vector<Face> &f2) {
	vector<int> p;
	if (g1.size() != g2.size())
		return false;
	if (graphHash(g1) != graphHash(g2))
		return false;
	
	int n = g1.size();
	
	for (int i=0; i<n; i++)
		p.push_back(i);
	
	vector<int> permutation;
	vector<bool> used(n, false);
	return checkPermutations(g1, g2, f1, f2, n, permutation, used);
}

// Computes faces of the given square map.
vector<Face> getFaces(const Graph &g) {
	vector<Face> res, output;
	
	// There are always exactly 12 outline vertices.
	for (int i=0; i<12; i++) {
		auto common = g[i]&g[(i+1)%12];
		for (int j=common._Find_first(); j<(int)common.size(); j = common._Find_next(j)) {
			res.push_back(Face({i, (i+1)%12, j}));
		}
	}
	assert(res.size() == 12);
	
	for (int i=12; i<(int)g.size(); i++) {
		for (int j=0; j<i; j++) {
			if (!g[i][j])
				continue;
			auto common = g[i]&g[j];
			int cnt = 0;
			for (int k=common._Find_first(); k<(int)common.size(); k = common._Find_next(k)) {
				cnt++;
				res.push_back(Face({i, j, k}));
			}
			assert(cnt == 2);
		}
	}
	
	for (int i=0; i<(int)res.size(); i++) {
		bool duplicate = false;
		for (int j=0; j<i; j++) {
			if (res[i].equals(res[j])) {
				duplicate = true;
				break;
			}
		}
		if (!duplicate)
			output.push_back(res[i]);
	}
	assert(output.size() == 2*(g.size()-7));
	
	return output;
}

// Checks whether the given square maps represent isomorphic toroidal graphs.
bool isSquareMapEqual(const Graph &g1, const Graph &g2) {
	auto t1 = glueTorus(g1);
	auto t2 = glueTorus(g2);
	
	auto f1 = getFaces(g1);
	auto f2 = getFaces(g2);
	
	glueFaces(f1);
	glueFaces(f2);
	
	return isFaceEqual(t1, f1, t2, f2);
}

// Checks whether the given graph is connected.
// This is done by verifying whether all the vertices can be reached
// from any starting one.
bool isConnected(const Graph &g) {
	Vertex x;
	x[0] = 1;
	int n = g.size();
	
	for (int i=0; i<n; i++) {
		for (int a=0; a<n; a++) {
			if (x[a])
				x = x | g[a];
		}
	}
	
	return ((int)x.count() == n);
}

// Verifies whether the given graph is triangulated.
bool isTriangulated(const Graph &g) {
	int n = g.size();
	
	for (int mas=0; mas<(1<<n); mas++) {
		int cycleSize = __builtin_popcount(mas);
		
		if (cycleSize < 3)
			continue;
		
		vector<int> v;
		Vertex x;
		for (int i=0; i<n; i++)
			if (mas&(1<<i)) {
				v.push_back(i);
				x[i] = 1;
			}
		
		bool cycle = 1;
		int edges = 0;
		
		for (int i=0; i<(int)v.size(); i++) {
			if (!g[v[i]][v[(i+1)%cycleSize]]) {
				cycle = 0;
				break;
			}
			
			edges += (x&g[v[i]]).count();
		}
		
		if (!cycle)
			continue;
		
		assert(edges%2 == 0);
		
		if (edges/2 != 2*cycleSize-3)
			return false;
	}
	
	return true;
}
