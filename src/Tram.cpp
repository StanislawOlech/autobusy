#include "Tram.hpp"


Tram::Tram(std::vector<Point2D> _path) :path{std::move(_path)} {}


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
