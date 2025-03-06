#pragma once

#include <algorithm>
#include <iostream>
#include <utility>
#include <variant>
#include <vector>

namespace hwcg {
template <typename VrtxType, typename EdgeType> class Graph final {
private:
  struct Edge;

private:
  using size_type = std::size_t;
  using graph_table = std::vector<std::vector<size_type>>;

private:
  struct Edge {
    EdgeType data_;
    size_type start_, end_;
  };

private:
  std::vector<EdgeType> elems_;
  graph_table table_{4, std::vector<size_type>{}};

  size_type vrtx_num_ = 0;
  size_type edge_num_ = 0;

  size_type edge_table_size_ = 2;
  size_type table_cols_num = vrtx_num_ + edge_num_ * edge_table_size_;

  template <typename EdgeIter>
  size_type find_max_vrtx(EdgeIter start_edge, EdgeIter end_edge) {
    auto max_value = std::max_element(
        start_edge, end_edge, [](const auto &a, const auto &b) {
          return std::max(a.first, a.second) < std::max(b.first, b.second);
        });

    return std::max(max_value->first, max_value->second);
  }

public:
  template <typename EdgeIter, typename DataIter>
  Graph(EdgeIter start_edge, EdgeIter end_edge, DataIter start_data,
        DataIter end_data) {
    for (; start_data != end_data; ++start_data) {
      elems_.push_back(*start_data);
    }

    edge_num_ = end_edge - start_edge;
    vrtx_num_ = find_max_vrtx(start_edge, end_edge);
    table_cols_num = vrtx_num_ + edge_num_ * edge_table_size_;

    for (auto &&line : table_) {
      line.resize(table_cols_num, 0);
    }

    for (int i = 0; i < vrtx_num_; ++i) {
      table_[0][i] = 0;
    }

    size_type edge_cur = vrtx_num_;

    for (; start_edge != end_edge; ++start_edge, edge_cur += edge_table_size_) {
      table_[0][edge_cur] = edge_cur;
      table_[0][edge_cur + 1] = edge_cur + 1;

      table_[1][edge_cur] = start_edge->first;
      table_[1][edge_cur + 1] = start_edge->second;
      if (start_edge->first == 3)
        std::cout << start_edge->first << std::endl;

      if (table_[2][start_edge->first - 1] == 0) {
        table_[2][start_edge->first - 1] = edge_cur;
      }

      else {
        auto cur = table_[2][start_edge->first - 1];
        while (1) {
          if (table_[2][cur] == 0) {
            if (table_[0][start_edge->first - 1] == 3) {
              std::cout << "hui!" << std::endl;
            }
            table_[2][cur] = edge_cur;
            break;
          }
          cur = table_[2][cur];
        }
      }

      if (table_[2][start_edge->second - 1] == 0) {
        table_[2][start_edge->second - 1] = edge_cur + 1;
      }

      else {
        auto cur = table_[2][start_edge->second - 1];
        while (1) {
          if (table_[2][cur] == 0) {
            if (table_[0][start_edge->second - 1] == 3) {
              std::cout << "hui" << std::endl;
              std::cout << cur << std::endl;
              std::cout << edge_cur + 1 << std::endl;
            }
            table_[2][cur] = edge_cur + 1;
            break;
          }
          cur = table_[2][cur];
        }
      }
    }

    std::cout << *this;
  }

  const graph_table &get_table() const { return table_; }
  size_type get_edge_num() const { return edge_num_; }
  size_type get_vrtx_num() const { return vrtx_num_; }
  size_type get_table_cols_num() const { return table_cols_num; }
};

std::size_t get_num_size(std::size_t num) {
  return std::to_string(num).length();
}

template <typename VrtxType, typename EdgeType>
std::ostream &operator<<(std::ostream &stream,
                         const Graph<VrtxType, EdgeType> &graph) {
  auto table = graph.get_table();
  auto table_cell_size = get_num_size(graph.get_table_cols_num());

  for (auto &&line : table) {
    for (auto &&elem : line) {
      stream << elem
             << std::string(table_cell_size - get_num_size(elem) + 1, ' ');
    }

    stream << std::endl;
  }

  return stream;
}
} // namespace hwcg
