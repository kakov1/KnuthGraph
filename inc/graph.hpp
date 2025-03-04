#pragma once

#include <algorithm>
#include <variant>
#include <vector>

namespace hwcg {
template <typename VertexType, typename EdgeType> class Graph final {
private:
  struct Vertex;
  struct Edge;

private:
  using size_type = std::size_t;
  using graph_elem = std::variant<elem_num, Vertex, Edge>;

private:
  struct Vertex {
    VertexType data_;
  };

  struct Edge {
    EdgeType data_;
  };

private:
  std::vector<std::variant<Vertex, Edge>> elems_;
  std::vector<std::vector<graph_elem>> table_{4, std::vector<graph_elem>{}};
  size_type edge_num;
  size_type vrtx_num;

public:
  template <typename EdgeIter, typename DataIter>
  Graph(EdgeIter start_edge, EdgeIter end_edge, DataIter start_data,
        DataIter end_data) {
    edge_num = end_edge - end_data;

    auto vrtx_num = std::max_element(
        start_edge.begin(), end_edge.end(), [](const auto &a, const auto &b) {
          return std::max(a.first, a.second) < std::max(b.first, b.second);
        });

    for (auto &&line : table_) {
      line.reserve(vrtx_num + edge_num);
    }

    for (size_type edge_counter = 0; start_edge != end_edge;
         ++start_edge, ++edge_counter) {
      table_[0][vrtx_num + edge_counter] = vrtx_num + edge_counter;
      table_[1][vrtx_num + edge_counter] = vrtx_num + edge_counter;
    }
  }
};
} // namespace hwcg
