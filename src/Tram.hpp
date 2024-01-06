#ifndef AUTOBUS_TRAM_HPP
#define AUTOBUS_TRAM_HPP

#include "Graph.hpp"

#include <array>
#include <list>
#include <stdexcept>
#include <random>
#include <utility>
#include "Station.hpp"


class Tram{
public:
    explicit Tram(std::vector<Point2D> _path) :path{std::move(_path)} {}
    Tram()= default;
    Tram(Tram const &other) = default;
    Point2D peek_next(int index);
    Point2D stop();
    void add_stop(Point2D next_stop);
    void set_start_point(int position_){position = position_;};
    void set_path(std::vector<Point2D> new_path) {path = std::move(new_path);};
    void set_direction(bool reverse_){reverse = reverse_;};

    [[nodiscard]] std::vector<Point2D> get() const {return (path);};

    [[nodiscard]] bool has(Point2D point2D) const { return std::ranges::find(path, point2D) != std::end(path); }

    [[nodiscard]] std::optional<uint32_t> distance_stations_count(Point2D point2D) const;

    [[nodiscard]] std::optional<double> distance_with_punishment(Point2D point2D) const;


    void DebugPrint() const;
    [[nodiscard]] std::string Print() const;

private:
    std::vector<Point2D> path;
    bool reverse  = false;
    int  position = 0;
};



class TramList{
public:
    explicit TramList(uint32_t longest_voyage_=3) :longest_voyage(longest_voyage_){};
    TramList(TramList const &other) = default;

    void gen_rand_trams(const Graph<Point2D>& graph, int tram_amount, int tram_length, Point2D depot);
    void gen_rand_trams(const Graph<Point2D>& graph, int tram_amount, int tram_length, Point2D depot, std::mt19937& generator);

    void gen_rand_unique(const Graph<Point2D>& graph, int tram_amount, int tram_length, Point2D depot, std::mt19937& generator);

    std::tuple<uint32_t, uint32_t, double> stop(StationList& stationList);

    void deleteTram(std::size_t position);

    void DebugPrint() const;

    [[nodiscard]] std::string Print() const;

private:
    void update();
    std::tuple<uint32_t, uint32_t, double> transfers(StationList& stationList, int traveled, Point2D station, Point2D orginalpoint);

    uint32_t longest_voyage;
    std::list<Tram> trams;
};

#endif //AUTOBUS_TRAM_HPP
