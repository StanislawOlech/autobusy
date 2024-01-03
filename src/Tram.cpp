#include "Tram.hpp"
#include <iostream>
#define bad_station {-1000, -1000}
#include "Settings.hpp"

Point2D Tram::stop() {
    /**
     * Function makes tram travel to next point
     *
     * @return Point where the train was before.
     */
     Point2D station = *std::next(path.begin(), position);
    if (position+1<path.size() && !reverse){
        position++;
    }
    else if (!reverse){
        reverse = !reverse;
        position --;
    }
    else if (position-1>=0){
        position --;
    }
    else {
        reverse = !reverse;
        position ++;
    }

    return station;
}

Point2D Tram::peek_next(int index) {
    /**
     * Function to see where the tram would be in index time
     *
     * The function takes into account if the tram would turn back
     *
     * @param index relative time
     * @return point where the train would be
     */
    if (position+index<path.size() && !reverse){
        return *std::next(path.begin(), position + index);
    }
    else if (!reverse){
        int old_position = position;
        int pos_after_rev = position + index - (int(path.size()) - 1);
        reverse = ! reverse;
        position = int(path.size()) - 1;
        Point2D ans = peek_next(pos_after_rev);
        reverse = ! reverse;
        position = old_position;
        return ans;
    }
    else if (position-index>=0){
        return *std::next(path.begin(), position - index);
    }
    else {
        int old_position = position;
        int pos_after_rev = index - position - (0);
        reverse = ! reverse;
        position = 0;
        Point2D ans = peek_next(pos_after_rev);
        reverse = ! reverse;
        position = old_position;
        return ans;
    }
}

void Tram::add_stop(Point2D next_stop) {
    if (path.empty()){path.push_back(next_stop);}

    uint8_t dist = NormL1(next_stop, path[path.size()-1]);

    if (dist == 0){return;}

    for (uint8_t x = 1; x != dist; x++){
        path.push_back(bad_station);
    }
    path.push_back(next_stop);
}

void Tram::DebugPrint() const
{
    std::cout << "Path: ";
    for (auto point2D : path)
        std::cout << point2D << ", ";
    std::cout << "\nPosition = " << position << ", Reverse: "  << reverse << std::endl;
}


void TramList::gen_rand_trams(const Graph<Point2D>& graph, int tram_amount, int tram_length, Point2D depot){
    std::mt19937 generator(random_tram_seed);
    gen_rand_trams(graph, tram_amount, tram_length, depot, generator);
}


void TramList::gen_rand_trams(const Graph<Point2D>& graph, int tram_amount, int tram_length, Point2D depot,
                              std::mt19937& generator) {
    /**
     * Function to random trams
     *
     * @param graph graph to see possible connections
     * @param tram_amount number of trams to generate
     * @param tram_length length of maximum tram length
     * @param depot position of depot (depot must be in tram path)
     *
     */

    // TODO - erase repeating stations, use random_shuffle with std::iota
    for (int i = 0; i != tram_amount; i++){
        Tram tram;
        tram.add_stop(depot);
        Point2D last = depot;
        for (int j = 0; j != tram_length; j ++){
            auto it = graph.GetEdge(last);

            if (it == nullptr){break;}

            auto neighbour = *it;
            uint32_t next = uint32_t(generator() % neighbour.size());
            last = neighbour[next];
            tram.add_stop(last);
        }

//        tram.set_start_point(abs(int(generator())) % (tram_length));
//        tram.set_direction(bool(generator() % 2));

        tram.set_start_point(0); // Must always start from depot = Point2D{0,0}
        tram.set_direction(false); // Always start going forward

        trams.push_back(tram);
    }
}

std::tuple<uint32_t, uint32_t, uint32_t> TramList::stop(StationList& stationList){
    /**
     * Function to determinate number of passengers and update number of people at every stop
     *
     * @param stationList list of stations
     * @return number of transported passengers, total traveled traveled
     */
    uint32_t transported = 0;
    uint32_t traveled    = 0;
    uint32_t distance    = 0;

    for (auto tram : trams){
        Point2D current_point = tram.peek_next(0);

        if (NormL1(current_point, bad_station) == 0){continue;}

        Station curentStat = stationList.Get(current_point);

        for (int i = 1; i < longest_voyage + 1; i++){
            Point2D dest = tram.peek_next(i);

            if (NormL1(dest, bad_station) == 0){continue;}

            uint32_t people_count = stationList.Get(current_point).GetPassengers(dest).count;

            transported += people_count;
            traveled    += people_count * (i);
            distance    += people_count * NormL1(current_point, dest);


            stationList.delatePassengers(current_point, dest);

            //std::cout << "z dworca: " << tram.peek_next(0) << " zniknelo: " << people_count << " jadacych do: " << dest << std::endl;

            std::tuple<uint32_t, uint32_t, uint32_t> objective = this->transfers(stationList, i, dest, current_point);
            transported += std::get<0>(objective);
            traveled    += std::get<1>(objective);
            distance    += std::get<2>(objective);

            //std::cout << stationList.Get(current_point).GetPassengers(dest).count;
        }
    }
    this->update();
    return {transported, traveled, distance};
}

std::tuple<uint32_t, uint32_t, uint32_t> TramList::transfers(StationList& stationList, int travel_time, Point2D trans_station, Point2D orginalpoint){
    /**
     * Function to determinate number of passangers that will transfer at  trans_station
     *
     * @param stationList list of stations
     * @param traveled distance already traveled
     * @param trans_station point of transfer
     * @param orginalStation orginal station
     * @return number of transported passengers, total distance traveled
     */
    uint32_t transported = 0;
    uint32_t traveled    = 0;
    uint32_t distance    = 0;


    for (auto tram : trams){

        for (int i = travel_time; i < longest_voyage + 1; i++){
            Point2D first_point = tram.peek_next(i);
            if (first_point != trans_station){continue;}

            for (int j = 1 + i; j < longest_voyage + 1;  j++) {
                Point2D dest = tram.peek_next(j);

                if (NormL1(dest, bad_station) == 0){continue;}

                uint32_t people_count = stationList.Get(orginalpoint).GetPassengers(dest).count;
                transported += people_count;
                traveled    += people_count * (j);
                distance    += people_count * NormL1(orginalpoint, dest);
                stationList.delatePassengers(orginalpoint, dest);

                //std::cout << orginalStation.GetPassengers(dest).count << std::endl;
                //std::cout << "z dworca: " << orginalStation << " zniknelo: " << people_count << " jadacych do: " << dest << " przez " << trans_station << std::endl;

            }
            break;
        }
    }
    return {transported, traveled, distance};
}

void TramList::deleteTram(std::size_t position){
    auto it = trams.begin();
    std::advance(it, position);
    trams.erase(it);
}

void TramList::update() {
    for (auto & tram : trams) {
        tram.stop();
    }
}

void TramList::DebugPrint() const
{
    for (const auto &tram : trams)
        tram.DebugPrint();
}






