#include <iostream>
#include <list>
#include <tuple>

struct tram{
    std::list<std::tuple<int, int>> path;
};

int main(int argc, char const *argv[])
{
    int n = 4;
    int m = 4;
    int time = 10;
    int tram_capacity = 50;
    int passengers[n][m][time];

    // determinate numbers of passengers
    for(int i=0; i < n; i++){
        for(int j=0; j < m; j++){
            for(int t=0; t < time; t++){
                passengers[i][j][t] = rand() % 50;
            } 
        }
    }

    // generate trams
    std::list<tram> trams;
    for(int i=0; i < n; i++){
        tram tram_;
        tram_.path = {std::make_tuple(i, 0), std::make_tuple(i, 1), std::make_tuple(i, 2), std::make_tuple(i, 3)};
        trams.push_back(tram_);
    }
    for(int j=0; j < m; j++){
        tram tram_;
        tram_.path = {std::make_tuple(0, j), std::make_tuple(1, j), std::make_tuple(2, j), std::make_tuple(3, j)};
        trams.push_back(tram_);
    }


    // show passengers 
    for(int t=0; t < time; t++){
        for(int i=0; i < n; i++){
            std::cout << i << " ";
            for(int j=0; j < m; j++){
                std::cout << passengers[i][j][t] << " ";
            }
            std::cout << std::endl;
        }
        std::cout << std::endl;

        std::list<tram> new_trams;
        for (auto tram_ : trams) {
            // extract stop
            auto stop = tram_.path.begin();
            if (stop == tram_.path.end()) {
                continue;
            }
            tram_.path.erase(stop);
            int stop_i = std::get<0>(*stop);
            int stop_j = std::get<1>(*stop);

            // extract destination
            auto dest = tram_.path.begin();
            if (dest == tram_.path.end()) {
                continue;
            }
            int dest_i = std::get<0>(*dest);
            int dest_j = std::get<1>(*dest);

            // load passengers
            passengers[stop_i][stop_j][t] = (passengers[stop_i][stop_j][t] > tram_capacity)? passengers[stop_i][stop_j][t] - tram_capacity: 0;

            new_trams.push_back(tram_);
        }
        trams = new_trams;




        for(int i=0; i < n; i++){
            std::cout << i << " ";
            for(int j=0; j < m; j++){
                std::cout << passengers[i][j][t] << " ";
            }
            std::cout << std::endl;
        }
        std::cout << std::endl;
    } 

    return 0;
}
