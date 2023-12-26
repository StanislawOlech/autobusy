#ifndef AUTOBUS_GUI_HPP
#define AUTOBUS_GUI_HPP

#include "Graph.hpp"
#include "Station.hpp"

#include <unordered_set>
#include <functional>

void HelpMarker(const char* desc);

void MakeWarningPopup(const char* name, const char* text);

void MakeInputPositive(const char* name, const char* error_text, int *number);


struct PassengerGui
{
    int count = 0;
    int dest_id = -1;
};


class GUI
{
public:
    void Draw();

private:
    void DrawAlgorithm();
    void DrawStationList();
    void DrawStationTable();
    void DrawPassengers(bool *open);
    void DrawArguments();

    void DrawStyle();

    void DrawPlot();

private:
    std::vector<Point2D> stations_ = {{0, 0}};
    std::vector<uint8_t> connections_{1};

    std::unordered_map<std::size_t, std::vector<std::vector<PassengerGui>>> table3D;

    int max_iter_ = 1;
    int autobus_number_ = 1;

    int solutions_number_ = 1;
    int elite_number_ = 1;
    int best_number_ = 1;
    int elite_size_ = 2;
    int best_size_ = 1;
};

#endif //AUTOBUS_GUI_HPP
