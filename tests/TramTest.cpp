#include <gtest/gtest.h>

#include "Tram.hpp"


TEST(TramTest, Make_tram)
{
    std::vector<Point2D> vec1 = {{1, 2}, {3, 1}, {5, 6}};
    Tram tram{vec1};
}

TEST(TramTest, stop_tram)
{
    std::vector<Point2D> vec1 = {{1, 2}, {3, 1}, {5, 6}};
    Tram tram{vec1};
    tram.stop();
}

TEST(TramTest, peek_tram1)
{
    std::vector<Point2D> vec1 = {{1, 2}, {3, 1}, {5, 6}};
    Tram tram{vec1};
    ASSERT_EQ(tram.peek_next(1), Point2D({3, 1}));
}

TEST(TramTest, peek_tram2)
{
    std::vector<Point2D> vec1 = {{1, 2}, {3, 1}, {5, 6}};
    Tram tram{vec1};
    ASSERT_EQ(tram.peek_next(3), Point2D({3, 1}));
}

TEST(TramTest, peek_tram3)
{
    std::vector<Point2D> vec1 = {{1, 2}, {3, 1}, {5, 6}};
    Tram tram{vec1};
    ASSERT_EQ(tram.peek_next(5), Point2D({3, 1}));
}

TEST(TramTest, peek_tram4)
{
    std::vector<Point2D> vec1 = {{1, 2}, {3, 1}, {5, 6}};
    Tram tram{vec1};
    ASSERT_EQ(tram.peek_next(6), Point2D({5, 6}));
}
