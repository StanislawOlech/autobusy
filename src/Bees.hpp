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
    Bees(AlgorithmParameters parameters,
         TramProblem tramProblem, double seed, Graph<Point2D> graph, Point2D depot, TramList& trams,
         criterion problem_criterion);

    // Perform the bee algorithm
    Bee run();

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
    std::vector<Bee> workers;
    std::vector<Bee> elites;

    // Random number generator
    std::mt19937_64 rng;

    // Problem
    TramProblem tramProblem_;
    criterion problem_criterion_;

    Graph<Point2D> graph_;
    double seed_;
    Point2D depot_;

};


#endif //AUTOBUS_BEES_HPP
