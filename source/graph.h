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

    friend std::ostream& operator<<(std::ostream& os, const Edge& dt);
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

    template<bool in_list>
    void generate_offset_list(std::vector<int> &offsets);

public:
    int num_nodes;
    int num_edges;
    
    Graph(const std::string &file_path);
    
    EdgeRange<EdgeType::INCOMING> get_incoming_edges(int node);
    EdgeRange<EdgeType::OUTGOING> get_outgoing_edges(int node);

    friend EdgeRange<EdgeType::INCOMING>;
    friend EdgeRange<EdgeType::OUTGOING>;
};

template<EdgeType edge_type>
class EdgeRange {
    Graph &graph;
    int begin_idx;
    int end_idx;

public:
    class Iterator {
        Graph &graph;
        int i;

    public:
        inline Iterator(Graph &graph, int i) : graph(graph), i(i) {}

        inline Edge &operator*() const { 
            if constexpr (edge_type == EdgeType::INCOMING) {
                return graph.edges[graph.target_ordering[i]];
            } else {
                return graph.edges[i]; 
            }
        }

        inline Iterator& operator++() { i++; return *this; }  
        inline bool operator== (const Iterator& b) { return i == b.i; };
        inline bool operator!= (const Iterator& b) { return i != b.i; };
    };

    inline Iterator begin() { return Iterator(graph, begin_idx); }
    inline Iterator end() { return Iterator(graph, end_idx); }

    inline EdgeRange(Graph &graph, int node) : graph(graph) {
        if constexpr (edge_type == EdgeType::INCOMING) {
            begin_idx = graph.in_offsets[node];
            end_idx = (node == graph.num_nodes - 1) ? graph.num_edges : graph.in_offsets[node + 1];
        } else {
            begin_idx = graph.out_offsets[node];
            end_idx = (node == graph.num_nodes - 1) ? graph.num_edges : graph.out_offsets[node + 1];
        }
    }
};