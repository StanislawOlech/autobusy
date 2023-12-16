#include "Tram.hpp"
//#include <iostream>



Point2D Tram::stop() {
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


void TramList::gen_rand_trams(Graph<Point2D> graph, int tram_amount, int tram_length, Point2D depot){
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
            int next = generator() % neighbour.size();
            last = neighbour[next];
            tram.add_stop(last);
        }
        trams.push_back(tram);
        tram.set_start_point(generator() % tram_length);
    }
}


std::tuple<uint32_t, uint32_t> TramList::stop(StationList& stationList){
    uint32_t transported = 0;
    uint32_t distance    = 0;

    for (auto tram : trams){
        Station curentStat = stationList.Get(tram.peek_next(0));

        for (int i = 1; i < longest_voyage + 1; i++){
            Point2D dest = tram.peek_next(i);
            uint32_t people_count = curentStat.GetPassengers(dest).count;
            transported += people_count;
            distance += people_count * (i);
            curentStat.DeletePassengers(dest);

            //std::cout << "z dworca: " << tram.peek_next(0) << " zniknelo: " << people_count << " jadacych do: " << dest << std::endl;


            std::tuple<uint32_t, uint32_t> objective = this->transfers(stationList, i, dest, curentStat);
            transported += std::get<0>(objective);
            distance += std::get<1>(objective);
        }
    }
    this->update();
    return {transported, distance};
}

std::tuple<uint32_t, uint32_t> TramList::transfers(StationList& stationList, int traveled, Point2D trans_station, Station orginalStation){
    uint32_t transported = 0;
    uint32_t distance    = 0;

    for (auto tram : trams){

        for (int i = traveled; i < longest_voyage + 1; i++){
            Point2D first_point = tram.peek_next(i);
            if (first_point != trans_station){continue;}

            for (int j = 1 + i; j < longest_voyage + 1; j++) {
                Point2D dest = tram.peek_next(j);
                uint32_t people_count = orginalStation.GetPassengers(dest).count;
                transported += people_count;
                distance += people_count * (j);
                orginalStation.DeletePassengers(dest);

                //std::cout << "z dworca: " << orginalStation << " zniknelo: " << people_count << " jadacych do: " << dest << " przez " << trans_station << std::endl;

            }
        }
    }
    return {transported, distance};
}

void TramList::update() {
    for (auto it = trams.begin(); it != trams.end(); ++it) {
        (*it).stop();
    }
}
