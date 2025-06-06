#include "dijkstra.h"

SearchTriple::SearchTriple(int idx, int s_dist, int t_dist, bool is_forward_search) : idx(idx), out_distance(s_dist), in_distance(t_dist), is_forward_search(is_forward_search) {}
SearchTuple::SearchTuple(int idx, int distance) : idx(idx), distance(distance) {}


bool settled(const SearchTriple &node) {
    return node.out_distance < std::numeric_limits<int>::max() && node.in_distance < std::numeric_limits<int>::max();
}

// a > b

bool operator<(const SearchTriple& a, const SearchTriple& b) {

    bool a_setteled = settled(a);
    bool b_setteled = settled(b);

    if (a_setteled && b_setteled) {
        return a.out_distance + a.in_distance > b.out_distance + b.in_distance;
    } 
    else if (!a_setteled && b_setteled) {
        return  false;
    }
    else if (a_setteled && !b_setteled) {
        return true;
    }
    else {
        const int a_d = a.is_forward_search ? a.out_distance : a.in_distance;
        const int b_d = b.is_forward_search ? b.out_distance : b.in_distance;
        return a_d > b_d;
    }
}

bool operator<(const SearchTuple& a, const SearchTuple& b) {
    return a.distance > b.distance;
}

Dijkstra::Dijkstra(Graph &graph) : graph(graph), last_start(-1) {
    const int max = std::numeric_limits<int>::max();
    out_distances.resize(graph.num_nodes, max);
    in_distances.resize(graph.num_nodes, max);
}


bool Dijkstra::should_stall_forward(int node_idx) {
    for (Edge edge : graph.get_incoming_edges(node_idx)) {
        if (graph.level_of(edge.source_idx) > graph.level_of(node_idx)) {
            if (out_distances[edge.source_idx] < std::numeric_limits<int>::max() && out_distances[node_idx] > out_distances[edge.source_idx] + edge.cost) {
                return true;
            }
                        
        }
    }
    return false;
}

bool Dijkstra::should_stall_backward(int node_idx) {
    for (Edge edge : graph.get_outgoing_edges(node_idx)) {
        if (graph.level_of(edge.target_idx) > graph.level_of(node_idx)) {
            if (in_distances[edge.target_idx] < std::numeric_limits<int>::max() && in_distances[node_idx] > in_distances[edge.target_idx] + edge.cost) {
                return true;
            }
        }
    }
    return false;
}

std::string dtos(int dist) {
    return dist == std::numeric_limits<int>::max() ? "inf" : std::to_string(dist);
}


