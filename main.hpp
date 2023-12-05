#ifndef AUTOBUS_MAIN_HPP
#define AUTOBUS_MAIN_HPP

struct Point2D
{
    int start;
    int end;
};


template <typename T, std::size_t DEPTH, std::size_t COL, std::size_t ROW>
using Matrix3D = std::array<std::array<std::array<T, ROW>, COL>, DEPTH>;

class Tram{
public:
    Tram(std::list<Point2D> _path);
    Point2D peek_next(int index) const;
    Point2D stop();
    void turn_back(){reverse = !reverse;};
private:
    std::list<Point2D> path;
    bool reverse = false;
    int positon = 0;
    int size;
};

#endif //AUTOBUS_MAIN_HPP
