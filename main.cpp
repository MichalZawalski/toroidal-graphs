#include "graph.h"
#include "outerplanar.h"

#include <bits/stdc++.h>
using namespace std;


// Number of outline vertices.
const int OUTSIZE = 12;


// Prints the current running time value.
void printTime(int begin=0) {
	printf("Current running time: %d s\n", (int)(time(0) - begin));
}

// Recursive function for finding a matching between two sets.
void matchingRecurse(int n1, int n2, vector<int> &matching, vector<vector<int>> &out, int node) {
	int n = matching.size();
	if (n1-node > n2-n)
		return;
	
	if (node == n1-1) {
		for (int i=n; i<n2; i++)
			matching.push_back(node);
		out.push_back(matching);
		for (int i=n; i<n2; i++)
			matching.pop_back();
		return;
	}
	
	for (int i=1; i<=n2-n; i++) {
		if (i != 0)
			matching.push_back(node);
		matchingRecurse(n1, n2, matching, out, node+1);
	}
	
	matching.resize(n);
}

// Finds all the matchings between two sets of given cardinalities.
void findMatchings(int n1, int n2, vector<vector<int>> &out) {
	// from n1 to n2, i.e. n2 times elements of n1, at least once
	vector<int> tmp;
	matchingRecurse(n1, n2, tmp, out, 0);
	assert(tmp.empty());
}

// Checks whether the given masks are equivalent by cyclic rotation.
bool simpleCyclicEquiv(int mask1, int mask2) {
	int cor[4];
	for (int i=0; i<4; i++)
		cor[i] = (mask1&(15<<(4*i)))>>(4*i);
	
	for (int i=0; i<4; i++) {
		int rotation = 0;
		for (int j=0; j<4; j++)
			rotation = (rotation<<4) + cor[(i+3-j)%4];
		
		if (rotation == mask2)
			return true;
	}
	
	return false;
}

// Swaps the given bits in the given mask.
int swapBits(int p1, int p2, int mask) {
	bool b1 = (mask&(1<<p1)) != 0;
	bool b2 = (mask&(1<<p2)) != 0;
	
	if (b1 != b2) {
		mask ^= (1<<p1);
		mask ^= (1<<p2);
	}
	
	return mask;
}

// Checks whether the given masks are isomorphic.
bool outlineMaskEquiv(int mask1, int mask2) {
	if (simpleCyclicEquiv(mask1, mask2))
		return true;
	
	// symmetry
	mask1 = swapBits(1, 2,  mask1);
	mask1 = swapBits(4, 12,  mask1);
	mask1 = swapBits(5, 14,  mask1);
	mask1 = swapBits(6, 13,  mask1);
	mask1 = swapBits(7, 15,  mask1);
	mask1 = swapBits(9, 10,  mask1);
	
	return simpleCyclicEquiv(mask1, mask2);
}

// Finds the border vertices of outline.
// These are the vertices that can be matched with the inside graph.
vector<int> outlineBorder(const Graph &outline) {
	int beg = 1;
	if (outline[2][11] || outline[2][10])
		beg = 2;
	vector<int> res;
	int x = beg;
	
	do {
		res.push_back(x);
		for (int i=4; i>=0; i--) {
			assert(i != 0);
			
			if (outline[x][(x+i)%OUTSIZE]) {
				x = (x+i)%OUTSIZE;
				break;
			}
		}
	} while (x != beg);
	
	return res;
}

// Computes the border outlines.
// Stores the outlines in form of bitmasks.
// 16 bits for every possible border edge.
vector<Graph> getOutlines() {
	vector<int> outlines_masks;
	vector<Graph> outlines;
	
	const int NUM_EDGES = 16;
	
	for (int mas=0; mas<(1<<NUM_EDGES); mas++) {
		bool wrong = false;
		
		for (int c=0; c<4; c++) {
			bool e0 = (mas&(1<<(4*c))) != 0;
			bool e1 = (mas&(1<<(4*c+1))) != 0;
			bool e2 = (mas&(1<<(4*c+2))) != 0;
			bool e3 = (mas&(1<<(4*c+3))) != 0;
			
			bool f1 = (mas&(1<<((4*c+4)%NUM_EDGES))) != 0;
			bool f2 = (mas&(1<<((4*c+8)%NUM_EDGES))) != 0;
			bool f3 = (mas&(1<<((4*c+12)%NUM_EDGES))) != 0;
			
			// correctness
			if (e1 && e2)
				wrong = true;
			if (e1 && f3)
				wrong = true;
			if (e1 && !e0)
				wrong = true;
			
			if (e2 && f1)
				wrong = true;
			if (e2 && !e0)
				wrong = true;
			
			if (e3 && f2)
				wrong = true;
			if (e3 && !e1 && !e2)
				wrong = true;
			if (e3 && !e0)
				wrong = true;
		}
		
		if (wrong)
			continue;
		
		for (int m : outlines_masks)
			if (outlineMaskEquiv(mas, m)) {
				wrong = true;
				break;
			}
		
		if (wrong)
			continue;
		
		outlines_masks.push_back(mas);
		
		auto g = emptyGraph(OUTSIZE);
		for (int i=0; i<OUTSIZE; i++)
			putEdge(i, (i+1)%OUTSIZE, g);
		
		for (int c=0; c<4; c++) {
			bool e0 = (mas&(1<<(4*c))) != 0;
			bool e1 = (mas&(1<<(4*c+1))) != 0;
			bool e2 = (mas&(1<<(4*c+2))) != 0;
			bool e3 = (mas&(1<<(4*c+3))) != 0;
			
			if (e0)
				putEdge((3*c+1)%OUTSIZE, (3*c+11)%OUTSIZE, g);
			if (e1)
				putEdge((3*c+1)%OUTSIZE, (3*c+10)%OUTSIZE, g);
			if (e2)
				putEdge((3*c+2)%OUTSIZE, (3*c+11)%OUTSIZE, g);
			if (e3)
				putEdge((3*c+2)%OUTSIZE, (3*c+10)%OUTSIZE, g);
		}
		
		outlines.push_back(g);
	}
	
	printf("outlines: %d\n\n", (int)outlines_masks.size());
	return outlines;
}

