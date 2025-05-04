#include "graph.h"
#include "util.h"
#include "math.h"

std::ostream& operator<<(std::ostream& os, const Edge& e) {
    os << "(" << e.source_idx << ',' << e.target_idx << ",c=" << e.cost << ")";
    return os;
}

std::ostream& operator<<(std::ostream& os, const Node& n) {
    os << "(" << n.id << ",l=" << n.level << ")";
    return os;
}

bool operator<(const Edge& edge_1, const Edge& edge_2) {
    return edge_1.source_idx < edge_2.source_idx || (edge_1.source_idx == edge_2.source_idx && edge_1.target_idx < edge_2.target_idx);
}


void  Graph::permute_graph() {

    std::sort(nodes.begin(), nodes.end(), [this] (Node &i1, Node &i2) { 
        return (i1.level > i2.level) || (i1.level == i2.level && i1.id < i2.id);
    });

    node_ordering.resize(num_nodes);
    node_ordering.resize(nodes.size());
    for (int i = 0; i < nodes.size(); i++) {
        node_ordering[nodes[i].id] = i;
    }

    for (Edge &edge : edges) {
        edge.source_idx = node_ordering[edge.source_idx];
        edge.target_idx = node_ordering[edge.target_idx];
    }

    std::sort(edges.begin(), edges.end(), [this] (Edge &e1, Edge &e2) { 
        return (e1.source_idx < e2.source_idx) || (e1.source_idx == e2.source_idx && e1.target_idx < e2.target_idx);
    });
}

int Graph::get_node_index(int node_id) {
    return node_ordering[node_id];
}

int Graph::get_node_id(int node_idx) {
    return nodes[node_idx].id;
}

template<EdgeType edge_type>
void Graph::generate_offset_list(std::vector<int> &offsets) {

    offsets.clear();
    offsets.resize(num_nodes, -1);

    int node_idx = 0;
    int edge_idx = 0;
    while (edge_idx < num_edges && node_idx < num_nodes) {
        int edge_node = (edge_type == EdgeType::INCOMING) ? edges[target_ordering.at(edge_idx)].target_idx : edges[edge_idx].source_idx;
        if (edge_node == node_idx) {
            offsets.at(node_idx) = edge_idx;
            while (edge_node == node_idx && edge_idx < num_edges - 1) {
                edge_idx++;
                edge_node = (edge_type == EdgeType::INCOMING) ? edges[target_ordering.at(edge_idx)].target_idx : edges[edge_idx].source_idx;
            }
        }
        node_idx++;
    }
    int previous = num_edges;
    for (int node_idx = num_nodes - 1; node_idx >= 0; node_idx--) {
        if (offsets.at(node_idx) == -1) {
            offsets.at(node_idx) = previous;
        } else {
            previous = offsets.at(node_idx);
        }
    }
}

void Graph::build_graph_representation() {
    
    num_edges = edges.size();
    target_ordering.clear();
    target_ordering.resize(num_edges);

    std::iota(target_ordering.begin(), target_ordering.end(), 0);
    std::stable_sort(target_ordering.begin(), target_ordering.end(), [this] (size_t i1, size_t i2) { 
        return (edges[i1].target_idx < edges[i2].target_idx) || (edges[i1].target_idx == edges[i2].target_idx && edges[i2].source_idx < edges[i2].source_idx);
    });

    generate_offset_list<EdgeType::OUTGOING>(out_offsets);
    generate_offset_list<EdgeType::INCOMING>(in_offsets);
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

    edges.reserve(num_edges);
    
    while (std::getline(file, line)) {
        const size_t target_pos = line.find(' ', 0);
        const size_t cost_pos = line.find(' ', target_pos + 1);
        const size_t type_pos = line.find(' ', cost_pos + 1);
        const size_t skip1_pos = line.find(' ', line.find(' ', type_pos + 1) + 1);
        const size_t skip2_pos = line.find(' ', skip1_pos + 1);
        const size_t end_pos = std::min(line.find('\n', skip2_pos + 1), line.find(' ', skip2_pos + 1)); // not found? npos?
        
        Edge e{};
        std::from_chars(&line[0], &line[target_pos], e.source_idx); // ignore errors
        std::from_chars(&line[target_pos+1], &line[cost_pos], e.target_idx);
        std::from_chars(&line[cost_pos+1], &line[end_pos], e.cost);
        
        edges.push_back(e);
    }
    file.close();

    permute_graph();
    build_graph_representation();
}


