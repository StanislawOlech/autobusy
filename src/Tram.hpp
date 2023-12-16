#ifndef AUTOBUS_TRAM_HPP
#define AUTOBUS_TRAM_HPP

#include "Graph.hpp"

#include <array>
#include <list>
#include <stdexcept>
#include <random>
#include "Station.hpp"
#define  seed rd()


template <typename T, std::size_t DEPTH, std::size_t COL, std::size_t ROW>
using Matrix3D = std::array<std::array<std::array<T, ROW>, COL>, DEPTH>;

class Tram{
public:
    Tram(std::vector<Point2D> _path) :path{std::move(_path)} {}
    Tram(){};
    Point2D peek_next(int index);
    Point2D stop();
    void add_stop(Point2D next_stop){path.push_back(next_stop);}
    void set_start_point(int positon_){positon = positon_;};
private:
    std::vector<Point2D> path;
    bool reverse = false;
    int positon  = 0;
};


class TramList{
public:
    explicit TramList(uint32_t longest_voyage_=3) :longest_voyage(longest_voyage_){};
    void gen_rand_trams(Graph<Point2D> graph, int tram_amount, int tram_length, Point2D depot);
    std::tuple<uint32_t, uint32_t> stop(StationList& stationList);
private:
    void update();
    std::tuple<uint32_t, uint32_t> transfers(StationList& stationList, int traveled, Point2D station, Station orginalStation);

    uint32_t longest_voyage;
    std::list<Tram> trams;
};

#endif //AUTOBUS_TRAM_HPP
