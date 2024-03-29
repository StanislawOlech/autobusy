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
            int dist = 1;
            if (y + dist < m){
                graph.AddEdge({x, y}, {x, y + dist});
                graph.AddEdge({x, y + dist}, {x, y});
            }
            if (x + dist < n){
                graph.AddEdge({x, y}, {x + dist, y});
                graph.AddEdge({x + dist, y}, {x, y});
            }
            if (x + dist < n && y + dist < m){
                graph.AddEdge({x, y}, {x + dist, y + dist});
                graph.AddEdge({x + dist, y + dist}, {x, y});
                graph.AddEdge({x + dist, y}, {x, y + dist});
                graph.AddEdge({x, y + dist}, {x + dist, y});
            }
            stationList.Create({x, y});
        }
    }

    // random trams generation
    TramList trams;
    trams.gen_rand_trams(graph, tram_amount, tram_length, depot);

    TramProblem tramProblem(time_itt, stationList);

    // objective function params
    std::tuple<float, uint32_t, float> objective = tramProblem.run(trams);
    float transported = std::get<2>(objective);
    uint32_t distance = std::get<1>(objective);


    std::cout << transported * 100 << " % passengers transported" <<std::endl;
    std::cout << distance << " units traveled"<<std::endl;


    std::random_device bees_seed{};
    AlgorithmParameters algorithmParameters{};
    algorithmParameters.solutionsNumber  = 20;
    algorithmParameters.bestCount        = 10;
    algorithmParameters.eliteCount       =  5;
    algorithmParameters.bestRecruits     =  5;
    algorithmParameters.eliteRecruits    = 10;
    algorithmParameters.neighborhoodSize = 10;
    algorithmParameters.maxIterations    =100;
    algorithmParameters.beeLifeTime      = 10;

    ProblemParameters problemParameters(tram_amount, graph, tramProblem, stationList, most_efficient);


    Bees bees(algorithmParameters, bees_seed(), depot, problemParameters);
    Bee best_bee = bees.run();
    std::cout << best_bee.quality * 100 << " % passengers transported" <<std::endl;

    return 0;
}
