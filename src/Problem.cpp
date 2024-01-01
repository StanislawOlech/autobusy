#include "Problem.hpp"


TramProblem::TramProblem(int time_, StationList& stationList_):
time(time_), stationList(stationList_){}

std::tuple<float, uint32_t> TramProblem::run(TramList& trams) {
    TramList new_trams(trams);

    // objective function params
    uint32_t transported      = 0;
    uint32_t all_passengers   = 0;
    uint32_t distance         = 0;

    std::mt19937 generator(random_map_seed);


    for (int t = 0; t != time; t++){

        // populating tram stops
        all_passengers += stationList.GenerateRandomPass(generator); // FIXME - change to parameter

        // packing people on trams and ride them to next stops
        std::tuple<uint32_t, uint32_t> objective = trams.stop(stationList);

        // objective function update
        transported += std::get<0>(objective);
        distance    += std::get<1>(objective);

        stationList.Update();
    }

    trams = new_trams;
    stationList.Clear();

    return {float(transported) / float(all_passengers), distance};
}


