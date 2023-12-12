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
    Tram(std::vector<Point2D> _path);
    Point2D peek_next(int index);
    Point2D stop();
private:
    std::vector<Point2D> path;
    bool reverse = false;
    int positon = 0;
};

#endif //AUTOBUS_TRAM_HPP
