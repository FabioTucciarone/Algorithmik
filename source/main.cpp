
#include <iostream>
#include <fstream>
#include <algorithm>
#include <string>
#include <vector>
#include <limits>
#include <charconv>
#include <array>

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

    std::cout << "Nodes: " << num_nodes << "\n";
    std::cout << "Edges: " << num_edges << "\n";

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

    std::vector<int> in_offsets;
    in_offsets.reserve(num_nodes);

    int node = 0;
    bool set = false;

    std::cout << "Offsets:\n";

    for (int i = 0; i < num_edges; ) {
        if (edges[i].source == node) {
            in_offsets[node] = i;
            while (edges[i].source == node) {
                i++;
            }
        } else {
            in_offsets[node] = -1;
        }
        node++;
    }
    while (j < num_nodes) {
        in_offsets[j] = -1;
        node++;
    }

    for (int i = 0; i < num_nodes; i++) {
        std::cout << in_offsets[i] << "\n";
    }



    // Djikstra: https://www2.informatik.uni-stuttgart.de/bibliothek/ftp/medoc.ustuttgart_fi/BCLR-0114/BCLR-0114.pdf


    // ausgehend_offset
    // eingehend_offset


    
}