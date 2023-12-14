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
                //graph.AddEdge({x, y + 1}, {x, y});
            }
            if (x + 1 < n){
                graph.AddEdge({x, y}, {x + 1, y});
                //graph.AddEdge({x + 1, y}, {x, y});
            }
        }
    }

    std::list<Tram> trams = gen_rand_trams(graph, tram_amount, tram_length, depot);


    for (int t = 0; t != time; t++){
        for (auto it = trams.begin(); it != trams.end(); ++it){
            Tram tram = *it;

        }
    }


    // x = int_distribution(generator);


    return 0;
}
