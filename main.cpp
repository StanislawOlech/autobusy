#include "Tram.hpp"
#include "Station.hpp"

#include <array>
#include <iostream>
#include <list>
#include <random>
#include <utility>

std::tuple<uint32_t, uint32_t>  stop(std::list<Tram>& trams, StationList& stationList, int longest_voyage){
    uint32_t transported = 0;
    uint32_t distance    = 0;

    for (auto it = trams.begin(); it != trams.end(); ++it){
        Tram tram = *it;

        Station curentStat = stationList.Get(tram.stop());

        for (int i = 0; i != longest_voyage; i++){
            Point2D dest = tram.peek_next(i);
            uint32_t people_count = curentStat.GetPassengers(dest).count;
            transported += people_count;
            distance += people_count * (i + 1);
            curentStat.DeletePassengers(dest);
        }
    }
    return {transported, distance};
}

template<Hashable T>
std::list<Tram> gen_rand_trams(Graph<T> graph, int tram_amount, int tram_length, T depot){
    std::list<Tram> trams;

    std::random_device rd;
    std::mt19937 generator{seed};

    for (int i = 0; i != tram_amount; i++){

        Tram tram;
        tram.add_stop(depot);
        Point2D last = depot;
        for (int j = 0; j != tram_length; j ++){
            auto it = graph.GetEdge(last);

            if (it == nullptr){break;}

            auto neighbour = *it;
            int next = generator() % neighbour.size();
            last = neighbour[next];
            tram.add_stop(last);
        }
        trams.push_back(tram);
    }
    return trams;
}

int main()
{
    constexpr int n = 3;
    constexpr int m = 3;
    constexpr int time = 10;
    constexpr int tram_amount = 3;
    constexpr int tram_length = 5; // to eliminate in final product
    constexpr int longest_voyage = 3;
    Point2D depot{0, 0};


    // generating graph and stations
    Graph<Point2D> graph{};
    PassengerTable::Table3D table3D;
    StationList stationList{table3D};

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
            stationList.Create({x, y});
        }
    }

    // random trams
    std::list<Tram> trams = gen_rand_trams(graph, tram_amount, tram_length, depot);


    // objective function params
    uint32_t transported      = 0;
    uint32_t all_passengers   = 0;
    uint32_t distance         = 0;


    // algorithm
    for (int t = 0; t != time; t++){

        all_passengers += stationList.GenerateRandomPass();

        std::tuple<uint32_t, uint32_t> objective = stop(trams, stationList, longest_voyage);

        // objective function update
        transported += std::get<0>(objective);
        distance += std::get<1>(objective);

        stationList.Update();
    }


    std::cout << float(transported) / float(all_passengers) << std::endl;
    std::cout << distance << std::endl;
    // x = int_distribution(generator);


    return 0;
}
