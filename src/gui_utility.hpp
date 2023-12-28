#ifndef AUTOBUS_GUI_UTILITY_HPP
#define AUTOBUS_GUI_UTILITY_HPP

#include "Graph.hpp"
#include "Station.hpp"

/// Adding new file allows for easier testing

struct PassengerGui
{
    int count = 0;
    int dest_id = -1;
};

[[nodiscard]] Graph<Point2D> ConvertGuiGraphToGraph(const std::vector<uint8_t> &connections,
                                                    const std::vector<Point2D> &stations);


[[nodiscard]] PassengerTable::Table3D ConvertGuiTableToTable3D(
        const std::unordered_map<std::size_t, std::vector<std::vector<PassengerGui>>> &table,
        const std::vector<Point2D> &stations);


#endif //AUTOBUS_GUI_UTILITY_HPP
