#ifndef AUTOBUS_GUI_HPP
#define AUTOBUS_GUI_HPP

#include "gui_utility.hpp"
#include "Graph.hpp"
#include "Station.hpp"
#include "Bees.hpp"

#include <unordered_set>
#include <functional>
#include <future>


void HelpMarker(const char* desc);

void MakeWarningPopup(const char* name, const char* text);

void MakeInputPositive(const char* name, const char* error_text, int *number);

Bees RunAlgorithm(AlgorithmParameters algorithmParameters, ProblemParameters problemParameters);

bool CheckFileExists(char* file_name);

class GUI
{
public:
    void Draw();

    [[nodiscard]] AlgorithmParameters ExportAlgorithm() const ;

    [[nodiscard]] ProblemParameters ExportProblem() const ;

    void SaveDataToFile();

    void LoadDataFromFile();

    void DrawAlgorithm();
    void DrawStationList();
    void DrawStationTable();
    void DrawPassengers(bool *open);
    void DrawArguments();

    void DrawStyle();

    void DrawResultPlot();
    void DrawResultWindow();

private:
    std::vector<double> y_value_;
    int axis_flag = 1;

    uint32_t objectiveFunCalls_ = 0;
    std::future<Bees> future_bees; // Change to bees
    std::string best_solution_text_ = "Nie uruchomiono jeszcze algorytmu";

    std::vector<Point2D> stations_ = {{0, 0}};
    std::vector<uint8_t> connections_{1}; // Cant be vector<bool> due to specialization

    std::unordered_map<std::size_t, std::vector<std::vector<PassengerGui>>> table3D;

    int max_iter_ = 1;
    int autobus_number_ = 1;
    int simulated_time_ = 1;

    char file_name[32] = "testy/1.txt";

    int solutions_number_ = 1;
    int elite_number_ = 1;
    int best_number_ = 1;
    int elite_size_ = 2;
    int best_size_ = 1;
    int neighborhood_size_ = 5;
    int lifetime_= 10;
    int passenger_loss_rate_ = 2;
    criterion problem_criterion_ = most_efficient;
    Neighborhood neighborhood_ = RANDOM_TRAM_RANDOM_PATH;

    int adaptive_size_ = 0;

    float execution_time_ms_{};
};

#endif //AUTOBUS_GUI_HPP
