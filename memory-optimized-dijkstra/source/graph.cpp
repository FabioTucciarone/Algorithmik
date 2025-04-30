#include "graph.h"

std::ostream& operator<<(std::ostream& os, const Edge& e) {
    os << "(" << e.source << ',' << e.target << ',' << e.cost << ")";
    return os;
}

Graph::Graph(const std::string &file_path) {
    std::ifstream file(file_path);

    if (file.fail()) {
        std::cout << "Graphendatei nicht gefunden\n";
        return;
    }

    std::string line;
    for (int i = 0; i < 5; i++) {
        file.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    }
    std::getline(file, line);
    num_nodes = std::stoi(line);
    std::getline(file, line);
    num_edges = std::stoi(line);

    for (int i = 0; i < num_nodes; i++) {
        file.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    }
    
    edges.reserve(num_edges);
    
    while (std::getline(file, line)) {
        const size_t pos1 = line.find(' ', 0);
        const size_t pos2 = line.find(' ', pos1 + 1);
        const size_t pos3 = std::min(line.find('\n', pos2 + 1), line.find(' ', pos2 + 1)); // not found? npos?
        
        Edge e{};
        std::from_chars(&line[0], &line[pos1], e.source); // ignore errors
        std::from_chars(&line[pos1+1], &line[pos2], e.target);
        std::from_chars(&line[pos2+1], &line[pos3], e.cost);
        
        edges.push_back(e);
    }
    file.close();

    target_ordering.resize(num_edges);
    std::iota(target_ordering.begin(), target_ordering.end(), 0);
    std::stable_sort(target_ordering.begin(), target_ordering.end(), [this](size_t i1, size_t i2) { 
        return edges[i1].target < edges[i2].target || (edges[i1].target == edges[i2].target && edges[i1].source < edges[i2].source); 
    }); //TODO: drüber nachdenken
    
    generate_offset_list<false>(out_offsets);
    generate_offset_list<true>(in_offsets);
}

template<bool in_list>
void Graph::generate_offset_list(std::vector<int> &offsets) {
    offsets.resize(num_nodes, -1); //TODO: in_offsets und out_offsets zusammenlegen zu [(o1,i1), (o2,i2), ..., (on,in)] ?
    int node = 0;
    int edge_idx = 0;
    while (edge_idx < num_edges && node < num_nodes) { //TODO: iterate through node list instead of node++
        int edge_node = in_list ? edges[target_ordering.at(edge_idx)].target : edges[edge_idx].source;
        if (edge_node == node) {
            offsets.at(node) = edge_idx;
            while (edge_node == node && edge_idx < num_edges - 1) {
                edge_idx++;
                edge_node = in_list ? edges[target_ordering.at(edge_idx)].target : edges[edge_idx].source;
            }
        }
        node++;
    }
    int previous = num_nodes;
    for (int i = num_nodes - 1; i >= 0; i--) {
        if (offsets.at(i) == -1) {
            offsets.at(i) = previous;
        } else {
            previous = offsets.at(i);
        }
    }
}


EdgeRange<EdgeType::OUTGOING> Graph::get_outgoing_edges(int node) {
    return {*this, node};
}

EdgeRange<EdgeType::INCOMING> Graph::get_incoming_edges(int node) {
    return {*this, node};
}
