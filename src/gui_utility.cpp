#include "gui_utility.hpp"


Graph<Point2D> ConvertGuiGraphToGraph(const std::vector<uint8_t> &connections,
                                      const std::vector<Point2D> &stations)
{
    Graph<Point2D> graph{};

    for (int row = 0; row < stations.size(); ++row)
    {
        for (int column = 0; column < stations.size(); ++column)
        {
            std::size_t idx;

            if (row >= column)
                idx = row * stations.size() + column;
            else
                idx = column * stations.size() + row;

            bool value = (bool)connections[idx];
            if (value)
                graph.AddEdge(stations[row], stations[column]);
        }
    }
    return graph;
}

PassengerTable::Table3D ConvertGuiTableToTable3D(
        const std::unordered_map<std::size_t, std::vector<std::vector<PassengerGui>>> &table,
        const std::vector<Point2D> &stations)
{
    PassengerTable::Table3D passengerTable;

    for (auto &[start_station_id, vec2D] : table)
    {
        auto start_station = stations[start_station_id];
        std::vector<std::vector<Passenger>> passengers2D;

        for (auto& vec: vec2D)
        {
            std::vector<Passenger> passengers;

            for (auto [count, end_station_idx]: vec)
            {
                auto end_station = stations[end_station_idx];

                Passenger passenger{
                        .start = start_station,
                        .end = end_station,
                        .count = static_cast<uint32_t>(count)
                };

                passengers.push_back(passenger);
            }
            passengers2D.push_back(passengers);
        }
        passengerTable[start_station] = passengers2D;
    }
    return passengerTable;
}
