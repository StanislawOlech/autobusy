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
    Bee() = default;
    Bee(const TramList& trams, double quality, uint8_t age);

    TramList trams;
    double quality;
    uint8_t    age;

    friend bool operator< (const Bee& c1, const Bee& c2){return c1.quality < c2.quality;};
    friend bool operator> (const Bee& c1, const Bee& c2){return c1.quality > c2.quality;};
    friend bool operator==(const Bee& c1, const Bee& c2) { return c1.GetId() == c2.GetId(); }

    [[nodiscard]] uint64_t GetId() const { return id; }

private:
    uint64_t id; // is unique
    inline static uint64_t all_id = 0;
};

template<>
struct std::hash<Bee>
{
    std::size_t operator()(const Bee& bee) const noexcept
    {
        return std::hash<uint64_t>{}(bee.GetId());;
    }
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

    std::unordered_map<Bee, std::vector<BeeLocal>> localBees_;

    // Problem
    ProblemParameters problemParameters_;

    uint32_t seed_;
    Point2D depot_;
    int tram_amount;

    std::mt19937 generator_;

    uint32_t objectiveFunCalls = 0;
};


#endif //AUTOBUS_BEES_HPP
