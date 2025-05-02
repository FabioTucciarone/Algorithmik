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

enum EdgeType {
    INCOMING,
    OUTGOING
}; 

struct Edge {
    int source;
    int target;
    int cost;

    friend std::ostream& operator<<(std::ostream& os, const Edge& e);
};

struct Node {
    int id;
    int level;

    friend std::ostream& operator<<(std::ostream& os, const Node& n);
};

struct IndexRange {
    int begin;
    int end;
    IndexRange(int begin, int end);
};


template<EdgeType edge_type>
struct EdgeRange;

class Graph {
    using clock = std::chrono::steady_clock;

    std::vector<int> out_offsets;
    std::vector<int> in_offsets;
    std::vector<int> target_ordering;
    std::vector<Edge> edges;
    std::vector<Node> nodes; 
    std::vector<int> node_ordering; 

    template<EdgeType edge_type>
    void generate_offset_list(std::vector<int> &offsets);

    void read_from_ch_file(const std::string &file_path);
    void read_from_graph_file(const std::string &file_path);

public:
    int num_nodes;
    int num_edges;
    
    Graph(const std::string &file_path);
    
    EdgeRange<EdgeType::INCOMING> get_incoming_edges(int node_id);
    EdgeRange<EdgeType::OUTGOING> get_outgoing_edges(int node_id);

    friend EdgeRange<EdgeType::INCOMING>;
    friend EdgeRange<EdgeType::OUTGOING>;
};

template<EdgeType edge_type>
class EdgeRange {
    const Graph &graph;
    int begin_idx;
    int end_idx;

public:
    class Iterator { // TODO Warum for (Edge &e : get...) geht nicht !!
        const Graph &graph;
        int i;

    public:
        inline Iterator(const Graph &graph, int i) : graph(graph), i(i) {}

        inline const Edge &operator*() const { 
            if constexpr (edge_type == EdgeType::INCOMING) {
                return graph.edges[graph.target_ordering[i]];
            } else {
                return graph.edges[i]; 
            }
        }

        inline Iterator &operator++() { i++; return *this; }  
        inline bool operator== (const Iterator& b) const { return i == b.i; };
        inline bool operator!= (const Iterator& b) const { return i != b.i; };
    };

    inline Iterator begin() { return Iterator(graph, begin_idx); }
    inline Iterator end() { return Iterator(graph, end_idx); }

    inline EdgeRange(const Graph &graph, const int node_id) : graph(graph) {
        int node_idx = graph.node_ordering[node_id];
        if constexpr (edge_type == EdgeType::INCOMING) {
            begin_idx = graph.in_offsets[node_idx];
            end_idx = (node_idx == graph.num_nodes - 1) ? graph.num_edges : graph.in_offsets[node_idx + 1];
        } else {
            begin_idx = graph.out_offsets[node_idx];
            end_idx = (node_idx == graph.num_nodes - 1) ? graph.num_edges : graph.out_offsets[node_idx + 1];
        }
    }
};