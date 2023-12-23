#include "Tram.hpp"
#include <iostream>
#define bad_station {-1000, -1000}


Point2D Tram::stop() {
    /**
     * Function makes tram travel to next point
     *
     * @return Point where the train was before.
     */
    if (positon+1<path.size() && !reverse){
        positon++;
    }
    else if (!reverse){
        reverse = !reverse;
        positon --;
    }
    else if (positon-1>=0){
        positon --;
    }
    else {
        reverse = !reverse;
        positon ++;
    }

    return peek_next(-1);
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
    if (positon+index<path.size() && !reverse){
        return *std::next(path.begin(), positon + index);
    }
    else if (!reverse){
        int pos_after_rev = (int(path.size()) - 1 - (positon + index - (int(path.size()) - 1)));
        reverse = ! reverse;
        Point2D ans = peek_next(pos_after_rev - positon);
        reverse = ! reverse;
        return ans;
    }
    else if (positon-index>=0){
        return *std::next(path.begin(), positon - index);
    }
    else {
        int pos_after_rev = (0 - (positon - index - 0));
        reverse = ! reverse;
        Point2D ans = peek_next(pos_after_rev - positon);
        reverse = ! reverse;
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


void TramList::gen_rand_trams(const Graph<Point2D>& graph, int tram_amount, int tram_length, Point2D depot){
    /**
     * Function to random trams
     *
     * @param graph graph to see possible connections
     * @param tram_amount number of trams to generate
     * @param tram_length length of maximum tram length
     * @param depot position of depot (depot must be in tram path)
     *
     */
    std::random_device rd;
    std::mt19937 generator{seed};

    for (int i = 0; i != tram_amount; i++){

        Tram tram;
        tram.add_stop(depot);
        Point2D last = depot;
        for (int j = 0; j != tram_length; j ++){
            auto it = graph.GetEdge(last);

            if (it == nullptr){break;}

            auto neighbour = *it;
            uint32_t next = int(generator()) % neighbour.size();
            last = neighbour[next];
            tram.add_stop(last);
        }
        trams.push_back(tram);
        tram.set_start_point(int(generator()) % tram_length);
    }
}


std::tuple<uint32_t, uint32_t> TramList::stop(StationList& stationList){
    /**
     * Function to determinate number of passengers and update number of people at every stop
     *
     * @param stationList list of stations
     * @return number of transported passengers, total distance traveled
     */
    uint32_t transported = 0;
    uint32_t distance    = 0;

    for (auto tram : trams){
        Point2D current_point = tram.peek_next(0);
        if (NormL1(current_point, bad_station) == 0){continue;}
        Station curentStat = stationList.Get(current_point);

        for (int i = 1; i < longest_voyage + 1; i++){
            Point2D dest = tram.peek_next(i);
            uint32_t people_count = curentStat.GetPassengers(dest).count;
            transported += people_count;
            distance += people_count * (i);
            stationList.delatePassengers(current_point, dest);

            //std::cout << "z dworca: " << tram.peek_next(0) << " zniknelo: " << people_count << " jadacych do: " << dest << std::endl;

            std::tuple<uint32_t, uint32_t> objective = this->transfers(stationList, i, dest, current_point);
            transported += std::get<0>(objective);
            distance += std::get<1>(objective);


            //std::cout << stationList.Get(current_point).GetPassengers(dest).count;
        }
    }
    this->update();
    return {transported, distance};
}

std::tuple<uint32_t, uint32_t> TramList::transfers(StationList& stationList, int traveled, Point2D trans_station, Point2D orginalpoint){
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
    uint32_t distance    = 0;


    for (auto tram : trams){

        for (int i = traveled; i < longest_voyage + 1; i++){
            Point2D first_point = tram.peek_next(i);
            if (first_point != trans_station){continue;}

            for (int j = 1 + i; j < longest_voyage + 1; j++) {
                Station orginalStation = stationList.Get(orginalpoint);
                Point2D dest = tram.peek_next(j);
                uint32_t people_count = orginalStation.GetPassengers(dest).count;
                transported += people_count;
                distance += people_count * (j);
                stationList.delatePassengers(orginalpoint, dest);

                //std::cout << orginalStation.GetPassengers(dest).count << std::endl;
                //std::cout << "z dworca: " << orginalStation << " zniknelo: " << people_count << " jadacych do: " << dest << " przez " << trans_station << std::endl;

            }
        }
    }
    return {transported, distance};
}

void TramList::update() {
    for (auto & tram : trams) {
        tram.stop();
    }
}


std::tuple<uint32_t, uint32_t, uint32_t> execute_path(int time, StationList stationList, TramList trams){
    // objective function params
    uint32_t transported      = 0;
    uint32_t all_passengers   = 0;
    uint32_t distance         = 0;

    for (int t = 0; t != time; t++){

        // populating tram stops
        all_passengers += stationList.GenerateRandomPass();

        // packing people on trams and ride them to next stops
        std::tuple<uint32_t, uint32_t> objective = trams.stop(stationList);

        // objective function update
        transported += std::get<0>(objective);
        distance    += std::get<1>(objective);

        stationList.Update();
    }

    return {transported, distance, all_passengers};
}

