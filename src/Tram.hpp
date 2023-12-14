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
    int positon = 0;
};


template<Hashable T>
std::list<Tram> gen_rand_trams(Graph<T> graph, int tram_amount, int tram_length, T depot){
    std::list<Tram> trams;
    for (int i = 0; i != tram_amount; i++){

        Tram tram;
        tram.add_stop(depot);
        Point2D last = depot;
        for (int j = 0; j != tram_length; j ++){
            auto it = graph.GetEdge(last);
            if (it == NULL){break;}
            auto neighbour = *it;
            int next = rand() % neighbour.size();
            last = neighbour[next];
            tram.add_stop(last);
        }
        trams.push_back(tram);
    }
    return trams;
}

#endif //AUTOBUS_TRAM_HPP
