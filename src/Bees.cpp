#include "Bees.hpp"

#include <utility>
#include <limits>
#include <chrono>

std::string_view CriterionToString(criterion c)
{
    switch (c)
    {
        case max_transported:
            return "Maksymalny przewóz";
        case max_distance:
            return "Maksymalny dystans";
        case most_efficient:
            return "Największa efetywność";
        case CRITERION_NR_ITEMS:
            break;
    }
    return "";
}

std::string_view NeighborhoodToString(Neighborhood neighborhood)
{
    switch (neighborhood)
    {
        case RANDOM_TRAM_RANDOM_PATH:
            return "Losowy tramwaj, losowa trasa";
        case RANDOM_TRAM_RANDOM_AFTER:
            return "Losowy tramwaj, losowa zmiana końca trasy";
        case NEIGHBORHOOD_NR_ITEMS:
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
        bee.trams.gen_rand_unique(problemParameters_.stations, tram_amount, tram_length, depot_, generator_);
        bee.quality = calculateFitness(bee.trams);
        bee.age = 0;
        solutions.push_back(bee);
    }

    std::sort(solutions.begin(), solutions.end(), std::greater<>());
    best_bee_ = solutions[0];
    bestValueIteration_.push_back(best_bee_.quality);
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

        bestValueIteration_.push_back(best_bee_.quality);

        // For debug only
        // bestValueIteration_.push_back(solutions[0].quality);
    }
    return best_bee_;
}

void Bees::elites_search() {
    for (int i=0; i != parameters_.eliteCount; i ++)
    {
        Bee &newBee = solutions[i];

        auto &vec = localBees_[newBee];
        bool has_changed_bee = false;

        if (vec.empty())
        {
            vec.reserve(parameters_.eliteCount);
            for (auto z : std::views::iota(0, parameters_.eliteCount))
                vec.push_back(CreatLocalBee(newBee));

        }

        for (auto &localBee: vec)
        {
            if (localBee.distance_from_original > parameters_.neighborhoodSize)
                localBee = CreatLocalBee(newBee);


            if (problemParameters_.neighborhood == RANDOM_TRAM_RANDOM_PATH)
            {
                localBee.trams.deleteTram(generator_() % tram_amount);
                localBee.trams.gen_rand_unique(problemParameters_.stations, 1, tram_length, depot_, generator_);
                localBee.quality = calculateFitness(localBee.trams);
                localBee.distance_from_original += 1;
            }
            else if (problemParameters_.neighborhood == RANDOM_TRAM_RANDOM_AFTER)
            {
                localBee.trams.gen_rand_after(problemParameters_.stations, problemParameters_.neighborhood, tram_length,
                                              depot_, generator_);
                localBee.quality = calculateFitness(localBee.trams);
                // dystans nie ma znaczenia, nie będzie większy niż rozmiar otoczenia
            }

            if (localBee.quality > newBee.quality)
            {
                newBee.trams = localBee.trams;
                newBee.quality = localBee.quality;
                newBee.age = 0;
                has_changed_bee = true;
            }
        }

        if (has_changed_bee)
        {
            auto localBee = CreatLocalBee(newBee);
            std::fill(vec.begin(), vec.end(), localBee);
        }
    }

//    for (int i=0; i != parameters_.eliteCount; i ++) {
//        Bee newBee(solutions[i]);
//
//        for (int j=0; j != parameters_.eliteRecruits; j ++) {
//            Bee tempBee(solutions[i]);
//
//            tempBee.trams.deleteTram(generator_() % tram_amount);
//            tempBee.trams.gen_rand_unique(problemParameters_.stations, 1, tram_length, depot_, generator_);
//            tempBee.quality = calculateFitness(tempBee.trams);
//            tempBee.age = 0;
//
//            if (tempBee.quality > newBee.quality){
//                newBee = tempBee;
//            }
//        }
//        if (newBee.quality > solutions[i].quality){
//            solutions[i] = newBee;
//        }
//    }
}

