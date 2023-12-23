#ifndef AUTOBUS_BEES_HPP
#define AUTOBUS_BEES_HPP

#include <vector>
#include <tuple>
#include <cmath>
#include <iostream>
#include <random>

class Bees {
public:
    Bees(int numScouts, int numRecruits, int maxIterations, int eliteCount, int neighborhoodSize);

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
    double calculateFitness(const Point2D& location);
    void performDanceCommunication();

private:
    int numScouts;
    int numRecruits;
    int maxIterations;
    int eliteCount;
    int neighborhoodSize;

    // Represents a bee's location and quality
    struct Bee {
        Point2D location;
        double quality;
    };

    // Bee populations
    std::vector<Bee> scouts;
    std::vector<Bee> workers;
    std::vector<Bee> elites;

    // Random number generator
    std::mt19937_64 rng;
};


#endif //AUTOBUS_BEES_HPP
