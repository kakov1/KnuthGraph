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
  graph_table table_{4, std::vector<table_elem>{}};

  size_type vrtx_num_ = 0;
  size_type edge_num_ = 0;

  size_type edge_table_size_ = 2;
  size_type table_cols_num;

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
  public:
    using iterator_category = std::random_access_iterator_tag;
    using difference_type = std::ptrdiff_t;
    using value_type = table_elem;
    using pointer = value_type *;
    using const_pointer = const pointer;
    using reference = value_type &;
    using const_reference = const reference;

  private:
    class Proxy final {
    private:
      pointer ptr_;

    public:
      Proxy(pointer ptr) : ptr_(ptr) {}
      Edge *operator->() { return std::addressof(std::get<Edge>(*ptr_)); }
    };

    Graph &graph_;
    pointer ptr_;

  public:
    GraphIt(Graph &graph, pointer ptr = nullptr) : graph_(graph), ptr_(ptr) {}

    GraphIt(const GraphIt &rhs) = default;

    GraphIt &operator=(const GraphIt &rhs) = default;

    GraphIt &operator++() {
      auto next_edge_num =
          std::get<size_type>(graph_.table_[2][std::get<Edge>(*ptr_).num_]);

      if (next_edge_num + 1 == graph_.vrtx_num_) {
        ptr_ = std::addressof(*graph_.table_[0].end());

        return *this;
      }

      if (next_edge_num < graph_.vrtx_num_)
        next_edge_num =
            std::get<size_type>(graph_.table_[2][next_edge_num + 1]);

      ptr_ = std::addressof(graph_.table_[0][next_edge_num]);

      return *this;
    }

    GraphIt operator++(int) {
      auto tmp{*this};
      ++*this;

      return tmp;
    }

    GraphIt &operator--() {}

    GraphIt operator--(int) {}

    GraphIt &operator+=(size_type num) {
      while (num-- > 0)
        ++(*this);

      return *this;
    }

    GraphIt operator+(size_type num) const {
      auto tmp{*this};

      tmp += num;

      return tmp;
    }

    bool operator==(const GraphIt &rhs) const { return ptr_ == rhs.ptr_; }

    bool operator!=(const GraphIt &rhs) const { return !(*this == rhs); }

    value_type operator*() const { return value_type{*ptr_}; }

    Proxy operator->() const { return Proxy{ptr_}; }
  };

private:
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

  std::vector<size_type> get_direct_order(size_type vrtx_num) const {
    std::vector<size_type> direct_order;
    direct_order.reserve(table_cols_num);

    do {
      direct_order.push_back(vrtx_num);
      vrtx_num = std::get<size_type>(table_[2][vrtx_num]);
    } while (vrtx_num >= vrtx_num_);

    return direct_order;
  }

  void set_reverse_orders() {
    std::vector<size_type> direct_order;

    for (auto vrtx = 0; vrtx < vrtx_num_; ++vrtx) {
      direct_order = get_direct_order(vrtx);

      table_[3][direct_order.front()] = direct_order.back();

      for (auto edge = direct_order.size() - 1; edge > 0; --edge) {
        table_[3][direct_order[edge]] = direct_order[edge - 1];
      }
    }
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

    set_reverse_orders();
  }

  const graph_table &get_table() const { return table_; }
  size_type get_edge_num() const { return edge_num_; }
  size_type get_vrtx_num() const { return vrtx_num_; }
  size_type get_table_cols_num() const { return table_cols_num; }

  GraphIt begin() {
    return GraphIt{*this, std::addressof(table_[0][vrtx_num_])};
  }

  GraphIt end() { return GraphIt{*this, std::addressof(*table_[0].end())}; }

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