std::vector<int> Graph::find_independent_set(std::vector<bool> &contracted) {
    std::vector<bool> adjacent(num_nodes, false); // REALLOC 
    std::vector<int> independent_set; // REALLOC; init?

    for (int node_idx = 0; node_idx < num_nodes; node_idx++) {
        if (contracted[node_idx] || adjacent[node_idx]) continue;

        independent_set.push_back(node_idx);
        for (const Edge &edge : get_outgoing_edges(node_idx)) {
            adjacent[edge.target_idx] = true;
        }
        for (const Edge &edge : get_incoming_edges(node_idx)) {
            adjacent[edge.source_idx] = true;
        }
    }
    return independent_set;
}

std::vector<int> Graph::partition_independent_set(int num_partitions, std::vector<int> &independent_set, std::vector<int> &edge_differences, std::vector<bool> &contracted) {

    // CONTRACT INDEPENDENT SET:
    for (int node_idx : independent_set) {
        // SIM-CONTRACT NODE
        edge_differences[node_idx] = 0;

        for (Edge in_edge : get_incoming_edges(node_idx)) { // approximate with in_deg * out_deg - deg?
            for (Edge out_edge : get_outgoing_edges(node_idx)) {
                if (in_edge.source_idx != out_edge.target_idx && in_edge.source_idx != node_idx && out_edge.target_idx != node_idx && !contracted[out_edge.target_idx] && !contracted[in_edge.source_idx]) {
                    edge_differences[node_idx]++;
                } 
            }
        }
        EdgeRange incoming = get_incoming_edges(node_idx);
        EdgeRange outgoing = get_incoming_edges(node_idx);
        edge_differences[node_idx] -= (incoming.size() + outgoing.size()); // eins zu klein falls schleife existiert (wird zwei mal abgezogen)
    }

    //std::cout << "\nindependent_set = { ";
    //for (int i = 0; i < independent_set.size(); i++) std::cout << "[" << independent_set[i] << ", " << edge_differences[independent_set[i]] << "] ";
    //std::cout << "}\n";

    int partition_size = independent_set.size() / num_partitions;
    std::vector<int> partitions(num_partitions + 1, 0);
    for (int p = 1; p < num_partitions; p++) {
        partitions[p] = p * partition_size + partitions[p - 1];
    }
    partitions[num_partitions] = independent_set.size();

    return partitions;
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
        std::from_chars(&line[0], &line[pos1], e.source_idx); // ignore errors
        std::from_chars(&line[pos1+1], &line[pos2], e.target_idx);
        std::from_chars(&line[pos2+1], &line[pos3], e.cost);
        
        edges.push_back(e);
    }
    file.close();

    build_graph_representation();

    std::cout << "\nBuilding CH:\n";

    //std::vector<Shortcut> shortcut_info;
    //shortcut_info.reserve(num_edges);
    //for (int id = 0; id < num_edges; id++) {
    //    shortcut_info.emplace_back(id, -1, -1);
    //}

    // CH BAUEN:::::
    int level_counter = 0;
    int num_uncontracted = num_nodes;

    std::vector<bool> contracted(num_nodes, false);
    std::vector<int> edge_differences(num_nodes, 0); // REALLOC 

    while (num_uncontracted > 0) {
        // FIND INDEPENDENT SET:

        //std::cout << "\n\x1B[31m[ITERATION: " << level_counter << "]\033[0m \n";

        int num_partitions = 4; //TODO: sinnvoll berechnen
        std::vector<int> independent_set = find_independent_set(contracted); // TODO: reallocates two arrays every time...
        std::vector<int> partitions = partition_independent_set(num_partitions, independent_set, edge_differences, contracted);

        std::vector<std::vector<Edge>> shortcut_collection(num_partitions, std::vector<Edge>());

        //#pragma omp parallel for shared(contracted, edge_differences, nodes, partitions, independent_set, shortcut_collection)
        for (int p = 0; p < num_partitions; p++) {
            
            auto iter = independent_set.begin();
            auto start = iter + partitions[p];
            auto end   = iter + partitions[p + 1];

            std::stable_sort(start, end, [&](size_t i1, size_t i2) { 
                return edge_differences[i1] < edge_differences[i2]; 
            });
            
            //std::cout << "\npartition[" << p << "] = { ";
            //for (int i = partitions[p]; i < partitions[p + 1]; i++) std::cout << "[" << independent_set[i] << ", " << edge_differences[independent_set[i]] << "] ";
            //std::cout << "}\n";

            Dijkstra dijkstra(*this);
        
            int num_to_contract = std::ceil(0.75 * (partitions[p + 1] - partitions[p]));

            for (int i = partitions[p]; i < partitions[p] + num_to_contract; i++) {
                int node_idx = independent_set[i];

                //std::cout << "   contracting: " << node_idx << " (l=" << level_counter << "), shortcuts:\n";

                nodes[node_idx].level = level_counter;
                for (const Edge &in_edge : get_incoming_edges(node_idx)) {
                    for (const Edge &out_edge : get_outgoing_edges(node_idx)) {
                        //std::cout << "   ?1\n";
                        if (contracted[out_edge.target_idx] || contracted[in_edge.source_idx]) continue;

                        if (in_edge.source_idx != out_edge.target_idx && in_edge.source_idx != node_idx && out_edge.target_idx != node_idx) {
                            
                            int direct_distance = in_edge.cost + out_edge.cost;
                            int shortest_distance = dijkstra.get_shortest_contraction_distance(in_edge.source_idx, out_edge.target_idx, contracted);
                            
                            Edge shortcut {in_edge.source_idx, out_edge.target_idx, in_edge.cost + out_edge.cost};
                            //std::cout << shortcut << " ";
                            if (shortest_distance >= direct_distance) { 
                                shortcut_collection[p].push_back(shortcut);
                            }
                        }
                    }
                }
                //std::cout << "\n";
            }
        }

        level_counter++;

        for (int node_idx : independent_set) {
            contracted[node_idx] = true;
        }
        for (std::vector<Edge> &shortcuts : shortcut_collection) { // TODO: achtung hinundherkopiererei
            for (Edge &shortcut : shortcuts) {
                edges.push_back(shortcut);
                //shortcut_info.push_back(num_edges + num_shortcuts_added, )
            }
        }
        
        std::sort(edges.begin(), edges.end());
        //std::stable_sort(shortcut_info.begin(), shortcut_info.end(), [this](size_t i1, size_t i2) {return edges[i1] < edges[i2];});

        build_graph_representation();

        num_uncontracted -= independent_set.size();
    }
    
    permute_graph();

    std::cout << "\n\x1B[31m[RESULT]\033[0m all edges: {\n";
    for (Edge &e : edges) std::cout << "  (" << get_node_id(e.source_idx) << "," << get_node_id(e.target_idx) << ") " << level_of(e.source_idx) << " " << level_of(e.target_idx) << "\n";
    std::cout << "}\n";

}


Graph::Graph(const std::string &file_path) {
    read_from_graph_file(file_path);
}


EdgeRange<EdgeType::OUTGOING> Graph::get_outgoing_edges(int node_idx) {
    return {*this, node_idx};
}

EdgeRange<EdgeType::INCOMING> Graph::get_incoming_edges(int node_idx) {
    return {*this, node_idx};
}

int Graph::level_of(int node_idx) {
    return nodes[node_idx].level;
}
