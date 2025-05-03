#include "graph.h"
#include "util.h"

std::ostream& operator<<(std::ostream& os, const Edge& e) {
    os << "(" << e.source_id << ',' << e.target_id << ",c=" << e.cost << ")";
    return os;
}

std::ostream& operator<<(std::ostream& os, const Node& n) {
    os << "(" << n.id << ",l=" << n.level << ")";
    return os;
}

void Graph::read_from_ch_file(const std::string &file_path) {
    std::ifstream file(file_path);

    if (file.fail()) {
        println_error("CH-Graphendatei nicht gefunden \"" + file_path + "\"");
        return;
    }

    std::string line;
    for (int i = 0; i < 10; i++) {
        file.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    }
    std::getline(file, line);
    num_nodes = std::stoi(line);
    std::getline(file, line);
    num_edges = std::stoi(line);

    for (int i = 0; i < num_nodes; i++) {
        std::getline(file, line);
        size_t pos = line.find(' ', 0);
        Node n{};
        std::from_chars(&line[0], &line[pos], n.id); // ignore errors
        for (int j = 0; j < 4; j++) {
            pos = line.find(' ', pos + 1);
        }
        const size_t end_pos = std::min(line.find('\n', pos + 1), line.find(' ', pos + 1));
        std::from_chars(&line[pos + 1], &line[end_pos], n.level); // ignore errors
        nodes.push_back(n);
    }

    //std::vector<int> node_id_to_index;
    //node_id_to_index.resize(num_nodes);
    //std::iota(node_id_to_index.begin(), node_id_to_index.end(), 0);
    //std::stable_sort(node_id_to_index.begin(), node_id_to_index.end(), [this](size_t i1, size_t i2) { 
    //    return nodes[i1].level < nodes[i2].level || (nodes[i1].level == nodes[i2].level && nodes[i1].id < nodes[i2].id); 
    //}); //TODO: drüber nachdenken

    //std::sort(nodes.begin(), nodes.end(), [this](Node &n1, Node &n2) { 
    //    return n1.level < n2.level || (n1.level == n2.level && n1.id < n2.id); 
    //});


    //std::cout << "nodes = [ ";
    //for (Node n : nodes) std::cout << n << ", ";
    //std::cout << "]\n";

    edges.reserve(num_edges);
    
    while (std::getline(file, line)) {
        const size_t target_pos = line.find(' ', 0);
        const size_t cost_pos = line.find(' ', target_pos + 1);
        const size_t type_pos = line.find(' ', cost_pos + 1);
        const size_t skip1_pos = line.find(' ', line.find(' ', type_pos + 1) + 1);
        const size_t skip2_pos = line.find(' ', skip1_pos + 1);
        const size_t end_pos = std::min(line.find('\n', skip2_pos + 1), line.find(' ', skip2_pos + 1)); // not found? npos?
        
        Edge e{};
        std::from_chars(&line[0], &line[target_pos], e.source_id); // ignore errors
        std::from_chars(&line[target_pos+1], &line[cost_pos], e.target_id);
        std::from_chars(&line[cost_pos+1], &line[end_pos], e.cost);
        
        edges.push_back(e);
    }
    file.close();

    /*
    std::cout << "STANDARD: \n";
    std::cout << "nodes = [ ";
    for (int i : node_ordering) std::cout << nodes[i] << ", ";
    std::cout << "]\n";
    std::cout << "edges = [ ";
    for (Edge &e : edges) std::cout << e << ", ";
    std::cout << "]\n\n";
    */
    
    std::sort(nodes.begin(), nodes.end(), [this] (Node &i1, Node &i2) { 
        return (i1.level > i2.level) || (i1.level == i2.level && i1.id < i2.id);
    });
   // node_id -> node_idx
    node_ordering.resize(num_nodes);
    for (int i = 0; i < num_nodes; i++) {
        node_ordering[nodes[i].id] = i;
    }
    
    std::sort(edges.begin(), edges.end(), [this] (Edge &e1, Edge &e2) { 
        return (node_ordering[e1.source_id] < node_ordering[e2.source_id]) || (e1.source_id == e2.source_id && node_ordering[e1.target_id] < node_ordering[e2.target_id]);
    });
    
    /*
    std::cout << "REORDERED: \n";
    std::cout << "node_ordering = [ ";
    for (int i : node_ordering) std::cout << i << ", ";
    std::cout << "]\n";
    std::cout << "nodes = [ ";
    for (Node n : nodes) std::cout << n << ", ";
    std::cout << "]\n";
    std::cout << "edges = [ ";
    for (Edge &e : edges) std::cout << e << ", ";
    std::cout << "]\n\n";
    */


    target_ordering.resize(num_edges);
    std::iota(target_ordering.begin(), target_ordering.end(), 0);
    std::stable_sort(target_ordering.begin(), target_ordering.end(), [this] (size_t i1, size_t i2) { 
        return (node_ordering[edges[i1].target_id] < node_ordering[edges[i2].target_id]) || (edges[i1].target_id == edges[i2].target_id && node_ordering[edges[i2].source_id] < node_ordering[edges[i2].source_id]);
    });

    generate_offset_list<EdgeType::OUTGOING>(out_offsets);
    generate_offset_list<EdgeType::INCOMING>(in_offsets);

    /*
    std::cout << "target edges = [ ";
    for (int i = 0; i < num_edges; i++) std::cout << edges[target_ordering[i]] << ", ";
    std::cout << "]\n\n";

    std::cout << "out offsets  = [ ";
    for (int i : out_offsets) std::cout << i << ", ";
    std::cout << "]\nin offsets   = [ ";
    for (int i : in_offsets) std::cout << i << ", ";
    std::cout << "]\ntarget order = [ ";
    for (int i : target_ordering) std::cout << i << ", ";
    std::cout << " ]\n\n";
    */
}

