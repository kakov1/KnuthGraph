#include "graph.hpp"
#include <sstream>

using namespace hwcg;
using size_type = std::size_t;

int main() {
  std::vector<std::pair<size_type, size_type>> edges;
  std::vector<int> data;

  size_type start, end;
  int value;
  std::string line;

  while (std::getline(std::cin, line)) {
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

    edges.emplace_back(start, end);
    data.emplace_back(value);
  }

  Graph<int, int> graph{edges.begin(), edges.end(), data.begin(), data.end()};

  //auto beg = graph.begin();
  //std::advance(beg, 5);
  //std::cout << beg->to_string()<<std::endl;
  //for (auto &&it: graph) {
    //std::cout << std::get<2>(it).num_ << std::endl;
    //beg++;
  //}
  
  std::cout << graph.color_vrts()<<std::endl;

  //std::cout << graph;
}