#include "Bees.hpp"

Bees::Bees(
          int numScouts,                        int numRecruits,
          int maxIterations,                    int eliteCount,
          int neighborhoodSize,                 TramProblem tramProblem,
          std::mt19937 generator,               Graph<Point2D> graph,
          Point2D depot,                        TramList& trams,
          criterion problem_criterion)
        : numScouts(numScouts),                 numRecruits(numRecruits),
          maxIterations(maxIterations),         eliteCount(eliteCount),
          neighborhoodSize(neighborhoodSize),   tramProblem_(tramProblem),
          generator_(generator),                graph_(graph),
          depot_(depot),                        problem_criterion_(problem_criterion){

    best_bee.trams = trams;
    best_bee.quality = calculateFitness(trams);

    // Initialize scout bees randomly
    for (int i=0; i != numScouts; i ++) {
        // generate random scouts
        Bee bee;
        bee.trams.gen_rand_trams(graph_, tram_amount, tram_length, depot_, generator_);
        scouts.push_back(bee);
    }

    // Initialize other populations (workers, elites) if needed
    // ...

    // Run the initial fitness calculation
    for (auto& scout : scouts) {
        scout.quality = calculateFitness(scout.trams);

        if (scout.quality > best_bee.quality){
            best_bee = scout;
        }
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

    switch (problem_criterion_) {
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