void Graph::read_from_graph_file(const std::string &file_path) {

    std::ifstream file(file_path);

    if (file.fail()) {
        println_error("Graphendatei nicht gefunden \"" + file_path + "\"");
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
    
    nodes.resize(num_nodes);
    for(int i = 0; i < num_nodes; i++) {
        nodes[i].id = i;
        nodes[i].level = 0;
    }
    
    while (std::getline(file, line)) {
        const size_t pos1 = line.find(' ', 0);
        const size_t pos2 = line.find(' ', pos1 + 1);
        const size_t pos3 = std::min(line.find('\n', pos2 + 1), line.find(' ', pos2 + 1)); // not found? npos?
        
        Edge e{};
        std::from_chars(&line[0], &line[pos1], e.source_id); // ignore errors
        std::from_chars(&line[pos1+1], &line[pos2], e.target_id);
        std::from_chars(&line[pos2+1], &line[pos3], e.cost);
        
        edges.push_back(e);
    }
    file.close();

    target_ordering.resize(num_edges);
    std::iota(target_ordering.begin(), target_ordering.end(), 0);
    std::stable_sort(target_ordering.begin(), target_ordering.end(), [this](size_t i1, size_t i2) { 
        return edges[i1].target_id < edges[i2].target_id || (edges[i1].target_id == edges[i2].target_id && edges[i1].source_id < edges[i2].source_id); 
    }); //TODO: drüber nachdenken
    
    generate_offset_list<EdgeType::OUTGOING>(out_offsets);
    generate_offset_list<EdgeType::INCOMING>(in_offsets);

    std::cout << "out offsets  = [ ";
    for (int i : out_offsets) std::cout << i << ", ";
    std::cout << "]\nin offsets   = [ ";
    for (int i : in_offsets) std::cout << i << ", ";
    std::cout << "]\ntarget order = [ ";
    for (int i : target_ordering) std::cout << i << ", ";
    std::cout << " ]\n";

    // CH BAUEN:::::



    // find independent set:

    /*
    for (int node_idx = 0; node_idx < num_nodes; node_idx++) {
        if (contracted[node_idx] || adjacent[node_idx]) continue;
        independent_set.push(node_idx);

        for (Edge edge : get_outgoing_edges()) {

        }
    }
    */

}


Graph::Graph(const std::string &file_path) {
    read_from_graph_file(file_path);
}

template<EdgeType edge_type>
void Graph::generate_offset_list(std::vector<int> &offsets) {
    offsets.resize(num_nodes, -1); //TODO: in_offsets und out_offsets zusammenlegen zu [(o1,i1), (o2,i2), ..., (on,in)] ?
    int node_idx = 0;
    int edge_idx = 0;
    while (edge_idx < num_edges && node_idx < num_nodes) { //TODO: iterate through node list instead of node++
        Node node = nodes[node_idx];
        int edge_node = (edge_type == EdgeType::INCOMING) ? edges[target_ordering.at(edge_idx)].target_id : edges[edge_idx].source_id;
        if (edge_node == node.id) {
            offsets.at(node_idx) = edge_idx;
            while (edge_node == node.id && edge_idx < num_edges - 1) {
                edge_idx++;
                edge_node = (edge_type == EdgeType::INCOMING) ? edges[target_ordering.at(edge_idx)].target_id : edges[edge_idx].source_id;
            }
        }
        node_idx++;
    }
    int previous = num_edges; //TODO: WARUM WAR HIER NUM_NODES???
    for (int node_idx = num_nodes - 1; node_idx >= 0; node_idx--) {
        if (offsets.at(node_idx) == -1) {
            offsets.at(node_idx) = previous;
        } else {
            previous = offsets.at(node_idx);
        }
    }
}


EdgeRange<EdgeType::OUTGOING> Graph::get_outgoing_edges(int node_id) {
    return {*this, node_id};
}

EdgeRange<EdgeType::INCOMING> Graph::get_incoming_edges(int node_id) {
    return {*this, node_id};
}

int Graph::get_level(int node_id) {
    return nodes[node_ordering[node_id]].level;
}
