#ifndef AUTOBUS_GRAPH_HPP
#define AUTOBUS_GRAPH_HPP

#include <unordered_map>
#include <vector>
#include <cinttypes>
#include <span>
#include <algorithm>
#include <ostream>


struct Point2D
{
    uint32_t x{}, y{};

    bool operator==(Point2D other) const { return x == other.x && y == other.y; }
    bool operator!=(Point2D other) const { return !(*this == other); }

    friend std::ostream& operator<<(std::ostream &oss, Point2D point);
};


double normL2(Point2D start, Point2D end);


// Required for unordered_map
template<>
struct std::hash<Point2D>
{
    std::size_t operator()(const Point2D &point) const noexcept
    {
        std::size_t h1 = std::hash<uint32_t>{}(point.x);
        std::size_t h2 = std::hash<uint32_t>{}(point.y);
        return h1 ^ (h2 << 1);
    }
};


class Graph
{
public:
    using GraphT = std::unordered_map<Point2D, std::vector<Point2D>>;

    Graph() = default;
    explicit Graph(GraphT graph): graph_{std::move(graph)} {}

    const std::vector<Point2D>& GetEdge(Point2D start) const { return graph_.at(start); }

    void AddEdge(Point2D start, Point2D end) { graph_[start].push_back(end); }
    void AddEdge(Point2D start, std::span<Point2D> ends);
    void AddEdge(Point2D start, std::initializer_list<Point2D> ends);

    auto begin() const { return graph_.begin(); }
    auto cbegin() const { return graph_.cbegin(); }
    auto end() const { return graph_.end(); }
    auto cend() const { return graph_.cend(); }

    friend std::ostream& operator<<(std::ostream &os, const Graph& graph);

private:
    GraphT graph_{};
};

#endif //AUTOBUS_GRAPH_HPP
