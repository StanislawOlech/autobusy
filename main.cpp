#include "Tram.hpp"

#include <array>
#include <iostream>
#include <list>
#include <random>
#include <utility>


int main()
{
    constexpr int n = 3;
    constexpr int m = 3;
    constexpr int time = 10;
    constexpr int tram_amount = 3;
    constexpr int tram_length = 5; // to eliminate in final product
    Point2D depot{0, 0};

    // Random numbers
    std::random_device rd;
    std::mt19937 generator{rd()};
    std::uniform_int_distribution<> int_distribution{0, 50};

    // generating graph
    Graph<Point2D> graph{};
    for (int x = 0; x != n; x++){
        for (int y = 0; y != m; y++){
            if (y + 1 < m){
                graph.AddEdge({x, y}, {x, y + 1});
            }
            if (x + 1 < n){
                graph.AddEdge({x, y}, {x + 1, y});
            }
        }
    }

    std::list<Tram> trams;
    for (int i = 0; i != tram_amount; i++){

        Tram tram;
        tram.add_stop(depot);
        Point2D last = depot;
        for (int j = 0; j != tram_length; j ++){
            auto neighbour = graph.GetEdge(last);
            if (neighbour.size() == 0){break;}
            int next = rand() % neighbour.size();
            last = neighbour[next];
            tram.add_stop(last);
        }
        trams.push_back(tram);
    }




    for (int t = 0; t != time; t++){

    }


    // x = int_distribution(generator);


    return 0;
}
