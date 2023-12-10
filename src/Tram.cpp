#include "Tram.hpp"


Tram::Tram(std::list<Point2D> _path) :path{std::move(_path)}, size{static_cast<int>(_path.size())} {}


Point2D Tram::stop() {
    if (positon+1<size && !reverse){
        positon++;
    }
    else if (!reverse){
        reverse == !reverse;
        positon --;
    }
    else if (positon-1>=0 && reverse){
        positon --;
    }
    else {
        reverse == !reverse;
        positon ++;
    }

    return peek_next(-1);
}

Point2D Tram::peek_next(int index) const {
    if (positon+index<size && !reverse){
        return *std::next(path.begin(), positon + index);
    }
    else if (!reverse){
        int pos_after_rev = (size - 1 - (positon + index - (size - 1)));
        return peek_next(pos_after_rev - positon);
    }
    else if (positon-index>=0 && reverse){
        return *std::next(path.begin(), positon - index);
    }
    else {
        int pos_after_rev = (0 - (positon - index - 0));
        return peek_next(pos_after_rev - positon);
    }
}
