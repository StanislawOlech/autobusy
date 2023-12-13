#include <gtest/gtest.h>

#include "Station.hpp"


TEST(StationTest, GeneralUse)
{
    constexpr Point2D start = {0, 0};

    constexpr Point2D dest1 = {10, -14};
    constexpr Point2D dest2 = {-15, 10};

    constexpr Passenger passenger1{start, dest1, 5};
    constexpr Passenger passenger2{start, dest2, 1};

    std::vector<Passenger> added_passengers = {passenger1, passenger2};

    Station::AddF addF = [&added_passengers](Point2D){ return std::span<Passenger>{added_passengers};};
    Station::DeleteF deleteF = [](){ return 2; };
// ====

    Station station{start, addF, deleteF};

    ASSERT_EQ(station.CountPassengers(), 6);

    ASSERT_TRUE(station.GetPassengers(dest1).count == 5);
    ASSERT_TRUE(station.GetPassengers(dest2).count == 1);

    station.Update();

    ASSERT_TRUE(station.GetPassengers(dest1).count == 5/2 + 5);
    ASSERT_TRUE(station.GetPassengers(dest2).count == 1/2 + 1);
}