void Bees::best_search() {
    for (int i=parameters_.eliteCount; i != parameters_.bestCount; i ++)
    {
        Bee &newBee = solutions[i];

        auto &vec = localBees_[newBee];

        bool has_changed_bee = false;

        if (vec.empty())
        {
            vec.resize(parameters_.bestCount);
            std::fill(vec.begin(), vec.end(), CreatLocalBee(newBee));
        }

        for (auto &localBee : vec)
        {
            if (localBee.distance_from_original > parameters_.neighborhoodSize)
                localBee = CreatLocalBee(newBee);

            localBee.trams.deleteTram(generator_() % tram_amount);
            localBee.trams.gen_rand_unique(problemParameters_.stations, 1, tram_length, depot_, generator_);
            localBee.quality = calculateFitness(localBee.trams);
            localBee.distance_from_original += 1;

            if (localBee.quality > newBee.quality)
            {
                newBee.trams = localBee.trams;
                newBee.quality = localBee.quality;
                newBee.age = 0;
                has_changed_bee = true;
            }
        }

        if (has_changed_bee)
            std::fill(vec.begin(), vec.end(), CreatLocalBee(newBee));
    }

//        for (int j=0; j != parameters_.eliteRecruits; j ++) {
//            BeeLocal tempBee = CreatLocalBee(newBee);
//
//
//            tempBee.trams.deleteTram(generator_() % tram_amount);
//            tempBee.trams.gen_rand_unique(problemParameters_.stations, 1, tram_length, depot_, generator_);
//            tempBee.quality = calculateFitness(tempBee.trams);
//            tempBee.distance_from_original += 1;
//
//

//    }
//    for (int i=parameters_.eliteCount; i != parameters_.bestCount; i ++) {
//        Bee newBee(solutions[i]);
//
//        for (int j=0; j != parameters_.bestRecruits; j ++) {
//            Bee tempBee(solutions[i]);
//
//            tempBee.trams.deleteTram(generator_() % tram_amount);
//            tempBee.trams.gen_rand_unique(problemParameters_.stations, 1, tram_length, depot_, generator_);
//            tempBee.quality = calculateFitness(tempBee.trams);
//            tempBee.age = 0;
//
//            if (tempBee.quality > newBee.quality){newBee = tempBee;}
//        }
//        if (newBee.quality > solutions[i].quality){solutions[i] = newBee;}
//    }
}


void Bees::scouts_search() {
    for (int i=parameters_.bestCount; i != parameters_.solutionsNumber; i ++) {
        Bee &bee = solutions[i];
        bee.trams.gen_rand_unique(problemParameters_.stations, tram_amount, tram_length, depot_, generator_);
        bee.quality = calculateFitness(bee.trams);
        bee.age = 0;
        solutions[i] = bee;
    }
}



float Bees::calculateFitness(TramList trams) {
    objectiveFunCalls++;
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
    for (int i=0; i != parameters_.bestCount; i ++) {
        Bee &bee = solutions[i];

        if (bee.age > parameters_.beeLifeTime){
            localBees_[bee].clear();
            bee.trams.gen_rand_unique(problemParameters_.stations, tram_amount, tram_length, depot_, generator_);
            bee.quality = calculateFitness(bee.trams);
            bee.age = 0;
        }
        else
            bee.age ++;
    }
}

ProblemParameters::ProblemParameters(const int i, Graph<struct Point2D> graph, TramProblem problem, StationList list,
                                     criterion criterion1, Neighborhood neighborhood)
                                     :tramCount(i), stations(std::move(graph)), tramProblem(std::move(problem)),
                                        stationList(std::move(list)), problemCriterion(criterion1),
                                        neighborhood{neighborhood}
{
}

Bee::Bee(const TramList& trams, double quality, uint8_t age): trams{trams}, quality{quality}, age{age}
{
    id = all_id;
    ++all_id;
}

BeeLocal CreatLocalBee(const Bee& bee)
{
    return {
        .trams = bee.trams,
        .quality = bee.quality,
        .distance_from_original = 0
    };
}
