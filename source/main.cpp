
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

bool has_cmd_argument(int argc, char **argv, const std::string &option) {
    char **end = argv + argc;
    return std::find(argv, end, option) != end;
}

char *get_cmd_argument(int argc, char **argv, const std::string &option) {
    char **end = argv + argc;
    char **itr = std::find(argv, end, option);

    if (itr != end && ++itr != end){
        return *itr;
    } else {
        return 0;
    }
}


struct Edge {
    int source;
    int target;
    int cost;

    friend std::ostream& operator<<(std::ostream& os, const Edge& dt);
};

struct DijkstraNode {
    int idx;
    float dist;

    friend bool operator<(const DijkstraNode& a, const DijkstraNode& b);
};

bool operator<(const DijkstraNode& a, const DijkstraNode& b) {
    return a.dist < b.dist;
}

std::ostream& operator<<(std::ostream& os, const Edge& e) {
    os << "(" << e.source << ',' << e.target << ',' << e.cost << ")";
    return os;
}


void apply_random_permutation(std::vector<int> &list) {
    for (int i = 0; i < list.size(); i++) {
        int p = rand() % list.size();
        std::swap(list.at(i), list[p]);
    }  
}

void reorder_edge_list(std::vector<Edge> &edges, std::vector<int> &nodes) {
    //TODO:
}

class Graph {
public:
    std::vector<int> out_offsets;
    std::vector<int> in_offsets;
    std::vector<int> target_ordering;
    std::vector<Edge> edges;
    int num_nodes;
    int num_edges;

    Graph(const std::string &file_path) {
        read_graph_file(file_path);
    }

    void read_graph_file(const std::string &file_path) {
        std::ifstream file(file_path); //TODO: Existiert file_path?

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
            const size_t pos3 = line.find(' ', pos2 + 1);
            
            Edge e{};
            std::from_chars(&line[0], &line[pos1], e.source); // ignore errors
            std::from_chars(&line[pos1+1], &line[pos2], e.target);
            std::from_chars(&line[pos2+1], &line[pos3], e.cost);
            
            edges.push_back(e);
        }

        std::cout << "[INFO] Anzahl Knoten: " << num_nodes << ", Anzahl Kanten: " << num_edges << std::endl;

        target_ordering.resize(num_edges);
        std::iota(target_ordering.begin(), target_ordering.end(), 0);
        std::stable_sort(target_ordering.begin(), target_ordering.end(), [this](size_t i1, size_t i2) { return edges[i1].target < edges[i2].target; }); //TODO: drüber nachdenken, sekundärordnung nach source

        generate_offset_list<false>(out_offsets);
        generate_offset_list<true>(in_offsets);

        std::cout << "\nOffsets (ausgehend):\n";
        for (int i = 0; i < num_nodes; i++) {
            std::cout << out_offsets.at(i) << ", ";
        }
        std::cout << "\nOffsets (eingehend):\n";
        for (int i = 0; i < num_nodes; i++) {
            std::cout << in_offsets.at(i) << ", ";
        }
        std::cout << std::endl;
    }

    template<bool in_list>
    void generate_offset_list(std::vector<int> &offsets) {
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

    std::tuple<int, int> get_outgoing_node_range(int node) { //TODO: implement iterator
        const int start = out_offsets[node];
        const int end = (node == num_nodes - 1) ? num_nodes : out_offsets[node + 1];
        return {start, end};
    }

    std::tuple<int, int> get_ingoing_node_range(int node) { //TODO: implement iterator
        const int start = in_offsets[node];
        const int end = (node == num_nodes - 1) ? num_nodes : in_offsets[node + 1];
        return {start, end};
    }
};


/// @brief [-f filepath] [-t]
int main(int argc, char *argv[]) {

    bool test_mode  = has_cmd_argument(argc, argv, "-t");
    char *file_path = get_cmd_argument(argc, argv, "-f");

    if (!file_path) {
        std::cout << "[FEHLER] Kein Graph gewählt." << std::endl;
        return -1;
    }

    Graph g{std::string(file_path)};

    // Djikstra: https://www2.informatik.uni-stuttgart.de/bibliothek/ftp/medoc.ustuttgart_fi/BCLR-0114/BCLR-0114.pdf

    std::vector<bool> visited; //TODO: bitrepräsentierung
    visited.resize(g.num_nodes, false);

    int components = 0;

    std::queue<int> q;
    for (int root_node = 0; root_node < g.num_nodes; root_node++) {
        if (!visited[root_node]) {
            //DFS:
            visited.at(root_node) = true;
            q.push(root_node);
            while (!q.empty()) {
                int source_node = q.front();
                q.pop();

                std::tuple<int, int> range = g.get_outgoing_node_range(source_node);
                for (int edge_index = std::get<0>(range); edge_index < std::get<1>(range); edge_index++) {
                    int target_node = g.edges[edge_index].target;
                    if (!visited[target_node]) {
                        visited[target_node] = true;
                        q.push(target_node);
                        //std::cout << source_node << "-" << target_node << ",  " << (visited[target_node] ? "visited" : "new") << "\n";
                    }
                }
                range = g.get_ingoing_node_range(source_node);
                for (int edge_index = std::get<0>(range); edge_index < std::get<1>(range); edge_index++) {
                    int target_node = g.edges[g.target_ordering[edge_index]].source;
                    //std::cout << source_node << "-" << target_node << ",  " << (visited[target_node] ? "visited" : "new") << "\n";
                    if (!visited[target_node]) {
                        visited[target_node] = true;
                        q.push(target_node);
                    }
                }
            }
            components++;
            // for (size_t i = 0; i < num_nodes; i++) {
            //     std::cout << (visited[i] ? 1 : 0) << ","; 
            // }
            // std::cout << "\ncomponents=" << components << "\n";
        }
    }

    std::cout << "[INFO] Schwache Zusamenhangskomponenten: " << components << std::endl;

    int s = 2;
    int t = 3;

    std::vector<float> distances;
    distances.resize(g.num_nodes, std::numeric_limits<float>::infinity());
    std::priority_queue<DijkstraNode> pq;

    pq.emplace(s, 0.0f);
    distances[s]= 0.0f;
    
    for (int i = 0; i < g.num_nodes; i++) {
        DijkstraNode node = pq.top();
        pq.pop();
        if (node.dist == distances[node.idx]) {
            //if (node.idx == t) {
            //    break;
            //}
            auto [start, end] = g.get_outgoing_node_range(node.idx);
            for (int edge_index = start; edge_index < end; edge_index++) {
                Edge edge = g.edges[edge_index];
                if (distances[edge.target] > distances[node.idx] + edge.cost) {
                    distances[edge.target] = distances[node.idx] + edge.cost;
                    pq.emplace(edge.target, distances[edge.target]);
                }
            }
        }
    }

    for (int i = 0; i < g.num_nodes; i++) {
        std::cout << distances[i] << ",";
    }

    // for i = 1,...,num_nodes:
    // 

}