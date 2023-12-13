#include "Station.hpp"


Station::Station(Point2D position, const AddF& add_f, const DeleteF& deleteF):
                position_{position}, passengers_{}, AddPassengers_{add_f}, DeletePassengers_{deleteF}
{
    Update();
}

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
    auto loss_rate = DeletePassengers_();

    for (auto &[end, count] : passengers_)
    {
        count /= loss_rate;
    }

    std::span<Passenger> new_passengers = AddPassengers_(position_);

    for (auto [start, end, count] : new_passengers)
        passengers_[end] += count;
}

uint32_t Station::CountPassengers() const
{
    uint32_t sum = 0;
    for (auto [dest, count]: passengers_)
        sum += count;
    return sum;
}

std::span<Passenger> PassengerTable::operator()(Point2D station)
{
    auto &vec2d = table_[station];
    if (curr_time >= vec2d.size())
        return {};
    return {vec2d[curr_time]};
}

void StationList::Update()
{
    for (auto &[point, station]: stations_)
        station.Update();

    passengerTable_.UpdateTime();
}
