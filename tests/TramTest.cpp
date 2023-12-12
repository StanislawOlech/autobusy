#include <gtest/gtest.h>

#include "Tram.hpp"


TEST(TramTest, Make_tram)
{
    std::vector<Point2D> vec1 = {{1, 2}, {3, 1}, {5, 6}};
    Tram tram{vec1};
}
