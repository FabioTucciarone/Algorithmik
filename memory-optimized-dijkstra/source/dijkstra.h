#pragma once

#include <iostream>
#include <fstream>
#include <algorithm>
#include <string>
#include <vector>
#include <limits>
#include <charconv>
#include <array>
#include <numeric>
#include <algorithm>
#include <queue>
#include <tuple>
#include <chrono>

#include "graph.h"

struct SearchTriple {
    int idx;
    int dist;

    SearchTriple(int idx, int dist);
    friend bool operator<(const SearchTriple& a, const SearchTriple& b);
};


class Dijkstra {
    // Djikstra: https://www2.informatik.uni-stuttgart.de/bibliothek/ftp/medoc.ustuttgart_fi/BCLR-0114/BCLR-0114.pdf

    using clock = std::chrono::steady_clock;

    std::vector<int> touched_nodes;
    std::vector<int> distances;
    std::priority_queue<SearchTriple> queue;
    Graph &graph;
    int last_start;
    
public:
    Dijkstra(Graph &graph);
    std::pair<int, int64_t> query(int start, int target);
};