// Identifier of the vertex after glueing the corresponding border pairs.
int gluedVertex(int a) {
	if (a >= OUTSIZE)
		return a;
	if (a%3 == 0)
		return 0;
	
	if (a == 8)
		return 1;
	if (a == 7)
		return 2;
	if (a == 11)
		return 4;
	if (a == 10)
		return 5;
	return a;
	
}

// List of forbidden edges.
// These are edges between corresponding vertices on the border,
// which cannot be present in a valid graph.
const vector<pair<int, int>> forbidden = {{0, 3}, {0, 6}, {0, 9}, {3, 6}, {3, 9}, {6, 9}, {1, 8}, {2, 7}, {4, 11}, {5, 10}};

// Checks whether the given graph is a valid triangulated toroidal graph.
bool validateGraph(const Graph &graph) {
	auto torus = glueTorus(graph);
	
	int n = 0, e = 0;
	for (int i=0; i<(int)torus.size(); i++) {
		if (torus[i].any()) {
			n++;
			e += torus[i].count();
			
			// Degrees are at least 3.
			if (torus[i].count() < 3)
				return false;
		}
	}
	
	e /= 2;
	// Euler characteristic is equal to 0.
	if (3*n != e)
		return false;
	
	// No 2-cycles.
	for (int i=0; i<(int)graph.size(); i++) {
		for (auto f : forbidden) {
			if (graph[i][f.first] && graph[i][f.second])
				return false;
		}
	}
	
	// Noncontractibility of all edges
	for (int i=0; i<(int)graph.size(); i++) {
		for (int j=0; j<i; j++) {
			if (i < OUTSIZE)
				if (j == (i+1)%OUTSIZE || i == (j+1)%OUTSIZE)
					continue;
			if (!graph[i][j])
				continue;
			if ((graph[i]&graph[j]).count() != 2)
				return false;
			if ((torus[gluedVertex(i)]&torus[gluedVertex(j)]).count() < 3)
				return false;
		}
	}
	
	return true;
}

// Combines the given inside graph with all the possible matchings with the border.
void combineInsideOutline(const Graph &inside, const Graph &outline, vector<Graph> &out, vector<Graph> &replay_buffer) {
	auto inside_border = border(inside);
	auto outline_border = outlineBorder(outline);
	
	vector<vector<int>> matchings;
	findMatchings(outline_border.size(), outline_border.size()+inside_border.size(), matchings);
	
	auto graph = outline;
	graph.resize(outline.size() + inside.size());
	int shift = outline.size();
	
	for (int i=0; i<(int)inside.size(); i++)
		for (int j=0; j<(int)inside.size(); j++)
			graph[shift+i][shift+j] = inside[i][j];
	
	for (const auto &matching : matchings) {
		for (int beg=0; beg<(int)inside_border.size(); beg++) {
			auto g = graph;
			
			int prev = -1;
			int match = beg;
			
			for (int node : matching) {
				if (node == prev)
					match = (match+1)%inside_border.size();
				prev = node;
				putEdge(outline_border[node], shift+inside_border[match], g);
			}
			
			if (!validateGraph(g))
				continue;
			
			bool duplicate = false;
			auto torus = glueTorus(g);
			for (const auto &x : replay_buffer) {
				if (isSquareMapEqual(x, g)) {
					duplicate = true;
					break;
				}
			}
			
			if (!duplicate) {
				printf("torus nb. %d\n", 1+(int)out.size());
				printGraph(g);
				out.push_back(g);
				replay_buffer.push_back(g);
			}
		}
	}
}

// Main function.
// Combines all the elements of the pipeline.
// Firstly, creates the list of all the outerplanar graphs of relevant sizes.
// Then combines all of them with all the possible matchings with the border outlines.
void combineAll() {
	int begin_time = time(0);
	
	getOuterplanars();
	vector<Graph> outlines = getOutlines();
	vector<Graph> out, replay_buffer;
	
	printTime(begin_time);
	
	for (int inside_size=2; inside_size<=MAXN; inside_size++) {
		printf("Current inside size: %d\n", inside_size);
		for (const auto &outline : outlines) {
			for (const auto &inside : outerplanars[inside_size]) {
				combineInsideOutline(inside, outline, out, replay_buffer);
			}
		}
		printTime(begin_time);
	}
}

int main() {
	combineAll();
	return 0;
}
