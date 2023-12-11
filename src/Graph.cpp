#include "Graph.hpp"


std::ostream &operator<<(std::ostream &oss, Point2D point)
{
    oss << "{" << point.x << ", " << point.y << "}";
    return oss;
}
