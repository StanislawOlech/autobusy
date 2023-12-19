#ifndef AUTOBUS_GUI_HPP
#define AUTOBUS_GUI_HPP

#include "Graph.hpp"
#include <unordered_set>


void HelpMarker(const char* desc);


class GUI
{
public:

    void Draw();


private:
    void DrawAlgorithm();
    void DrawStationList();
    void DrawStationTable();
    void DrawArguments();

    void DrawStyle();

    void DrawPlot();

private:
    std::vector<Point2D> stations_ = {{0, 0}};
    std::vector<uint8_t> connections_{1};

    int iteration_number_ = 1;
    int autobus_number = 1;
};

#endif //AUTOBUS_GUI_HPP
