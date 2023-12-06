#include "Graph.hpp"

#include <cmath>

double normL2(Point2D start, Point2D end)
{
    return sqrt(pow((start.x - end.x), 2) + pow((start.y - end.y), 2));
}

std::ostream &operator<<(std::ostream &oss, Point2D point)
{
    oss << "{" << point.x << ", " << point.y << "}";
    return oss;
}
