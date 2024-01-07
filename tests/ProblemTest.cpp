#include <gtest/gtest.h>

#include "Problem.hpp"


TEST(ProblemTest, Repeatability)
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

    int all = 0;
    std::map<int, int> answers;

    for (int x = 0; x < 100; x ++){
        std::tuple<float, uint32_t, float> objective = tramProblem.run(trams);
        uint32_t distance = std::get<1>(objective);

        answers[int(distance)] ++;
        all ++;
    }

    std::map<int,int>::iterator best
            = std::max_element(answers.begin(),answers.end(),[] (const std::pair<char,int>& a, const std::pair<char,int>& b)->bool{ return a.second < b.second; } );

    ASSERT_EQ(best->second, all);
}