std::pair<int, int64_t> Dijkstra::query(int start_id, int target_id) {

    int start = graph.get_node_index(start_id);
    int target = graph.get_node_index(target_id);

    if (start >= graph.num_nodes || target >= graph.num_nodes) {
        return { std::numeric_limits<int>::max(), 0 };
    }

    clock::time_point start_time = clock::now();


    for (int node : touched_nodes) {
        in_distances[node] = std::numeric_limits<int>::max();
        out_distances[node] = std::numeric_limits<int>::max();
    }
    touched_nodes.clear();
    bidirectional_queue = std::priority_queue<SearchTriple>();
    bidirectional_queue.emplace(start, 0, std::numeric_limits<int>::max(), true);
    bidirectional_queue.emplace(target, std::numeric_limits<int>::max(), 0, false);
    touched_nodes.push_back(start);
    touched_nodes.push_back(target);
    out_distances[start] = 0;
    in_distances[target] = 0;
    last_start = start;
    
    int shortest_path = std::numeric_limits<int>::max();

    while(!bidirectional_queue.empty()) {         
        SearchTriple node = bidirectional_queue.top();
        bidirectional_queue.pop();

        DEBUG(std::cout << "pop: " << graph.get_node_id(node.idx) << ", [" << (node.is_forward_search ? "forw: " : "back: ") << dtos(node.out_distance) << "," << dtos(node.in_distance) << "]");

        if (settled(node)) {
            if (shortest_path >= node.out_distance + node.in_distance) {
                shortest_path = node.out_distance + node.in_distance;
                DEBUG(std::cout << " -> settled: " << shortest_path);
            } else {
                DEBUG(std::cout << " -> FERTIG");
                break; // FERTIG
            }
        }
        DEBUG(std::cout << "\n");

        if (node.is_forward_search) {
            if (node.out_distance == out_distances[node.idx]) {
                
                if (should_stall_forward(node.idx)) {
                    DEBUG(std::cout << " -> stall\n");
                    continue;
                }

                for (const Edge &edge : graph.get_outgoing_edges(node.idx)) {
                    DEBUG(std::cout << "    (" << graph.get_node_id(edge.source_idx) << "," << graph.get_node_id(edge.target_idx) << "),  " << graph.level_of(edge.source_idx) << " > " << graph.level_of(edge.target_idx) << "\n");
                    if (graph.level_of(edge.target_idx) < graph.level_of(edge.source_idx)) {
                        break;
                    }
                    const int alternative_distance = out_distances[node.idx] + edge.cost;
                    DEBUG(std::cout << "       d=" << dtos(out_distances[edge.source_idx]) << ", alt_d=" << alternative_distance << "=" << out_distances[node.idx] << "+" << edge.cost << "\n");
                    if (out_distances[edge.target_idx] > alternative_distance) {
                        out_distances[edge.target_idx] = alternative_distance;
                        touched_nodes.push_back(edge.target_idx);
                        DEBUG(std::cout << "       push: " << graph.get_node_id(edge.target_idx) << ", [forw: " << dtos(out_distances[edge.target_idx]) << "," << dtos(in_distances[edge.target_idx]) << "]\n");
                        bidirectional_queue.emplace(edge.target_idx, out_distances[edge.target_idx], in_distances[edge.target_idx], true);
                    }
                }
            }
        } else {
            if (node.in_distance == in_distances[node.idx]) {

                if (should_stall_backward(node.idx)) {
                    DEBUG(std::cout << " -> stall\n");
                    continue;
                }

                for (const Edge &edge : graph.get_incoming_edges(node.idx)) {
                    DEBUG(std::cout << "    (" << graph.get_node_id(edge.source_idx) << "," << graph.get_node_id(edge.target_idx) << "),  " << graph.level_of(edge.source_idx) << " < " << graph.level_of(edge.target_idx) << "\n");
                    if (graph.level_of(edge.source_idx) < graph.level_of(edge.target_idx)) {
                        break;
                    }
                    const int alternative_distance = in_distances[node.idx] + edge.cost;
                    DEBUG(std::cout << "       d=" << dtos(in_distances[edge.source_idx]) << ", alt_d=" << alternative_distance << "=" << in_distances[node.idx] << "+" << edge.cost << "\n");
                    if (in_distances[edge.source_idx] > alternative_distance) {
                        in_distances[edge.source_idx] = alternative_distance;
                        touched_nodes.push_back(edge.source_idx);
                        DEBUG(std::cout << "       push: " << graph.get_node_id(edge.source_idx) << ", [forw: " << dtos(out_distances[edge.source_idx]) << "," << dtos(in_distances[edge.source_idx]) << "]\n");
                        bidirectional_queue.emplace(edge.source_idx, out_distances[edge.source_idx], in_distances[edge.source_idx], false);
                    }
                }
            }
        } 
        DEBUG(std::cout << "\n");  
    }

    clock::time_point end_time = clock::now();
    int64_t duration = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time).count();

    return { shortest_path, duration };
}

int Dijkstra::get_shortest_contraction_distance(int source_idx, int target_idx, int direct_distance, std::vector<bool> &contracted) {

    if(last_start != source_idx) {
        for (int node : touched_nodes) {
            out_distances[node] = std::numeric_limits<int>::max();
        }
        unidirectional_queue = std::priority_queue<SearchTuple>();
        unidirectional_queue.emplace(source_idx, 0);
        out_distances[source_idx] = 0;
        last_start = source_idx;
    }

    for (int i = 0; i < graph.num_nodes; i++) {         
        SearchTuple node = unidirectional_queue.top();
        if (node.idx == target_idx && out_distances[node.idx] >= direct_distance) return node.distance;
        unidirectional_queue.pop();
        if (!contracted[node.idx] && node.distance == out_distances[node.idx]) {
            for (const Edge &edge : graph.get_outgoing_edges(node.idx)) {
                const int alternative_distance = out_distances[node.idx] + edge.cost;
                if (out_distances[edge.target_idx] > alternative_distance) {
                    out_distances[edge.target_idx] = alternative_distance;
                    touched_nodes.push_back(edge.target_idx);
                    unidirectional_queue.emplace(edge.target_idx, out_distances[edge.target_idx]);
                }
            }
        }
    }

    return out_distances[target_idx];
}