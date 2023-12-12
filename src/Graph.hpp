#ifndef AUTOBUS_GRAPH_HPP
#define AUTOBUS_GRAPH_HPP

#include <unordered_map>
#include <vector>
#include <cinttypes>
#include <span>
#include <algorithm>
#include <ostream>
#include <ranges>


template<typename T>
concept Hashable = requires(T a) { {std::hash<T>{}(a)} -> std::convertible_to<std::size_t>; };

template<typename T>
concept Printable = requires(std::ostream& os, T a) { os << a; };


struct Point2D
{
    constexpr Point2D() = default;
    constexpr Point2D(int x, int y): x{x}, y{y} {}

    int x{}, y{};

    bool operator==(Point2D other) const { return x == other.x && y == other.y; }
    bool operator!=(Point2D other) const { return !(*this == other); }

    friend std::ostream& operator<<(std::ostream &oss, Point2D point);
};

constexpr int NormL1(Point2D start, Point2D end)
{
    return abs(start.x - end.x) + abs(start.y - end.y);
}


template<Hashable T>
class Graph
{
public:
    using GraphT = std::unordered_map<T, std::vector<T>>;

    Graph() = default;
    explicit Graph(GraphT graph): graph_{std::move(graph)} {}


    const std::vector<T>& GetEdge(T start) const { return graph_.at(start); }

    void AddEdge(T start, T end) { graph_[start].push_back(end); graph_[end].push_back(start); }

    void AddEdge(T start, std::span<T> ends)
    {
        graph_[start].insert(graph_[start].end(), ends.begin(), ends.end());
        for (auto end: ends)
        {
            AddEdge(end, start);
        }
    }

    void AddEdge(T start, std::initializer_list<T> ends)
    {
        graph_[start].insert(graph_[start].end(), ends.begin(), ends.end());
        for (auto end: ends)
        {
            AddEdge(end, start);
        }
    }


    auto begin() const { return graph_.begin(); }
    auto cbegin() const { return graph_.cbegin(); }
    auto end() const { return graph_.end(); }
    auto cend() const { return graph_.cend(); }

    // operator<< is optional
    template<Printable V>
    friend std::ostream& operator<<(std::ostream &os, const Graph<V>& graph);

private:
    GraphT graph_{};
};



template <Printable V>
std::ostream &operator<<(std::ostream &oss, const Graph<V> &graph)
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

#endif //AUTOBUS_GRAPH_HPP
