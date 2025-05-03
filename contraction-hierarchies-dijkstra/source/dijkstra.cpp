#include "dijkstra.h"

DNode::DNode(int id, int s_dist, int t_dist, bool is_forward_search) : id(id), distance({s_dist, t_dist}), is_forward_search(is_forward_search) {}

DistancePair::DistancePair(int s, int t) : s(s), t(t) {};


bool settled(const DNode &node) {
    return node.distance.s < std::numeric_limits<int>::max() && node.distance.t < std::numeric_limits<int>::max();
}

// a > b

bool operator<(const DNode& a, const DNode& b) {

    bool a_setteled = settled(a);
    bool b_setteled = settled(b);

    if (a_setteled && b_setteled) {
        return a.distance.s + a.distance.t > b.distance.s + b.distance.t;
    } 
    else if (!a_setteled && b_setteled) {
        return  false;
    }
    else if (a_setteled && !b_setteled) {
        return true;
    }
    else {
        const int a_d = a.is_forward_search ? a.distance.s : a.distance.t;
        const int b_d = b.is_forward_search ? b.distance.s : b.distance.t;
        return a_d > b_d;
    }
}

Dijkstra::Dijkstra(Graph &graph) : graph(graph), last_start(-1) {
    const int max = std::numeric_limits<int>::max();
    distances.resize(graph.num_nodes, DistancePair(max, max));
}

std::pair<int, int64_t> Dijkstra::query(int start, int target) {

    if (start >= graph.num_nodes || target >= graph.num_nodes) {
        return { std::numeric_limits<int>::max(), 0 };
    }

    clock::time_point start_time = clock::now();

    if(true) { // Nachdenken
        for (int node : touched_nodes) {
            distances[node].s = std::numeric_limits<int>::max();
            distances[node].t = std::numeric_limits<int>::max();
        }
        touched_nodes.clear();
        queue = std::priority_queue<DNode>();
        queue.emplace(start, 0, std::numeric_limits<int>::max(), true);
        queue.emplace(target, std::numeric_limits<int>::max(), 0, false);
        touched_nodes.push_back(start);
        touched_nodes.push_back(target);
        distances[start].s = 0;
        distances[target].t = 0;
        last_start = start;
    }

    int shortest_path = std::numeric_limits<int>::max();

    for (int i = 0; i < graph.num_nodes; i++) {         
        DNode node = queue.top();
        queue.pop();

        // std::cout << "pop: " << node.id << " (" << node.distance.s << "," << node.distance.t << "," << (node.is_forward_search ? "out" : "in") << ")";

        if (settled(node)) {
            if (shortest_path >= node.distance.s + node.distance.t) {
                shortest_path = node.distance.s + node.distance.t;
                // std::cout << " -> settled";
            } else {
                // std::cout << "\n";
                break; // FERTIG
            }
        }
        // std::cout << "\n";

        if (node.is_forward_search) {
            if (node.distance.s == distances[node.id].s) {
                for (Edge edge : graph.get_outgoing_edges(node.id)) {
                    // std::cout << "  > " << edge << ", level: " << graph.get_level(edge.source_id) << " " << graph.get_level(edge.target_id) << "\n";
                    if (graph.get_level(edge.target_id) < graph.get_level(edge.source_id)) {
                        break;
                    }
                    if (distances[edge.target_id].s > distances[node.id].s + edge.cost) {
                        // std::cout << "    d(" << edge.target_id << ") = d(" << node.id << ") + " << edge.cost << " = " << distances[node.id].s + edge.cost << "\n";
                        distances[edge.target_id].s = distances[node.id].s + edge.cost;
                        touched_nodes.push_back(edge.target_id);
                        // std::cout << "    push: " << edge.target_id << "(" <<  distances[edge.target_id].s << "," << distances[edge.target_id].t << ",out)\n";
                        queue.emplace(edge.target_id, distances[edge.target_id].s, distances[edge.target_id].t, true);
                    }
                }
            }
        } else {
            if (node.distance.t == distances[node.id].t) {
                for (Edge edge : graph.get_incoming_edges(node.id)) {
                    // std::cout << "  > " << edge << ", level: " << graph.get_level(edge.source_id) << " " << graph.get_level(edge.target_id) << "\n";
                    if (graph.get_level(edge.source_id) < graph.get_level(edge.target_id)) {
                        break;
                    }
                    if (distances[edge.source_id].t > distances[node.id].t + edge.cost) {
                        // std::cout << "    d(" << edge.source_id << ") = d(" << node.id << ") + " << edge.cost << " = " << distances[node.id].t + edge.cost << "\n";
                        distances[edge.source_id].t = distances[node.id].t + edge.cost;
                        touched_nodes.push_back(edge.source_id);
                        // std::cout << "    push: " << edge.source_id << "(" <<  distances[edge.target_id].s << "," << distances[edge.source_id].t << ",in)\n";
                        queue.emplace(edge.source_id, distances[edge.source_id].s, distances[edge.source_id].t, false);
                    }
                }
            }
        }   
    }

    clock::time_point end_time = clock::now();
    int64_t duration = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time).count();

    return { shortest_path, duration };
}