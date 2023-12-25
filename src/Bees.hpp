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

enum criterion{
    max_transported = 0,
    max_distance = 1
};

class Bees {
public:
    Bees(int numScouts, int numRecruits, int maxIterations, int eliteCount, int neighborhoodSize,
         TramProblem tramProblem, std::mt19937 generator, Graph<Point2D> graph, Point2D depot);

// Initialize the bee algorithm
    void initialize();

    // Perform the bee algorithm
    void run();

private:
    // Bee-related methods
    void scoutBeesExplore();
    void recruitWorkerBees();
    void updateBeeAlgorithm();

    // Utility methods
    float calculateFitness(TramList trams);
    void performDanceCommunication();

private:
    int numScouts;
    int numRecruits;
    int maxIterations;
    int eliteCount;
    int neighborhoodSize;

    // Represents a bee's location and quality
    struct Bee {
        TramList trams;
        double quality;
    };

    // Bee populations
    std::vector<Bee> scouts;
    std::vector<Bee> workers;
    std::vector<Bee> elites;

    // Random number generator
    std::mt19937_64 rng;

    // Problem
    TramProblem tramProblem_;
    criterion problem_criterion;

    Graph<Point2D> graph_;
    std::mt19937 generator_;
    Point2D depot_;

};


#endif //AUTOBUS_BEES_HPP
