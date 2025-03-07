#pragma once

#include <algorithm>
#include <iostream>
#include <iterator>
#include <utility>
#include <variant>
#include <vector>

namespace hwcg {
template <typename VrtxType, typename EdgeType> class Graph final {
private:
  struct Vertex;
  struct Edge;

private:
  using size_type = std::size_t;
  using table_elem = std::variant<size_type, Vertex, Edge>;
  using graph_table = std::vector<std::vector<table_elem>>;

private:
  struct Vertex final {
    size_type num_;
    VrtxType data_;

    explicit Vertex(size_type num, const VrtxType &data = VrtxType{})
        : num_(num), data_(data) {}
  };

  struct Edge final {
    size_type num_, start_, end_;
    EdgeType data_;

    explicit Edge(size_type num, size_type start, size_type end,
                  const EdgeType &data = EdgeType{})
        : num_(num), start_(start), end_(end), data_(data) {}
  };

  class GraphIt final {
  private:
    class Proxy;

  private:
    using iterator_category = std::bidirectional_iterator_tag;
    using difference_type = std::ptrdiff_t;
    using value_type = table_elem;
    using reference = table_elem &;
    using pointer = table_elem *;

    pointer current_;

  private:
    class Proxy final {
    private:
      reference ref_;

    public:
      Proxy() = default;
      pointer operator->() { return std::addressof(ref_); }
    };

  public:
    GraphIt(pointer ptr = nullptr) : current_(ptr) {}

    GraphIt &operator++() {}
  };

private:
  graph_table table_{4, std::vector<table_elem>{}};

  size_type vrtx_num_ = 0;
  size_type edge_num_ = 0;

  size_type edge_table_size_ = 2;
  size_type table_cols_num;

  template <typename EdgeIter>
  size_type find_max_vrtx(EdgeIter start_edge, EdgeIter end_edge) const {
    auto max_value = std::max_element(
        start_edge, end_edge, [](const auto &a, const auto &b) {
          return std::max(a.first, a.second) < std::max(b.first, b.second);
        });

    return std::max(max_value->first, max_value->second);
  }

  size_type find_place_for_new_edge(size_type pos) const {
    auto cur = std::get<size_type>(table_[2][pos]);

    while (std::get<size_type>(table_[2][cur]) != pos)
      cur = std::get<size_type>(table_[2][cur]);

    return cur;
  }

public:
  template <typename EdgeIter, typename DataIter>
  Graph(EdgeIter start_edge, EdgeIter end_edge, DataIter start_data,
        DataIter end_data) {
    edge_num_ = end_edge - start_edge;
    vrtx_num_ = find_max_vrtx(start_edge, end_edge);
    table_cols_num = vrtx_num_ + edge_num_ * edge_table_size_;

    for (auto &&line : table_) {
      line.resize(table_cols_num, 0u);
    }

    for (size_type i = 0; i < vrtx_num_; ++i) {
      table_[2][i] = i;
    }

    size_type edge_cur = vrtx_num_;

    for (; start_edge != end_edge;
         ++start_edge, ++start_data, edge_cur += edge_table_size_) {
      auto [start_num, end_num] = *start_edge;

      if (table_[0][start_num - 1].index() == 0) {
        if (start_data->size() == 3)
          table_[0][start_num - 1] = Vertex{start_num, (*start_data)[0]};
        else
          table_[0][start_num - 1] = Vertex{start_num};
      }

      if (table_[0][end_num - 1].index() == 0) {
        if (start_data->size() == 3)
          table_[0][end_num - 1] = Vertex{end_num, (*start_data)[0]};
        else
          table_[0][end_num - 1] = Vertex{end_num};
      }

      table_[0][edge_cur] =
          Edge{edge_cur, start_num, end_num, (*start_data)[2]};
      table_[0][edge_cur + 1] =
          Edge{edge_cur + 1, start_num, end_num, (*start_data)[2]};

      table_[1][edge_cur] = start_num;
      table_[1][edge_cur + 1] = end_num;

      table_[2][find_place_for_new_edge(start_num - 1)] = edge_cur;
      table_[2][edge_cur] = start_num - 1;

      table_[2][find_place_for_new_edge(end_num - 1)] = edge_cur + 1;
      table_[2][edge_cur + 1] = end_num - 1;
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
      switch (elem.index()) {
      case 0:
        stream << std::get<0>(elem)
               << std::string(table_cell_size -
                                  get_num_size(std::get<0>(elem)) + 1,
                              ' ');
        break;
      case 1:
        stream << std::get<1>(elem).num_
               << std::string(table_cell_size -
                                  get_num_size(std::get<1>(elem).num_) + 1,
                              ' ');
        break;
      case 2:
        stream << std::get<2>(elem).num_
               << std::string(table_cell_size -
                                  get_num_size(std::get<2>(elem).num_) + 1,
                              ' ');
        break;
      }
    }

    stream << std::endl;
  }

  return stream;
}
} // namespace hwcg
