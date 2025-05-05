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
#include <omp.h>

#include "util.h"
#include "graph.h"

class Graph;

struct SearchTriple {
    int idx;
    int out_distance;
    int in_distance;
    bool is_forward_search;

    SearchTriple(int idx, int s_dist, int t_dist, bool is_forward_search);
    friend bool operator<(const SearchTriple& a, const SearchTriple& b);
};

struct SearchTuple {
    int idx;
    int distance;

    SearchTuple(int idx, int distance);
    friend bool operator<(const SearchTuple& a, const SearchTuple& b);
};

class Dijkstra {
    // Djikstra: https://www2.informatik.uni-stuttgart.de/bibliothek/ftp/medoc.ustuttgart_fi/BCLR-0114/BCLR-0114.pdf

    using clock = std::chrono::steady_clock;

    std::vector<int> touched_nodes;
    std::vector<int> out_distances;
    std::vector<int> in_distances;
    std::priority_queue<SearchTriple> bidirectional_queue;
    std::priority_queue<SearchTuple> unidirectional_queue;
    Graph &graph;
    int last_start;
    
    bool should_stall_forward(int node_idx);
    bool should_stall_backward(int node_idx);

public:
    Dijkstra(Graph &graph);
    std::pair<int, int64_t> query(int start_id, int target_id);

    int get_shortest_contraction_distance(int source_idx, int target_idx, std::vector<bool> &contracted);
};