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

struct Edge {
    int source;
    int target;
    int cost;

    friend std::ostream& operator<<(std::ostream& os, const Edge& dt);
};

struct IndexRange {
    int begin;
    int end;
    IndexRange(int begin, int end);
};

class Graph {
public:
    using clock = std::chrono::steady_clock;

    std::vector<int> out_offsets;
    std::vector<int> in_offsets;
    std::vector<int> target_ordering;
    std::vector<Edge> edges;
    int num_nodes;
    int num_edges;
    
    Graph(const std::string &file_path);
    
    template<bool in_list>
    void generate_offset_list(std::vector<int> &offsets);
    
    IndexRange get_out_range(int node);
    IndexRange get_in_range(int node);

    int get_out_node(int edge_index);
    int get_in_node(int edge_index);
};
