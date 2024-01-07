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

    uint32_t passenger_divider = 2;
// ====

    Station station{start};
    station.AddPassengers(added_passengers);

    ASSERT_EQ(station.CountPassengers(), 6);

    ASSERT_TRUE(station.GetPassengers(dest1).count == 5);
    ASSERT_TRUE(station.GetPassengers(dest2).count == 1);

    station.DividePassengers(passenger_divider);
    station.AddPassengers(added_passengers);

    ASSERT_TRUE(station.GetPassengers(dest1).count == 5/2 + 5);
    ASSERT_TRUE(station.GetPassengers(dest2).count == 1/2 + 1);
}

TEST(PassengerTableTest, GeneralUse)
{
    constexpr Point2D start = {0, 0};

    constexpr Point2D dest1 = {10, -14};
    constexpr Point2D dest2 = {-15, 10};

    constexpr Passenger passenger1{start, dest1, 5};
    constexpr Passenger passenger2{start, dest2, 1};

    std::vector<Passenger> vec1 = {passenger1, passenger2};

    constexpr Passenger passenger3{start, dest1, 1};
    constexpr Passenger passenger4{start, dest2, 9};

    std::vector<Passenger> vec2 = {passenger3, passenger4};

    std::vector<std::vector<Passenger>> vec2D = {vec1, vec2};

    PassengerTable::Table3D table3D = {
            {start, { vec1, vec2} }
    };

    PassengerTable passengerTable{table3D};
    ASSERT_EQ(passengerTable.size2D(start), 2);

    for (std::size_t i = 0; i < passengerTable.size2D(start); ++i)
    {
        std::span<Passenger> span = passengerTable(start);
        ASSERT_EQ(span.size(), vec2D[i].size());

        for (std::size_t j = 0; j < span.size(); ++j)
        {
            /// EqualStartEnd - ten sam start i koniec dla passengers
            ASSERT_TRUE(EqualStartEnd(span[j], vec2D[i][j]));
            ASSERT_EQ(span[j].count, vec2D[i][j].count);
        }
        passengerTable.UpdateTime();
    }

    /// Po wykorzystaniu wszystkich danych dla danej stacji zwraca pusty kontener
    ASSERT_TRUE(passengerTable(start).empty());
}

//TEST(StationListTest, GeneralUse)
//{
//    constexpr Point2D start = {0, 0};
//
//    constexpr Point2D dest1 = {10, -14};
//    constexpr Point2D dest2 = {-15, 10};
//
//    constexpr Passenger passenger1{start, dest1, 5};
//    constexpr Passenger passenger2{start, dest2, 1};
//
//    std::vector<Passenger> vec1 = {passenger1, passenger2};
//
//    constexpr Passenger passenger3{start, dest1, 1};
//    constexpr Passenger passenger4{start, dest2, 9};
//
//    std::vector<Passenger> vec2 = {passenger3, passenger4};
//
//    std::vector<std::vector<Passenger>> vec2D = {vec1, vec2};
//
//    PassengerTable::Table3D table3D = {
//            {start, { vec1, vec2} }
//    };
//
//    // =======================
//
//    StationList stationList{table3D};
//
//    Point2D start2 = {2, 3};
//
//    stationList.Create(start);
//    stationList.Create(start2);
//
//    ASSERT_TRUE(stationList.Has(start));
//    ASSERT_TRUE(stationList.Has(start2));
//
//    EXPECT_EQ(stationList.Get(start).CountPassengers(), passenger1.count + passenger2.count);
//    EXPECT_EQ(stationList.Get(start2).CountPassengers(), 0);
//
//    stationList.Update();
//
//    ASSERT_EQ(stationList.Get(start).CountPassengers(),
//              passenger1.count / 2 + passenger2.count / 2 + passenger3.count + passenger4.count);
//    ASSERT_EQ(stationList.Get(start2).CountPassengers(), 0);
//
//}
