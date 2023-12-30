#include "Station.hpp"
#include "Tram.hpp"
#include <iostream>
#include <map>
#include "Bees.hpp"
#include "Problem.hpp"
#include "Settings.hpp"




int main()
{
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

    TramProblem tramProblem(time_itt, stationList);

    // objective function params
    std::tuple<float, uint32_t> objective = tramProblem.run(trams);
    float transported = std::get<0>(objective);
    uint32_t distance = std::get<1>(objective);


    std::cout << transported * 100 << " % passengers transported" <<std::endl;
    std::cout << distance << " units traveled"<<std::endl;

    /*
    std::random_device bees_seed;
    std::mt19937 bees_generator(bees_seed());
    Bees bees(10, 10, 10, 10, 10, tramProblem, bees_generator, graph, depot, trams, max_transported);
    bees.run();
    std::cout << bees.best_bee.quality * 100 << " % passengers transported" <<std::endl;
    */
    return 0;
}
