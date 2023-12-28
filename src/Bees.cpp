#include "Bees.hpp"

std::string_view CriterionToString(criterion c)
{
    switch (c)
    {
        case max_transported:
            return "Maksymalny przewóz";
        case max_distance:
            return "Maksymalny dystans";
        case CRITERION_NR_ITEMS:
            break;
    }
    return "";
}


Bees::Bees(
          AlgorithmParameters parameters,       TramProblem tramProblem,
          std::mt19937 generator,               Graph<Point2D> graph,
          Point2D depot,                        TramList& trams,
          criterion problem_criterion)
        : tramProblem_(tramProblem),            generator_(generator),
          graph_(graph),                        depot_(depot),
          problem_criterion_(problem_criterion){

    best_bee.trams = trams;
    best_bee.quality = calculateFitness(trams);

    // Initialize scout bees randomly
    for (int i=0; i != parameters.numScouts; i ++) {
        // generate random scouts
        Bee bee;
        bee.trams.gen_rand_trams(graph_, tram_amount, tram_length, depot_, generator_);
        scouts.push_back(bee);
    }

    // Run the initial fitness calculation
    for (auto& scout : scouts) {
        scout.quality = calculateFitness(scout.trams);
    }

    std::sort(scouts.begin(), scouts.end(), std::greater<Bee>());

    if (scouts[0].quality > best_bee.quality){
        best_bee = scouts[0];
    }
}

void Bees::run() {
    for (int iteration = 0; iteration < parameters_.maxIterations; ++iteration) {
        // Perform scout bee exploration
        scoutBeesExplore();

        // Recruit worker bees based on scout information
        recruitWorkerBees();

        // Update the bee algorithm (e.g., perform dance communication)
        updateBeeAlgorithm();
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

Tram Bees::generate_rand_tram() {
    Tram tram;
    tram.add_stop(depot_);
    Point2D last = depot_;
    for (int j = 0; j != tram_length; j ++){
        auto it = graph_.GetEdge(last);

        if (it == nullptr){break;}

        auto neighbour = *it;
        uint32_t next = int(generator_()) % neighbour.size();
        last = neighbour[next];
        tram.add_stop(last);
    }
    tram.set_start_point(abs(int(generator_())) % (tram_length));
    tram.set_direction(bool(generator_() % 1));
    return tram;
}