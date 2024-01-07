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
    most_efficient = 2,
    CRITERION_NR_ITEMS
};

enum Neighborhood {
    RANDOM_TRAM_RANDOM_PATH = 0,
    RANDOM_TRAM_RANDOM_AFTER= 1,
    NEIGHBORHOOD_NR_ITEMS
};

std::string_view CriterionToString(criterion c);

std::string_view NeighborhoodToString(Neighborhood neighborhood);

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
    int adaptive_size;
};


struct ProblemParameters
{
    ProblemParameters(int i, Graph<struct Point2D> graph, TramProblem problem, StationList list,
                      criterion criterion1, Neighborhood neighborhood);

    int tramCount;
    Graph<Point2D> stations;
    TramProblem tramProblem;
    StationList stationList;
    criterion problemCriterion;
    Neighborhood neighborhood;
};



// Represents a bee's location and quality
class Bee {
public:
    TramList trams;
    double quality;
    uint8_t    age;

    friend bool operator< (const Bee& c1, const Bee& c2){return c1.quality < c2.quality;};
    friend bool operator> (const Bee& c1, const Bee& c2){return c1.quality > c2.quality;};
};



class BeeLocal {
public:
    TramList trams;
    double quality;
    uint32_t distance_from_original = 0;
    friend bool operator< (const BeeLocal& c1, const BeeLocal& c2){return c1.quality < c2.quality;};
    friend bool operator> (const BeeLocal& c1, const BeeLocal& c2){return c1.quality > c2.quality;};
};

BeeLocal CreatLocalBee(const Bee& bee);

class Bees {
public:
    Bees(AlgorithmParameters parameters, uint32_t seed, Point2D depot, ProblemParameters problemParameters);

    // Perform the bee algorithm
    Bee run();

    [[nodiscard]] std::vector<double> getResultIteration() const { return bestValueIteration_; }

    [[nodiscard]] uint32_t getObjectiveFunCalls() const { return objectiveFunCalls; }

    [[nodiscard]] Bee getBestBee() const { return best_bee_; }

private:
    void elites_search();

    void best_search();

    void scouts_search();

    void age();

    // Utility methods
    float calculateFitness(TramList trams);

private:
    AlgorithmParameters parameters_;

    // Bee populations
    std::vector<Bee> solutions;
    Bee best_bee_;

    // in each iteration, for generating plot
    std::vector<double> bestValueIteration_;

    // Problem
    ProblemParameters problemParameters_;

    uint32_t seed_;
    Point2D depot_;
    int tram_amount;

    std::mt19937 generator_;

    uint32_t objectiveFunCalls = 0;
};


#endif //AUTOBUS_BEES_HPP
