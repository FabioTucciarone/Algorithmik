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

struct DistancePair {
    int forward;
    int backward;

    DistancePair(int s, int t);
};

struct DNode {
    int idx;
    DistancePair distance;
    bool is_forward_search;

    DNode(int idx, int s_dist, int t_dist, bool is_forward_search);
    friend bool operator<(const DNode& a, const DNode& b);
};

class Dijkstra {
    // Djikstra: https://www2.informatik.uni-stuttgart.de/bibliothek/ftp/medoc.ustuttgart_fi/BCLR-0114/BCLR-0114.pdf

    using clock = std::chrono::steady_clock;

    std::vector<int> touched_nodes;
    std::vector<DistancePair> distances;
    std::priority_queue<DNode> queue;
    Graph &graph;
    int last_start;
    
    bool should_stall_forward(int node_idx);
    bool should_stall_backward(int node_idx);

public:
    Dijkstra(Graph &graph);
    std::pair<int, int64_t> query(int start, int target);
};