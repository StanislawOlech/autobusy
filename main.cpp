#include <array>
#include <iostream>
#include <list>
#include <random>


struct Point2D
{
    int start;
    int end;
};

using Tram = std::list<Point2D>;


template <typename T, std::size_t DEPTH, std::size_t COL, std::size_t ROW>
using Matrix3D = std::array<std::array<std::array<T, ROW>, COL>, DEPTH>;



int main()
{
    constexpr int n = 4;
    constexpr int m = 4;
    constexpr int time = 10;
    constexpr int tram_capacity = 50;

    // int passengers[n][m][time];
    Matrix3D<int, n, m, time> passengers;

    // Random numbers
    std::random_device rd;
    std::mt19937 generator{rd()};
    std::uniform_int_distribution<> int_distribution{0, 50};


    // determinate numbers of passengers
    for (auto &coll: passengers)
    {
        for (auto &row: coll)
        {
            for (auto &x: row)
                x = int_distribution(generator);
        }
    }

    // generate trams
    std::list<Tram> trams;

    for(int i = 0; i < n; ++i){
        trams.emplace_back( Tram({{i, 0}, {i, 1}, {i, 2}, {i, 3}}) );

    }

    for(int j=0; j < m; ++j){
        trams.emplace_back( Tram({{0, j}, {1, j}, {2, j}, {3, j}}) );
    }


    // show passengers
    for(int t=0; t < time; t++){

        for(int i=0; i < n; i++){
            std::cout << i << " ";

            for(int j=0; j < m; j++)
                std::cout << passengers[i][j][t] << " ";

            std::cout << "\n";
        }
        std::cout << std::endl;


        std::list<Tram> new_trams;

        for (auto tram_: trams) {
            // extract stop
            auto stop = tram_.begin();

            if (stop == tram_.end()) {
                continue;
            }

            tram_.erase(stop);
            int stop_i = stop->start;
            int stop_j = stop->end;

            // extract destination
            auto dest = tram_.begin();
            if (dest == tram_.end()) {
                continue;
            }

            int dest_i = dest->start;
            int dest_j = dest->end;

            // load passengers
            auto &curr_passengers = passengers[stop_i][stop_j][t];
            curr_passengers = (curr_passengers > tram_capacity)? curr_passengers - tram_capacity: 0;

            new_trams.push_back(tram_);
        }
        trams = new_trams;


        for(int i = 0; i < n; ++i) {
            std::cout << i << " ";

            for(int j = 0; j < m; ++j) {
                std::cout << passengers[i][j][t] << " ";
            }
            std::cout << "\n";
        }
        std::cout << std::endl;
    }

    return 0;
}
