#pragma once

#include <algorithm>
#include <initializer_list>
#include <iomanip>
#include <iostream>
#include <iterator>
#include <map>
#include <numeric>
#include <set>
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
  using colors_map = std::map<size_type, Color>;
  using edge_pair = std::pair<size_type, size_type>;
  using edges_vector = std::vector<edge_pair>;

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
    size_type pos_, num_;
    VrtxT data_;

    explicit Vertex(size_type pos, size_type num, const VrtxT &data = VrtxT{})
        : pos_(pos), num_(num), data_(data) {}

    std::string to_string() const { return std::to_string(pos_); }
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
    using const_pointer = const value_type *;
    using reference = value_type &;
    using const_reference = const value_type &;

  private:
    class Proxy final {
    private:
      const_pointer ptr_;

    public:
      Proxy(const_pointer ptr) : ptr_(ptr) {}
      const Edge *operator->() const {
        return std::addressof(std::get<Edge>(*ptr_));
      }
    };

    const Graph &graph_;
    const_pointer ptr_;

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
    GraphIt(const Graph &graph, const_pointer ptr = nullptr)
        : graph_(graph), ptr_(ptr) {}

    GraphIt(const Graph &graph, size_type col)
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
  size_type find_vrtx_num(EdgeIt start, EdgeIt end) const {
    std::set<size_type> vrtcs;

    for (; start != end; ++start) {
      vrtcs.insert(start->first);
      vrtcs.insert(start->second);
    }

    return vrtcs.size();
  }

  size_type find_place_for_new_edge(size_type pos) const {
    auto cur = std::get<size_type>(table_[n][pos]);

    while (std::get<size_type>(table_[n][cur]) != pos)
      cur = std::get<size_type>(table_[n][cur]);

    return cur;
  }

  size_type add_new_vrtx(size_type vrtx,
                         std::unordered_map<size_type, size_type> &vrtcs) {
    if (vrtcs.find(vrtx) == vrtcs.end()) {
      auto pos = vrtcs.size();
      vrtcs[vrtx] = pos;
      table_[a][pos] = Vertex{pos, vrtx};
    }

    return vrtcs[vrtx];
  }

  void add_new_edge(size_type pos, size_type start, size_type end) {
    table_[a][pos] = Edge{pos, start, end};
    table_[a][pos + 1] = Edge{pos + 1, end, start};

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

  template <typename EdgeIt> void sort_edges(EdgeIt start, EdgeIt end) const {
    std::for_each(start, end, [](edge_pair &edge) {
      if (edge.first > edge.second)
        std::swap(edge.first, edge.second);
    });

    std::sort(start, end);
  }

  bool dfs(size_type vrtx, colors_map &colors) const {
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

  bool bfs(size_type start_vrtx, colors_map &colors) const {
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
  Graph(std::initializer_list<edge_pair> list)
      : Graph(list.begin(), list.end()) {}

  template <typename EdgeIt> Graph(EdgeIt start_edge, EdgeIt end_edge) {
    edges_vector edges = std::vector<edge_pair>{start_edge, end_edge};
    sort_edges(edges.begin(), edges.end());

    auto start = edges.begin();
    auto end = edges.end();

    edge_num_ = end - start;
    vrtx_num_ = find_vrtx_num(start, end);
    table_cols_num = vrtx_num_ + edge_num_ * edge_table_size_;

    for (auto &&line : table_) {
      line.resize(table_cols_num, 0u);
    }

    std::iota(table_[n].begin(), std::next(table_[n].begin(), vrtx_num_), 0u);

    size_type edge_cur = vrtx_num_;
    std::unordered_map<size_type, size_type> vrtcs;

    for (; start != end; ++start, edge_cur += edge_table_size_) {
      auto [start_num, end_num] = *start;

      auto start_pos = add_new_vrtx(start_num, vrtcs);
      auto end_pos = add_new_vrtx(end_num, vrtcs);

      add_new_edge(edge_cur, start_pos + 1, end_pos + 1);

      set_new_refs(start_pos + 1, edge_cur);
      set_new_refs(end_pos + 1, edge_cur + 1);
    }

    set_reverse_orders();
  }

  std::string is_bipartite() const {
    colors_map colors;

    for (size_type key = 0; key < vrtx_num_; ++key) {
      colors[key] = Color::White;
    }

    std::string ans;
    colors_map colors_num;

    if (dfs(0, colors)) {
      for (size_type key = 0; key < vrtx_num_; ++key) {
        colors_num[std::get<Vertex>(table_[a][key]).num_] = colors[key];
      }

      for (auto &&[vrtx, color] : colors_num) {
        ans += std::to_string(vrtx) + " ";

        if (color == Color::Blue) {
          ans += "b ";
        } else {
          ans += "r ";
        }
      }
    }

    else {
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

  GraphIt begin() const { return GraphIt{*this, vrtx_num_}; }
  GraphIt end() const {
    return GraphIt{*this, std::addressof(*table_[a].end())};
  }
};

template <typename VrtxT, typename EdgeT>
std::ostream &operator<<(std::ostream &os, const Graph<VrtxT, EdgeT> &graph) {
  graph.print(os);

  return os;
}
} // namespace hwcg
