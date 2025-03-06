#include "graph.hpp"

using namespace hwcg;

int main() {
  std::vector<std::pair<int, int>> a = {{1, 2}, {1, 3}, {2, 3}, {2, 4}, {3, 4}};
  std::vector<int> b = {1, 2, 3, 4, 5};
  Graph<int, int> graph{a.begin(), a.end(), b.begin(), b.end()};
}