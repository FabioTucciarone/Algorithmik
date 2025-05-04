
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <tuple>
#include <chrono>
#include <optional>

#include "util.h"
#include "graph.h"
#include "dijkstra.h"

bool has_cmd_argument(int argc, char **argv, const std::string &option) {
    char **end = argv + argc;
    return std::find(argv, end, option) != end;
}

std::optional<std::string> get_cmd_argument(int argc, char **argv, const std::string &option) {
    char **end = argv + argc;
    char **itr = std::find(argv, end, option);

    if (itr != end && ++itr != end){
        return std::optional<std::string>{*itr};
    } else {
        return std::nullopt;
    }
}

std::pair<int32_t, int64_t> calculate_weakly_connected_components(Graph &graph) {

    using clock = std::chrono::steady_clock;
    clock::time_point start_time = clock::now();

    std::vector<bool> visited;
    visited.resize(graph.num_nodes, false);

    int num_components = 0;

    std::queue<int> q;
    for (int root_node = 0; root_node < graph.num_nodes; root_node++) {
        if (!visited[root_node]) {
            visited.at(root_node) = true;
            q.push(root_node);
            while (!q.empty()) {
                int source_node = q.front();
                q.pop();

                for (Edge edge : graph.get_outgoing_edges(source_node)) {
                    if (!visited[edge.target_idx]) {
                        visited[edge.target_idx] = true;
                        q.push(edge.target_idx);
                    }
                }
                for (Edge edge : graph.get_incoming_edges(source_node)) {
                    if (!visited[edge.source_idx]) {
                        visited[edge.source_idx] = true;
                        q.push(edge.source_idx);
                    }
                }
            }
            num_components++;
        }
    }
    clock::time_point end_time = clock::now();
    int64_t duration = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time).count();

    return {num_components, duration};
}

std::vector<std::pair<int,int>> read_query_file(const std::string &file_path) {

    std::ifstream file(file_path);

    std::string line;
    std::vector<std::pair<int,int>> queries;

    if (file.fail()) {
        println_error("Querydatei nicht gefunden \"" + file_path + "\"");
        return queries;
    }

    while (std::getline(file, line)) {
        const size_t space_pos = line.find(' ', 0);
        const size_t end_pos = std::min(line.find('\n', space_pos + 1), line.find(' ', space_pos + 1));

        int source = 0;
        int target = 0;
        std::from_chars(&line[0], &line[space_pos], source); // ignore errors
        std::from_chars(&line[space_pos + 1], &line[end_pos], target);

        queries.emplace_back(source, target);
    }
    return queries;
}





/// @brief [-graph filepath] [-queries filepath] [-results filepath] [-mode {components, distances, both}]
int main(int argc, char *argv[]) {

    using clock = std::chrono::steady_clock;

    std::string graph_path   = get_cmd_argument(argc, argv, "-graph").value_or("graph.fmi");
    std::string queries_path = get_cmd_argument(argc, argv, "-queries").value_or("queries.txt");
    std::string results_path = get_cmd_argument(argc, argv, "-results").value_or("result.txt");
    std::string mode         = get_cmd_argument(argc, argv, "-mode").value_or("both");

    std::vector<std::pair<int,int>> queries = read_query_file(queries_path);

    Graph graph{ graph_path };
    
    Dijkstra d(graph);

    for (auto [s, t] : queries) {
        auto [distance, duration] = d.query(s, t);
        if (distance == std::numeric_limits<int>::max()) distance = -1;
        std::cout << "[OUT]   " << s << "->" << t << ": d=" << distance << " [t=" << duration << "ms]" << "\n";
    }


}