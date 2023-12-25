#include "Bees.hpp"

Bees::Bees(
          int numScouts,                        int numRecruits,
          int maxIterations,                    int eliteCount,
          int neighborhoodSize,                 TramProblem tramProblem,
          std::mt19937 generator,               Graph<Point2D> graph,
          Point2D depot)
        : numScouts(numScouts),                 numRecruits(numRecruits),
          maxIterations(maxIterations),         eliteCount(eliteCount),
          neighborhoodSize(neighborhoodSize),   tramProblem_(tramProblem),
          generator_(generator),                graph_(graph),
          depot_(depot){ this->initialize();}

void Bees::initialize() {
    // Initialize scout bees randomly
    for (auto& scout : scouts) {
        // generate random scouts
        scout.trams.gen_rand_trams(graph_, tram_amount, tram_length, depot_, generator_);
    }

    // Initialize other populations (workers, elites) if needed
    // ...

    // Run the initial fitness calculation
    for (auto& scout : scouts) {
        scout.quality = calculateFitness(scout.trams);
    }
}

void Bees::run() {
    for (int iteration = 0; iteration < maxIterations; ++iteration) {
        // Perform scout bee exploration
        scoutBeesExplore();

        // Recruit worker bees based on scout information
        recruitWorkerBees();

        // Update the bee algorithm (e.g., perform dance communication)
        updateBeeAlgorithm();

        // Additional logic if needed
        // ...
    }
}



void Bees::scoutBeesExplore() {
    // Logic for scout bees to explore the search space
    // ...
}

void Bees::recruitWorkerBees() {
    // Logic for recruiting worker bees based on scout information
    // ...
}

void Bees::updateBeeAlgorithm() {
    // Update the bee algorithm, e.g., perform dance communication
    performDanceCommunication();

    // Additional update logic if needed
    // ...
}

float Bees::calculateFitness(TramList trams) {
    // Calculate the fitness (quality) of a Trams
    std::tuple<float, uint32_t> objective = tramProblem_.run(trams);

    switch (problem_criterion) {
        case max_transported:
            return std::get<0>(objective);
        case max_distance:
            return float(std::get<1>(objective));
    }
}

void Bees::performDanceCommunication() {
    // Logic for dance communication among bees
    // ...
}