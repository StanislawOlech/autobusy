#ifndef AUTOBUS_PROBLEM_HPP
#define AUTOBUS_PROBLEM_HPP
#include "Tram.hpp"
#include "Settings.hpp"



class TramProblem{
public:
    TramProblem(int time_, StationList& stationList_);
    std::tuple<float, uint32_t> run(TramList& trams);
private:
    int time;
    StationList stationList;
};
#endif
