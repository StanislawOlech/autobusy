#ifndef AUTOBUS_PROBLEM_HPP
#define AUTOBUS_PROBLEM_HPP
#include "Tram.hpp"
#include "Settings.hpp"



class TramProblem{
public:
    TramProblem(int time_, StationList& stationList_);
    std::tuple<float, uint32_t, float> run(TramList trams);
private:
    const int time;
    StationList stationList;
};
#endif
