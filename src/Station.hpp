#ifndef AUTOBUS_STATION_HPP
#define AUTOBUS_STATION_HPP

#include "Graph.hpp"

#include <functional>
#include <utility>

using Station = Point2D;


struct Passenger
{
    Station destination;
};


class StationWithPassenger
{
    void Update();

private:
    std::unordered_map<Station, std::vector<Passenger>> passengers_on_station_;
};


#endif //AUTOBUS_STATION_HPP
