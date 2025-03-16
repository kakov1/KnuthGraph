#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/breadth_first_search.hpp>
#include <boost/program_options.hpp>
#include <fstream>
#include <iostream>
#include <vector>

namespace po = boost::program_options;

using Graph =
    boost::adjacency_list<boost::vecS, boost::vecS, boost::undirectedS>;
using Vertex = boost::graph_traits<Graph>::vertex_descriptor;
using size_type = std::size_t;

const std::string TEST_DIR = std::string(TEST_DATA_DIR) + "/";

enum vertex_color { red, blue, uncolored };

class BFSVisitor : public boost::default_bfs_visitor {
public:
  BFSVisitor(std::vector<vertex_color> &colors) : colors(colors) {}

  void discover_vertex(Vertex u, const Graph &g) const {
    if (colors[u] == uncolored) {
      colors[u] = blue;
    }

    auto neighbors = boost::adjacent_vertices(u, g);
    for (auto v : boost::make_iterator_range(neighbors)) {
      if (colors[v] == uncolored) {
        colors[v] = (colors[u] == red) ? blue : red;
      } else if (colors[v] == colors[u]) {
        throw std::runtime_error("Graph is not bipartite!");
      }
    }
  }

private:
  std::vector<vertex_color> &colors;
};

std::string test(std::string file_name) {
  std::ifstream test_file{file_name};

  size_type start, end;
  int value;
  std::string line;

  Graph g;

  while (std::getline(test_file, line)) {
    if (line.empty())
      continue;

    std::istringstream iss(line);
    std::string token;

    std::getline(iss, token, '-');
    start = std::stoi(token);

    std::getline(iss, token, '-');

    std::getline(iss, token, ',');
    end = std::stoi(token);

    std::getline(iss, token);
    value = std::stoi(token);

    add_edge(start - 1, end - 1, g);
  }

  std::vector<vertex_color> colors(boost::num_vertices(g), uncolored);

  BFSVisitor vis(colors);

  boost::breadth_first_search(g, vertex(0, g), boost::visitor(vis));

  std::string ans;

  for (size_t i = 0; i < colors.size(); ++i) {
    if (colors[i] != uncolored) {
      ans += std::to_string(i + 1) + " ";
      if (colors[i] == red)
        ans += "r ";
      else if (colors[i] == blue)
        ans += "b ";
    }
  }

  return ans;
}

int main(int argc, char *argv[]) {
  try {
    po::options_description desc("Options");
    desc.add_options()("help,h", "Help")("input,i", po::value<std::string>(),
                                         "Input file");

    po::variables_map vm;
    po::store(po::parse_command_line(argc, argv, desc), vm);
    po::notify(vm);

    if (vm.count("help")) {
       std::cout << desc << std::endl;
       return 0;
    }

    if (vm.count("input")) {
      std::cout << test(vm["input"].as<std::string>()) << std::endl;
      ;
    }

  } catch (const std::exception &e) {
    std::cerr << "Error: " << e.what() << std::endl;
    return 1;
  }

  return 0;
}
