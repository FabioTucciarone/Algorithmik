
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
    std::vector<int> out_offsets;
    std::vector<int> in_offsets;
    std::vector<int> target_ordering;

};


/// @brief [-f filepath] [-t]
int main(int argc, char *argv[]) {

    bool test_mode  = has_cmd_argument(argc, argv, "-t");
    char *file_path = get_cmd_argument(argc, argv, "-f");

    if (!file_path) {
        std::cout << "[FEHLER] Kein Graph gewählt." << std::endl;
        return -1;
    }

    std::ifstream file(file_path);

    //TODO: Existiert file_path?

    std::string line;

    for (int i = 0; i < 5; i++) {
        file.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    }
    std::getline(file, line);
    const int num_nodes = std::stoi(line);
    std::getline(file, line);
    const int num_edges = std::stoi(line);


    for (int i = 0; i < num_nodes; i++) {
        file.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    }

    std::vector<Edge> edges;
    edges.reserve(num_edges);

    while (std::getline(file, line)) {
        const size_t pos1 = line.find(' ', 0);
        const size_t pos2 = line.find(' ', pos1+1);
        const size_t pos3 = line.find(' ', pos2+1);
        
        Edge e{};
        std::from_chars(&line[0], &line[pos1], e.source); // ignore errors
        std::from_chars(&line[pos1+1], &line[pos2], e.target);
        std::from_chars(&line[pos2+1], &line[pos3], e.cost);
        
        edges.push_back(e);
    }


    //TODO: create node list
    //TODO: sort edges according to node list

    std::vector<int> out_offsets;
    std::vector<int> in_offsets;
    std::vector<int> target_ordering(num_edges);
    out_offsets.resize(num_nodes, -1);
    in_offsets.resize(num_nodes, -1); // TODO: in_offsets und out_offsets zusammenlegen zu [(o1,i1), (o2,i2), ..., (on,in)] ?

    int node = 0;
    
    for (int i = 0; i < num_edges && node < num_nodes; ) { //TODO: iterate through node list instead of node++
        if (edges.at(i).source == node) {
            out_offsets.at(node) = i;
            while (edges.at(i).source == node && i < num_edges - 1) { // warum -1
                i++;
            }
        }
        node++;
    }
    while (node < num_nodes) {
        out_offsets.at(node) = -1;
        node++;
    }

    int previous = num_nodes;
    for (int i = num_nodes-1; i >= 0; i--) {
        if (out_offsets.at(i) == -1) {
            out_offsets.at(i) = previous;
        } else {
            previous = out_offsets.at(i);
        }
    }

    std::iota(target_ordering.begin(), target_ordering.end(), 0);
    std::stable_sort(target_ordering.begin(), target_ordering.end(), [&edges](size_t i1, size_t i2) { return edges[i1].target < edges[i2].target; }); //TODO: drüber nachdenken, sekundärordnung nach source

    node = 0;
    for (int i = 0; i < num_edges && node < num_nodes; ) { //TODO: iterate through node list instead of node++
        if (edges[target_ordering.at(i)].target == node) {
            in_offsets.at(node) = i;
            while (edges[target_ordering.at(i)].target == node && i < num_edges - 1) {
                i++;
            }
        }
        node++;
    }
    while (node < num_nodes) {
        in_offsets.at(node) = -1;
        node++;
    }
    
    previous = num_nodes;
    for (int i = num_nodes-1; i >= 0; i--) {
        if (in_offsets.at(i) == -1) {
            in_offsets.at(i) = previous;
        } else {
            previous = in_offsets.at(i);
        }
    }

#if false
    std::cout << "Kanten (Startordnung):\n";
    for (int i = 0; i < num_edges; i++) {
        std::cout << edges.at(i) << "\n";
    }
    std::cout << "Offsets (ausgehend):\n";
    for (int i = 0; i < num_nodes; i++) {
        std::cout << out_offsets.at(i) << "\n";
    }

    std::cout << "Kanten (Zielordnung):\n";
    for (int i = 0; i < num_edges; i++) {
        std::cout << edges.at(target_ordering.at(i)) << "\n";
    }
    std::cout << "Offsets (eingehend):\n";
    for (int i = 0; i < num_nodes; i++) {
        std::cout << in_offsets.at(i) << "\n";
    }
#endif

    // Djikstra: https://www2.informatik.uni-stuttgart.de/bibliothek/ftp/medoc.ustuttgart_fi/BCLR-0114/BCLR-0114.pdf

    std::vector<bool> visited; //TODO: bitrepräsentierung
    visited.resize(num_nodes, false);

    int components = 0;

    for (int root_node = 0; root_node < num_nodes; root_node++) {
        if (!visited[root_node]) {
            //DFS:
            std::queue<int> q;
            visited.at(root_node) = true;
            q.push(root_node);
            while (!q.empty()) {
                int source_node = q.front();
                q.pop();

                int max = (source_node == num_nodes - 1) ? num_nodes : out_offsets[source_node + 1];
                for (int edge_index = out_offsets[source_node]; edge_index < max; edge_index++) {
                    int target_node = edges[edge_index].target;
                    if (!visited[target_node]) {
                        visited[target_node] = true;
                        q.push(target_node);
                        //std::cout << source_node << "-" << target_node << ",  " << (visited[target_node] ? "visited" : "new") << "\n";
                    }
                }
                max = (source_node == num_nodes - 1) ? num_nodes : in_offsets[source_node + 1];
                for (int edge_index = in_offsets[source_node]; edge_index < max; edge_index++) {
                    int target_node = edges[target_ordering[edge_index]].source;
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
}