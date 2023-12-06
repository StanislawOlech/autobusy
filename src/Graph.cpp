#include "Graph.hpp"

#include <cmath>
#include <ranges>


double normL2(Point2D start, Point2D end)
{
    return sqrt(pow((start.x - end.x), 2) + pow((start.y - end.y), 2));
}

std::ostream &operator<<(std::ostream &oss, Point2D point)
{
    oss << "{" << point.x << ", " << point.y << "}";
    return oss;
}

void Graph::AddEdge(Point2D start, std::span<Point2D> ends)
{
    graph_[start].insert(graph_[start].end(), ends.begin(), ends.end());
}

void Graph::AddEdge(Point2D start, std::initializer_list<Point2D> ends)
{
    graph_[start].insert(graph_[start].end(), ends.begin(), ends.end());
}

std::ostream &operator<<(std::ostream &oss, const Graph &graph)
{
    oss << "{\n";
    for (auto [start, vec]: graph)
    {
        oss << start << ": {";
        for (auto end: vec | std::views::take(vec.size() - 1))
        {
            oss << end << ", ";
        }
        oss << *vec.rbegin() << "},\n";
    }
    oss << "}";
    return oss;
}
