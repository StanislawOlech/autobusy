#include "Station.hpp"



void Station::DeletePassengers(Point2D dest)
{
    passengers_[dest] = 0;
}

Passenger Station::GetPassengers(Point2D dest)
{
    return {position_, dest, passengers_[dest]};
}

void Station::Update()
{
    std::span<Passenger> new_passengers = UpdatePassengers_(position_);

    for (auto [start, end, count] : new_passengers)
        passengers_[end] += count;
}
