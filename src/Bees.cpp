#include "Bees.hpp"

#include <utility>

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
          AlgorithmParameters parameters, double seed,  Point2D depot, TramList& trams, ProblemParameters problemParameters)
        : seed_(seed), depot_(depot), problemParameters_(std::move(problemParameters)), parameters_(parameters){

    // original bee becomes solution bee
    Bee firstBee;
    firstBee.trams = trams;
    firstBee.quality = calculateFitness(trams);
    solutions.push_back(firstBee);

    // Initialize rest of solutions bees randomly
    for (int i=0; i != parameters.solutionsNumber - 1; i ++) {
        // generate generator
        std::mt19937 generator(seed_);
        seed_ = seed_ + generator();

        // generate random solutions
        Bee bee;
        bee.trams.gen_rand_trams(problemParameters_.stations, tram_amount, tram_length, depot_, generator);
        bee.quality = calculateFitness(bee.trams);
        solutions.push_back(bee);
    }

    std::sort(solutions.begin(), solutions.end(), std::greater<>());
}

Bee Bees::run() {
    for (int iteration = 0; iteration != parameters_.maxIterations; iteration++) {
        elites_search();

        best_search();

        scouts_search();

        std::sort(solutions.begin(), solutions.end(), std::greater<>());
    }
    return solutions[0];
}

void Bees::elites_search() {
    for (int i=0; i != parameters_.eliteCount; i ++) {
        Bee newBee(solutions[i]);

        for (int j=0; j != parameters_.eliteRecruits; j ++) {
            Bee tempBee(solutions[i]);

            // generate generator
            std::mt19937 generator(seed_);
            seed_ = seed_ + generator();

            tempBee.trams.deleteTram(generator() % tram_amount);
            tempBee.trams.gen_rand_trams(problemParameters_.stations, 1, tram_length, depot_);
            tempBee.quality = calculateFitness(tempBee.trams);

            if (tempBee.quality > newBee.quality){newBee = tempBee;}
        }
        if (newBee.quality > solutions[i].quality){solutions[i] = newBee;}
    }
}

void Bees::best_search() {
    for (int i=parameters_.eliteCount; i != parameters_.bestCount; i ++) {
        Bee newBee(solutions[i]);

        for (int j=0; j != parameters_.bestRecruits; j ++) {
            Bee tempBee(solutions[i]);

            // generate generator
            std::mt19937 generator(seed_);
            seed_ = seed_ + generator();

            tempBee.trams.deleteTram(generator() % tram_amount);
            tempBee.trams.gen_rand_trams(problemParameters_.stations, 1, tram_length, depot_);
            tempBee.quality = calculateFitness(tempBee.trams);

            if (tempBee.quality > newBee.quality){newBee = tempBee;}
        }
        if (newBee.quality > solutions[i].quality){solutions[i] = newBee;}
    }

}

void Bees::scouts_search() {
    for (int i=parameters_.bestCount; i != parameters_.solutionsNumber; i ++) {
        // generate generator
        std::mt19937 generator(seed_);
        seed_ = seed_ + generator();

        // generate random solutions
        Bee bee;
        bee.trams.gen_rand_trams(problemParameters_.stations, tram_amount, tram_length, depot_, generator);
        bee.quality = calculateFitness(bee.trams);
        solutions[i] = bee;
    }
}



float Bees::calculateFitness(TramList trams) {
    // Calculate the fitness (quality) of a Trams
    std::tuple<float, uint32_t> objective = problemParameters_.tramProblem.run(trams);

    switch (problemParameters_.problemCriterion) {
        case max_transported:
            return std::get<0>(objective);
        case max_distance:
            return float(std::get<1>(objective));
        case CRITERION_NR_ITEMS:
            break;
    }
}

ProblemParameters::ProblemParameters(const int i, Graph<struct Point2D> graph, TramProblem problem, StationList list,
                                     criterion criterion1) :tramCount(i), stations(std::move(graph)), tramProblem(std::move(problem)),
                                                            stationList(std::move(list)), problemCriterion(criterion1){}
