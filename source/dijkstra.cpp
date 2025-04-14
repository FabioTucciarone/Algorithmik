#include "dijkstra.h"

Dijkstra::Dijkstra(Graph &graph) : graph(graph), last_start(-1) {
    distances.resize(graph.num_nodes, std::numeric_limits<float>::infinity());
}

std::pair<int, int64_t> Dijkstra::query(int start, int target) {

    if (start >= graph.num_nodes && target >= graph.num_nodes) {
        return {std::numeric_limits<float>::infinity(), 0};
    }

    clock::time_point start_time = clock::now();

    queue = std::priority_queue<DNode>();
    if(last_start != start) {
        for (int node : touched_nodes) {
            distances[node] = std::numeric_limits<float>::infinity();
        }
        last_start = start;
    }

    queue.emplace(start, 0.0f);
    distances[start] = 0.0f;
    
    for (int i = 0; i < graph.num_nodes; i++) {         
        DNode node = queue.top();
        queue.pop();
        if (node.dist == distances[node.idx]) {
            IndexRange range = graph.get_out_range(node.idx);
            for (int edge_index = range.begin; edge_index < range.end; edge_index++) {
                Edge edge = graph.edges[edge_index];
                if (distances[edge.target] > distances[node.idx] + edge.cost) {
                    distances[edge.target] = distances[node.idx] + edge.cost;
                    touched_nodes.push_back(edge.target);
                    queue.emplace(edge.target, distances[edge.target]);
                }
            }
        }
    }

    clock::time_point end_time = clock::now();
    int64_t duration = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time).count();

    return {distances[target], duration};
}


bool operator<(const DNode& a, const DNode& b) {
    return a.dist > b.dist;
}