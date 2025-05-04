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

#include "dijkstra.h"

enum EdgeType {
    INCOMING,
    OUTGOING
}; 

struct Edge {
    int source_idx;
    int target_idx;
    int cost;

    friend std::ostream& operator<<(std::ostream& os, const Edge& e);
    friend bool operator<(const Edge& edge_1, const Edge& edge_2);
};

struct Shortcut {
    int id;
    int first_edge_id;
    int second_edge_id;
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

    void permute_graph();
    void build_graph_representation();

    std::vector<int> find_independent_set(std::vector<bool> &contracted);
    std::vector<int> partition_independent_set(int num_partitions, std::vector<int> &independent_set, std::vector<int> &edge_differences, std::vector<bool> &contracted);
    
    std::vector<int> permute_graph(std::vector<Edge> &edges, std::vector<Node> &node_levels);

public:
    int num_nodes;
    int num_edges;
    
    Graph(const std::string &file_path);
    
    EdgeRange<EdgeType::INCOMING> get_incoming_edges(int node_idx);
    EdgeRange<EdgeType::OUTGOING> get_outgoing_edges(int node_idx);

    int level_of(int node_idx);
    int get_node_index(int node_id);
    int get_node_id(int node_idx);

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

    int size() {
        return end_idx - begin_idx;
    }

    inline EdgeRange(const Graph &graph, const int node_idx) : graph(graph) {
        if constexpr (edge_type == EdgeType::INCOMING) {
            begin_idx = graph.in_offsets[node_idx];
            end_idx = (node_idx == graph.num_nodes - 1) ? graph.num_edges : graph.in_offsets[node_idx + 1];
        } else {
            begin_idx = graph.out_offsets[node_idx];
            end_idx = (node_idx == graph.num_nodes - 1) ? graph.num_edges : graph.out_offsets[node_idx + 1];
        }
    }
};