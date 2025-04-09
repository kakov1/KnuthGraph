#include "graph.hpp"
#include <boost/algorithm/string.hpp>
#include <cstddef>
#include <fstream>
#include <gtest/gtest.h>
#include <string>
#include <vector>

using namespace hwcg;
using size_type = std::size_t;

const std::string TEST_DIR = std::string(TEST_DATA_DIR) + "/";

class AllTests : public testing::Test {
protected:
  std::string test(size_t test_number) {
    std::ifstream test_file(TEST_DIR + "in/" + std::to_string(test_number) +
                            "test.in");

    test_file.exceptions(std::ifstream::badbit);

    std::vector<std::pair<size_type, size_type>> edges;

    size_type start, end;
    int value;
    std::string line;

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

      edges.emplace_back(start, end);
    }

    Graph<int, int> graph{edges.begin(), edges.end()};

    return graph.is_bipartite();
  }

  std::string get_answer(size_t test_number) {
    std::ifstream answer_file(TEST_DIR + "out/" + std::to_string(test_number) +
                              "test.out");

    answer_file.exceptions(std::ifstream::badbit);

    std::string buf;
    std::string ans;

    while (answer_file >> buf) {
      ans += buf + " ";
    }

    return ans;
  }
};

TEST_F(AllTests, test) {
  for (int i = 1; i <= 10; ++i) {
    auto test_res = test(i);
    auto ans = get_answer(i);

    boost::algorithm::trim(test_res);
    boost::algorithm::trim(ans);

    ASSERT_TRUE(test_res == ans);
  }
}

int main(int argc, char *argv[]) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
