#include "Problem.hpp"

#include <iostream>


TramProblem::TramProblem(int time_, StationList& stationList_):
time(time_), stationList(stationList_){}

std::tuple<float, uint32_t, float> TramProblem::run(TramList& trams) {
    TramList new_trams(trams);

    // objective function params
    uint32_t transported      = 0;
    uint32_t all_passengers   = stationList.GetPassengersCount();
    uint32_t distance         = 0;
    uint32_t traveled         = 0;



    for (int t = 0; t != time; t++){
//        stationList.DebugPrint();

        // packing people on trams and ride them to next stops
        std::tuple<uint32_t, uint32_t, uint32_t> objective = trams.stop(stationList);

        // objective function update
        transported += std::get<0>(objective);
        traveled    += std::get<1>(objective);
        distance    += std::get<2>(objective);

        stationList.Update();
    }


    trams = new_trams;
    stationList.Clear();
    stationList.Restart();

    return {float(transported) / float(all_passengers), traveled, float(distance) / float(traveled)};
}


