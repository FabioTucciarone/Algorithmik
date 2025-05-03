#include "dijkstra.h"

DNode::DNode(int idx, int s_dist, int t_dist, bool is_forward_search) : idx(idx), distance({s_dist, t_dist}), is_forward_search(is_forward_search) {}

DistancePair::DistancePair(int s, int t) : forward(s), backward(t) {};


bool settled(const DNode &node) {
    return node.distance.forward < std::numeric_limits<int>::max() && node.distance.backward < std::numeric_limits<int>::max();
}

// a > b

bool operator<(const DNode& a, const DNode& b) {

    bool a_setteled = settled(a);
    bool b_setteled = settled(b);

    if (a_setteled && b_setteled) {
        return a.distance.forward + a.distance.backward > b.distance.forward + b.distance.backward;
    } 
    else if (!a_setteled && b_setteled) {
        return  false;
    }
    else if (a_setteled && !b_setteled) {
        return true;
    }
    else {
        const int a_d = a.is_forward_search ? a.distance.forward : a.distance.backward;
        const int b_d = b.is_forward_search ? b.distance.forward : b.distance.backward;
        return a_d > b_d;
    }
}

Dijkstra::Dijkstra(Graph &graph) : graph(graph), last_start(-1) {
    const int max = std::numeric_limits<int>::max();
    distances.resize(graph.num_nodes, DistancePair(max, max));
}


bool Dijkstra::should_stall_forward(int node_idx) {
    for (Edge edge : graph.get_incoming_edges(node_idx)) {
        if (graph.get_level(edge.source_idx) > graph.get_level(node_idx)) {
            if (distances[edge.source_idx].forward < std::numeric_limits<int>::max() && distances[node_idx].forward > distances[edge.source_idx].forward + edge.cost) {
                return true;
            }
                        
        }
    }
    return false;
}

bool Dijkstra::should_stall_backward(int node_idx) {
    for (Edge edge : graph.get_outgoing_edges(node_idx)) {
        if (graph.get_level(edge.target_idx) > graph.get_level(node_idx)) {
            if (distances[edge.target_idx].backward < std::numeric_limits<int>::max() && distances[node_idx].backward > distances[edge.target_idx].backward + edge.cost) {
                return true;
            }
        }
    }
    return false;
}


std::pair<int, int64_t> Dijkstra::query(int start_id, int target_id) {

    int start = graph.get_node_index(start_id);
    int target = graph.get_node_index(target_id);

    if (start >= graph.num_nodes || target >= graph.num_nodes) {
        return { std::numeric_limits<int>::max(), 0 };
    }

    clock::time_point start_time = clock::now();

    if(true) { // Nachdenken
        for (int node : touched_nodes) {
            distances[node].forward = std::numeric_limits<int>::max();
            distances[node].backward = std::numeric_limits<int>::max();
        }
        touched_nodes.clear();
        queue = std::priority_queue<DNode>();
        queue.emplace(start, 0, std::numeric_limits<int>::max(), true);
        queue.emplace(target, std::numeric_limits<int>::max(), 0, false);
        touched_nodes.push_back(start);
        touched_nodes.push_back(target);
        distances[start].forward = 0;
        distances[target].backward = 0;
        last_start = start;
    }

    int shortest_path = std::numeric_limits<int>::max();

    while(!queue.empty()) {         
        DNode node = queue.top();
        queue.pop();

        if (settled(node)) {
            if (shortest_path >= node.distance.forward + node.distance.backward) {
                shortest_path = node.distance.forward + node.distance.backward;
            } else {
                break; // FERTIG
            }
        }

        if (node.is_forward_search) {
            if (node.distance.forward == distances[node.idx].forward) {
                
                if (should_stall_forward(node.idx)) continue;

                for (const Edge &edge : graph.get_outgoing_edges(node.idx)) {
                    if (graph.get_level(edge.target_idx) < graph.get_level(edge.source_idx)) {
                        break;
                    }
                    const int alternative_distance = distances[node.idx].forward + edge.cost;
                    if (distances[edge.target_idx].forward > alternative_distance) {
                        distances[edge.target_idx].forward = alternative_distance;
                        touched_nodes.push_back(edge.target_idx);
                        queue.emplace(edge.target_idx, distances[edge.target_idx].forward, distances[edge.target_idx].backward, true);
                    }
                }
            }
        } else {
            if (node.distance.backward == distances[node.idx].backward) {

                if (should_stall_backward(node.idx)) continue;

                for (const Edge &edge : graph.get_incoming_edges(node.idx)) {
                    if (graph.get_level(edge.source_idx) < graph.get_level(edge.target_idx)) {
                        break;
                    }
                    const int alternative_distance = distances[node.idx].backward + edge.cost;
                    if (distances[edge.source_idx].backward > alternative_distance) {
                        distances[edge.source_idx].backward = alternative_distance;
                        touched_nodes.push_back(edge.source_idx);
                        queue.emplace(edge.source_idx, distances[edge.source_idx].forward, distances[edge.source_idx].backward, false);
                    }
                }
            }
        }   
    }

    clock::time_point end_time = clock::now();
    int64_t duration = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time).count();

    return { shortest_path, duration };
}