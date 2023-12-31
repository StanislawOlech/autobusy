#ifndef AUTOBUS_BEES_HPP
#define AUTOBUS_BEES_HPP

#include <vector>
#include <tuple>
#include <cmath>
#include <iostream>
#include <random>
#include "Tram.hpp"
#include "Problem.hpp"
#include "Settings.hpp"

enum criterion {
    max_transported = 0,
    max_distance = 1,
    CRITERION_NR_ITEMS
};

std::string_view CriterionToString(criterion c);

struct AlgorithmParameters
{
    int solutionsNumber;

    int bestCount;// best_non-elite = best - elite
    int eliteCount; // elite <= best

    int bestRecruits;
    int eliteRecruits;

    int neighborhoodSize;
    int maxIterations;
    int beeLifeTime;
};


struct ProblemParameters
{
    ProblemParameters(int i, Graph<struct Point2D> graph, TramProblem problem, StationList list,
                      criterion criterion1);

    int tramCount;
    Graph<Point2D> stations;
    TramProblem tramProblem;
    StationList stationList;
    criterion problemCriterion;
};



// Represents a bee's location and quality
class Bee {
public:
    TramList trams;
    double quality;
    friend bool operator< (const Bee& c1, const Bee& c2){return c1.quality < c2.quality;};
    friend bool operator> (const Bee& c1, const Bee& c2){return c1.quality > c2.quality;};
};

class Bees {
public:
    Bees(AlgorithmParameters parameters, uint32_t seed, Point2D depot, ProblemParameters problemParameters);

    // Perform the bee algorithm
    Bee run();

    [[nodiscard]] std::vector<double> getResultIteration() const { return bestValueIteration_; }

private:
    void elites_search();

    void best_search();

    void scouts_search();


    // Utility methods
    float calculateFitness(TramList trams);

private:
    AlgorithmParameters parameters_;

    // Bee populations
    std::vector<Bee> solutions;

    // in each iteration, for generating plot
    std::vector<double> bestValueIteration_;

    // Problem
    ProblemParameters problemParameters_;

    uint32_t seed_;
    Point2D depot_;

    std::mt19937 generator_;
};


#endif //AUTOBUS_BEES_HPP
