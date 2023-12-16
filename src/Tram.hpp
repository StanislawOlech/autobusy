#ifndef AUTOBUS_TRAM_HPP
#define AUTOBUS_TRAM_HPP

#include "Graph.hpp"

#include <array>
#include <list>
#include <stdexcept>


template <typename T, std::size_t DEPTH, std::size_t COL, std::size_t ROW>
using Matrix3D = std::array<std::array<std::array<T, ROW>, COL>, DEPTH>;

class Tram{
public:
    Tram(std::vector<Point2D> _path) :path{std::move(_path)} {}
    Tram(){};
    Point2D peek_next(int index);
    Point2D stop();
    void add_stop(Point2D next_stop){path.push_back(next_stop);}
private:
    std::vector<Point2D> path;
    bool reverse = false;
    int positon  = 0;
};

#endif //AUTOBUS_TRAM_HPP
