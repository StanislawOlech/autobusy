#include "Problem.hpp"

#include <iostream>


TramProblem::TramProblem(int time_, StationList& stationList_):
time(time_), stationList(stationList_){}

std::tuple<float, uint32_t, float> TramProblem::run(TramList trams) {

    // objective function params
    uint32_t transported      = 0;
    uint32_t all_passengers   = stationList.GetPassengersCount();
    double distance         = 0;
    uint32_t traveled         = 0;



    for (int t = 0; t != time; t++){

        // packing people on trams and ride them to next stops
        std::tuple<uint32_t, uint32_t, uint32_t> objective = trams.stop(stationList);

        // objective function update
        transported += std::get<0>(objective);
        traveled    += std::get<1>(objective);
        distance    += std::get<2>(objective);

        stationList.Update();
    }


    stationList.Clear();
    stationList.Restart();

    return {float(transported) / float(all_passengers), traveled, (double)transported - distance * 0.1};
}


