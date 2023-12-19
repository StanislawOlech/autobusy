#ifndef AUTOBUS_GUI_HPP
#define AUTOBUS_GUI_HPP

#include "Graph.hpp"
#include <unordered_set>



class GUI
{
public:

    void Draw();


private:
    void DrawAlgorithm();
    void DrawStationList();
    void DrawStationTable();

    void DrawStyle();

    void DrawPlot();

private:
    std::vector<Point2D> stations_ = {{0, 0}};
    std::vector<uint8_t> connections_{1};
};

#endif //AUTOBUS_GUI_HPP
