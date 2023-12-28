#include <gtest/gtest.h>

#include "gui_utility.hpp"

TEST(ConvertGuiGraphToGraphTest, GeneralUse)
{
    std::vector<Point2D> stations = {
            {1, 1},
            {2, 2},
            {3, 3},
            {4, 4},
    };

    // connection {2, 2} <-> {1, 1}, {4, 4} <-> {1, 1}
    std::vector<uint8_t> connections = {
            0, 1, 0, 1,
            1, 0, 0, 0,
            0, 0, 0, 0,
            1, 0, 0, 0
    };

    Graph<Point2D> graph = ConvertGuiGraphToGraph(connections, stations);

    std::vector<Point2D> expected_result1 = {{2, 2}, {4, 4}};
    std::vector<Point2D> expected_result2 = {{1, 1}};
//                       expected_result3 = nullptr;
    std::vector<Point2D> expected_result4 = {{1, 1}};


    auto result1 = graph.GetEdge({1, 1});
    auto result2 = graph.GetEdge({2, 2});
    auto result3 = graph.GetEdge({3, 3});
    auto result4 = graph.GetEdge({4, 4});

    ASSERT_TRUE(result1 != nullptr);
    ASSERT_TRUE(result2 != nullptr);
    ASSERT_TRUE(result3 == nullptr);
    ASSERT_TRUE(result4 != nullptr);

    EXPECT_EQ(*result1, expected_result1);
    EXPECT_EQ(*result2, expected_result2);
    EXPECT_EQ(*result4, expected_result4);
}
