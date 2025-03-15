#pragma once

#include <algorithm>
#include <iomanip>
#include <iostream>
#include <iterator>
#include <stack>
#include <unordered_map>
#include <utility>
#include <variant>
#include <vector>

namespace hwcg {
template <typename VrtxT, typename EdgeT> class Graph final {
private:
  enum class Color;
  struct Vertex;
  struct Edge;

private:
  using size_type = std::size_t;
  using table_elem = std::variant<size_type, Vertex, Edge>;
  using graph_table = std::vector<std::vector<table_elem>>;
  using colors_map = std::unordered_map<size_type, Color>;

private:
  graph_table table_{4, std::vector<table_elem>{}};
  static constexpr size_type a = 0, t = 1, n = 2, p = 3;

  size_type vrtx_num_ = 0;
  size_type edge_num_ = 0;

  size_type edge_table_size_ = 2;
  size_type table_cols_num;

private:
  enum class Color { White, Blue, Red };

  struct Vertex final {
    size_type num_;
    VrtxT data_;

    explicit Vertex(size_type num, const VrtxT &data = VrtxT{})
        : num_(num), data_(data) {}

    std::string to_string() const { return std::to_string(num_); }
  };

  struct Edge final {
    size_type num_, start_, end_;
    EdgeT data_;

    explicit Edge(size_type num, size_type start, size_type end,
                  const EdgeT &data = EdgeT{})
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

    GraphIt &advance(size_type row, int n) {
      auto &table = graph_.table_;
      auto edge_num =
          std::get<size_type>(table[row][std::get<Edge>(*ptr_).num_]);

      if (n >= 0) {
        if (edge_num + 1 == graph_.vrtx_num_) {
          ptr_ = std::addressof(*table[graph_.a].end());

          return *this;
        }
      } else {
        if (edge_num == 0) {
          ptr_ = std::addressof(*table[graph_.a].rend());

          return *this;
        }
      }

      while (edge_num < graph_.vrtx_num_)
        edge_num = std::get<size_type>(table[row][++edge_num]);

      ptr_ = std::addressof(table[graph_.a][edge_num]);

      return *this;
    }

  public:
    GraphIt(Graph &graph, pointer ptr = nullptr) : graph_(graph), ptr_(ptr) {}

    GraphIt(Graph &graph, size_type col)
        : graph_(graph), ptr_(std::addressof(graph.table_[a][col])) {}

    GraphIt(const GraphIt &rhs) = default;

    GraphIt &operator=(const GraphIt &rhs) = default;

    GraphIt &operator++() { return advance(graph_.n, 1); }

    GraphIt operator++(int) {
      auto tmp{*this};
      ++*this;

      return tmp;
    }

    GraphIt &operator--() { return advance(graph_.p, -1); }

    GraphIt operator--(int) {
      auto tmp{*this};
      --*this;

      return tmp;
    }

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
  template <typename EdgeIt>
  size_type find_max_vrtx(EdgeIt start_edge, EdgeIt end_edge) const {
    auto max_value = std::max_element(
        start_edge, end_edge, [](const auto &a, const auto &b) {
          return std::max(a.first, a.second) < std::max(b.first, b.second);
        });

    return std::max(max_value->first, max_value->second);
  }

  size_type find_place_for_new_edge(size_type pos) const {
    auto cur = std::get<size_type>(table_[n][pos]);

    while (std::get<size_type>(table_[n][cur]) != pos)
      cur = std::get<size_type>(table_[n][cur]);

    return cur;
  }

  void add_new_vrtx(size_type pos) {
    if (table_[a][pos - 1].index() == 0)
      table_[a][pos - 1] = Vertex{pos};
  }

  void add_new_edge(size_type pos, size_type start, size_type end,
                    const EdgeT &data) {
    table_[a][pos] = Edge{pos, start, end, data};
    table_[a][pos + 1] = Edge{pos + 1, end, start, data};

    table_[t][pos] = start;
    table_[t][pos + 1] = end;
  }

  void set_new_refs(size_type vrtx, size_type edge) {
    table_[n][find_place_for_new_edge(vrtx - 1)] = edge;
    table_[n][edge] = vrtx - 1;
  }

  std::vector<size_type> get_direct_order(size_type vrtx_num) const {
    std::vector<size_type> direct_order;
    direct_order.reserve(table_cols_num);

    do {
      direct_order.push_back(vrtx_num);
      vrtx_num = std::get<size_type>(table_[n][vrtx_num]);
    } while (vrtx_num >= vrtx_num_);

    return direct_order;
  }

  void set_reverse_orders() {
    std::vector<size_type> direct_order;

    for (auto vrtx = 0; vrtx < vrtx_num_; ++vrtx) {
      direct_order = get_direct_order(vrtx);

      table_[p][direct_order.front()] = direct_order.back();

      for (auto edge = direct_order.size() - 1; edge > 0; --edge) {
        table_[p][direct_order[edge]] = direct_order[edge - 1];
      }
    }
  }

  bool dfs(size_type vrtx, colors_map &colors) {
    std::stack<std::pair<size_type, GraphIt>> stack;
    stack.emplace(vrtx, GraphIt{*this, std::get<size_type>(table_[n][vrtx])});

    colors[vrtx] = Color::Blue;

    while (!stack.empty()) {
      size_type cur_vrtx = stack.top().first;
      GraphIt *outgoing_edge = &stack.top().second;
      if (*outgoing_edge == end() || (*outgoing_edge)->start_ - 1 != cur_vrtx)
        stack.pop();

      while (*outgoing_edge != end() &&
             (*outgoing_edge)->start_ - 1 == cur_vrtx) {
        if (colors[(*outgoing_edge)->end_ - 1] == Color::White) {
          vrtx = cur_vrtx;
          cur_vrtx = (*outgoing_edge)->end_ - 1;

          ++*outgoing_edge;

          stack.emplace(cur_vrtx, GraphIt{*this, std::get<size_type>(
                                                     table_[n][cur_vrtx])});
          outgoing_edge = &stack.top().second;
          colors[cur_vrtx] =
              colors[vrtx] == Color::Blue ? Color::Red : Color::Blue;

          break;

        } else {
          if (colors[(*outgoing_edge)->end_ - 1] == colors[cur_vrtx]) {
            return false;
          }
        }

        ++*outgoing_edge;
      }
    }

    return true;
  }

  bool bfs(size_type start_vrtx, colors_map &colors) {
    std::stack<std::pair<size_type, Color>> stack;
    stack.push({start_vrtx, Color::Blue});

    while (!stack.empty()) {
      auto [vrtx, color] = stack.top();
      stack.pop();

      if (colors[vrtx] == color) {
        return false;
      }

      colors[vrtx] = color == Color::Blue ? Color::Red : Color::Blue;

      GraphIt outgoing_edge{*this, std::get<size_type>(table_[n][vrtx])};

      while (outgoing_edge != end() && outgoing_edge->start_ - 1 == vrtx) {
        size_type neighbor = outgoing_edge->end_ - 1;

        if (colors[neighbor] == Color::White) {
          stack.push({neighbor, colors[vrtx]});
        } else {
          if (colors[neighbor] == colors[vrtx]) {
            return false;
          }
        }

        ++outgoing_edge;
      }
    }

    return true;
  }

public:
  template <typename EdgeIt, typename DataIt>
  Graph(EdgeIt start_edge, EdgeIt end_edge, DataIt start_data,
        DataIt end_data) {
    edge_num_ = end_edge - start_edge;
    vrtx_num_ = find_max_vrtx(start_edge, end_edge);
    table_cols_num = vrtx_num_ + edge_num_ * edge_table_size_;

    for (auto &&line : table_) {
      line.resize(table_cols_num, 0u);
    }

    for (size_type i = 0; i < vrtx_num_; ++i) {
      table_[n][i] = i;
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

    set_reverse_orders();
  }

  std::string is_bipartite() {
    colors_map colors;

    for (size_type key = 0; key < vrtx_num_; ++key) {
      colors[key] = Color::White;
    }

    std::string ans;

    if (dfs(0, colors)) {
      for (size_type key = 1; key <= vrtx_num_; ++key) {
        if (colors[key - 1] != Color::White) {
          ans += std::to_string(key) + " ";

          if (colors[key - 1] == Color::Blue) {
            ans += "b ";
          } else {
            ans += "r ";
          }
        }
      }
    } else {
      ans = "Isn't bipartite";
    }

    return ans;
  }

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

  size_type get_edge_num() const { return edge_num_; }
  size_type get_vrtx_num() const { return vrtx_num_; }
  size_type get_table_cols_num() const { return table_cols_num; }

  GraphIt begin() { return GraphIt{*this, vrtx_num_}; }
  GraphIt end() { return GraphIt{*this, std::addressof(*table_[a].end())}; }
};

template <typename VrtxT, typename EdgeT>
std::ostream &operator<<(std::ostream &os, const Graph<VrtxT, EdgeT> &graph) {
  graph.print(os);

  return os;
}
} // namespace hwcg
