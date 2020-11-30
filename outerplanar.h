#ifndef __OUTERPLANAR_H
#define __OUTERPLANAR_H

#include "graph.h"
#include <bits/stdc++.h>


const int MAXN = 8;


extern std::vector<Graph> outerplanars[MAXN+10];

std::vector<int> border(const Graph &g);
void getOuterplanars();

#endif /* __OUTERPLANAR_H */
