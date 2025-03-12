#pragma once

#include <algorithm>
#include <iomanip>
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

    std::string to_string() const { return std::to_string(num_); }
  };

  struct Edge final {
    size_type num_, start_, end_;
    EdgeType data_;

    explicit Edge(size_type num, size_type start, size_type end,
                  const EdgeType &data = EdgeType{})
        : num_(num), start_(start), end_(end), data_(data) {}

    std::string to_string() const { return std::to_string(num_); }
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

  void add_new_vrtx(size_type pos) {
    if (table_[0][pos - 1].index() == 0)
      table_[0][pos - 1] = Vertex{pos};
  }

  void add_new_edge(size_type pos, size_type start, size_type end,
                    const EdgeType &data) {
    table_[0][pos] = Edge{pos, start, end, data};
    table_[0][pos + 1] = Edge{pos + 1, start, end, data};

    table_[1][pos] = start;
    table_[1][pos + 1] = end;
  }

  void set_new_refs(size_type vrtx, size_type edge) {
    table_[2][find_place_for_new_edge(vrtx - 1)] = edge;
    table_[2][edge] = vrtx - 1;
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

      add_new_vrtx(start_num);
      add_new_vrtx(end_num);

      add_new_edge(edge_cur, start_num, end_num, *start_data);

      set_new_refs(start_num, edge_cur);
      set_new_refs(end_num, edge_cur + 1);
    }

    std::cout << *this;
  }

  const graph_table &get_table() const { return table_; }
  size_type get_edge_num() const { return edge_num_; }
  size_type get_vrtx_num() const { return vrtx_num_; }
  size_type get_table_cols_num() const { return table_cols_num; }

  void print(std::ostream &os) const {
    auto table_cell_size = std::to_string(table_cols_num).length();

    for (auto &&line : table_) {
      for (auto &&elem : line) {
        std::visit(
            [&os, table_cell_size](const auto &value) {
              if constexpr (std::is_same_v<std::decay_t<decltype(value)>,
                                           Vertex> ||
                            std::is_same_v<std::decay_t<decltype(value)>,
                                           Edge>) {
                os << std::left << std::setw(table_cell_size + 1)
                   << value.to_string();
              } else {
                os << std::left << std::setw(table_cell_size + 1) << value;
              }
            },
            elem);
      }
      os << std::endl;
    }
  }
};

template <typename VrtxType, typename EdgeType>
std::ostream &operator<<(std::ostream &os,
                         const Graph<VrtxType, EdgeType> &graph) {
  graph.print(os);

  return os;
}
} // namespace hwcg
