#include <gtest/gtest.h>

#include "Graph.hpp"


TEST(GraphTest, GeneralUse)
{
    std::vector<Point2D> vec1 = {{1, 2}, {3, 1}, {5, 6}};
    std::vector<Point2D> vec2 = {{9, 0}, {9, 9}};

    Graph<Point2D> graph{};

    // using std::initialization_list
//    graph.AddEdge({0, 0}, {{1, 2}, {3, 1}, {5, 6}});
    graph.AddEdge({0, 0,}, vec1);

    graph.AddEdge({0, 9}, vec2);

    ASSERT_EQ(graph.GetEdge({0, 0}), vec1);
    ASSERT_EQ(graph.GetEdge({0, 9}), vec2);
}