#include "Station.hpp"
#include "Tram.hpp"
#include <iostream>


int main()
{
    constexpr int n = 2;
    constexpr int m = 2;
    constexpr int time = 10;
    constexpr int tram_amount = 100;
    constexpr int tram_length = 5; // to eliminate in final product
    Point2D depot{0, 0};


    // generating graph and stations
    Graph<Point2D> graph{};
    PassengerTable::Table3D table3D;
    StationList stationList{table3D};

    for (int x = 0; x != n; x++){
        for (int y = 0; y != m; y++){
            if (y + 1 < m){
                graph.AddEdge({x, y}, {x, y + 1});
                graph.AddEdge({x, y + 1}, {x, y});
            }
            if (x + 1 < n){
                graph.AddEdge({x, y}, {x + 1, y});
                graph.AddEdge({x + 1, y}, {x, y});
            }
            if (x + 1 < n && y + 1 < m){
                graph.AddEdge({x, y}, {x + 1, y + 1});
                graph.AddEdge({x + 1, y + 1}, {x, y});
                graph.AddEdge({x + 1, y}, {x, y + 1});
                graph.AddEdge({x, y + 1}, {x + 1, y});
            }
            stationList.Create({x, y});
        }
    }

    // random trams generation
    TramList trams;
    trams.gen_rand_trams(graph, tram_amount, tram_length, depot);

    // objective function params
    uint32_t transported      = 0;
    uint32_t all_passengers   = 0;
    uint32_t distance         = 0;


    // algorithm
    for (int t = 0; t != time; t++){

        // populating tram stops
        all_passengers += stationList.GenerateRandomPass(10);

        // packing people on trams and ride them to next stops
        std::tuple<uint32_t, uint32_t> objective = trams.stop(stationList);

        // objective function update
        transported += std::get<0>(objective);
        distance += std::get<1>(objective);

        stationList.Update();
    }


    std::cout << float(transported) / float(all_passengers) * 100 << " % passengers transported" <<std::endl;
    std::cout << distance << " units traveled"<<std::endl;

    return 0;
}
