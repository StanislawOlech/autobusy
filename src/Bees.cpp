#include "Bees.hpp"

#include <utility>
#include <limits>

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
          AlgorithmParameters parameters, uint32_t seed,  Point2D depot, ProblemParameters problemParameters)
        : seed_(seed), depot_(depot), problemParameters_(std::move(problemParameters)), parameters_(parameters),
          generator_{seed_}
{
    tram_amount = problemParameters_.tramCount;
    solutions.reserve(parameters.solutionsNumber);

    // Initialize rest of solutions bees randomly
    for (int i=0; i != parameters.solutionsNumber; i ++) {
        // generate random solutions
        Bee bee;
        bee.trams.gen_rand_trams(problemParameters_.stations, tram_amount, tram_length, depot_, generator_);
        bee.quality = calculateFitness(bee.trams);
        bee.age = 0;
        solutions.push_back(bee);
    }

    std::sort(solutions.begin(), solutions.end(), std::greater<>());
    best_bee_ = solutions[0];
    bestValueIteration_.push_back(best_bee_.quality * 100.f);
}

Bee Bees::run() {
    for (int iteration = 0; iteration != parameters_.maxIterations; iteration++) {
        elites_search();

        best_search();

        scouts_search();

        age();

        std::sort(solutions.begin(), solutions.end(), std::greater<>());

        if (solutions[0].quality > best_bee_.quality)
            best_bee_ = solutions[0];

        // TODO - for final project do best of all time
//        bestValueIteration_.push_back(best_bee_.quality * 100.f);

        // TODO - delete for final project
        bestValueIteration_.push_back(solutions[0].quality * 100.f);
    }
    return best_bee_;
}

void Bees::elites_search() {
    for (int i=0; i != parameters_.eliteCount; i ++) {
        Bee newBee(solutions[i]);

        for (int j=0; j != parameters_.eliteRecruits; j ++) {
            Bee tempBee(solutions[i]);

            tempBee.trams.deleteTram(generator_() % tram_amount);
            tempBee.trams.gen_rand_trams(problemParameters_.stations, 1, tram_length, depot_, generator_);
            tempBee.quality = calculateFitness(tempBee.trams);
            tempBee.age = 0;

            if (tempBee.quality > newBee.quality){
                newBee = tempBee;
            }
        }
        if (newBee.quality > solutions[i].quality){
            solutions[i] = newBee;
        }
    }
}

void Bees::best_search() {
    for (int i=parameters_.eliteCount; i != parameters_.bestCount; i ++) {
        Bee newBee(solutions[i]);

        for (int j=0; j != parameters_.bestRecruits; j ++) {
            Bee tempBee(solutions[i]);

            tempBee.trams.deleteTram(generator_() % tram_amount);
            tempBee.trams.gen_rand_trams(problemParameters_.stations, 1, tram_length, depot_, generator_);
            tempBee.quality = calculateFitness(tempBee.trams);
            tempBee.age = 0;

            if (tempBee.quality > newBee.quality){newBee = tempBee;}
        }
        if (newBee.quality > solutions[i].quality){solutions[i] = newBee;}
    }

}

void Bees::scouts_search() {
    for (int i=parameters_.bestCount; i != parameters_.solutionsNumber; i ++) {

        // generate random solutions
        Bee bee;
        bee.trams.gen_rand_trams(problemParameters_.stations, tram_amount, tram_length, depot_, generator_);
        bee.quality = calculateFitness(bee.trams);
        bee.age = 0;
        solutions[i] = bee;
    }
}



float Bees::calculateFitness(TramList trams) {
    objectiveFunCalls ++;
    // Calculate the fitness (quality) of a Trams
    std::tuple<float, uint32_t, float> objective = problemParameters_.tramProblem.run(trams);

    switch (problemParameters_.problemCriterion) {
        case max_transported:
            return std::get<0>(objective);
        case max_distance:
            return float(std::get<1>(objective));
        case most_efficient:
            return std::get<2>(objective);
        case CRITERION_NR_ITEMS:
            break;
    }


    return std::numeric_limits<float>::min();
}

void Bees::age() {
    for (int i=0; i != parameters_.solutionsNumber; i ++) {
        if (solutions[i].age > parameters_.beeLifeTime){
            // generate random solutions
            Bee bee;
            bee.trams.gen_rand_trams(problemParameters_.stations, tram_amount, tram_length, depot_, generator_);
            bee.quality = calculateFitness(bee.trams);
            bee.age = 0;
            solutions[i] = bee;
        }
        solutions[i].age ++;
    }
}

ProblemParameters::ProblemParameters(const int i, Graph<struct Point2D> graph, TramProblem problem, StationList list,
                                     criterion criterion1)
                                     :tramCount(i), stations(std::move(graph)), tramProblem(std::move(problem)),
                                        stationList(std::move(list)), problemCriterion(criterion1)
{
}
