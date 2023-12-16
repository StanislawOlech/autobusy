#include "Station.hpp"

bool EqualStartEnd(Passenger passenger1, Passenger passenger2)
{
    return passenger1.start == passenger2.start && passenger1.end == passenger2.end;
}


Station::Station(Point2D position): position_{position}, passengers_{}
{
}

void Station::DeletePassengers(Point2D dest)
{
    passengers_[dest] = 0;
}

Passenger Station::GetPassengers(Point2D dest)
{
    return {position_, dest, passengers_[dest]};
}

void Station::DividePassengers(uint32_t divider)
{
    for (auto &[end, count] : passengers_)
        count /= divider;
}

uint32_t Station::CountPassengers() const
{
    uint32_t sum = 0;
    for (auto [dest, count]: passengers_)
        sum += count;
    return sum;
}

void Station::AddPassengers(std::span<Passenger> new_passengers)
{
    if (new_passengers.empty())
        return;

    for (auto [start, end, count] : new_passengers)
        passengers_[end] += count;

}

std::span<Passenger> PassengerTable::operator()(Point2D station)
{
    if (!table_.contains(station))
        return {};

    auto &vec2d = table_[station];
    if (curr_time >= vec2d.size())
        return {};

    return {vec2d[curr_time]};
}

StationList::StationList(PassengerTable::Table3D passengerTable, uint32_t divider):
        stations_{}, passengerTable_{std::move(passengerTable)} , passenger_divider_{divider}
{

}

void StationList::Create(Point2D position)
{
    Station station{position};
    station.AddPassengers(passengerTable_(position));

    stations_.emplace(position, std::move(station));
}

void StationList::Update()
{
    passengerTable_.UpdateTime();

    for (auto &[point, station]: stations_)
    {
        station.DividePassengers(passenger_divider_);
        station.AddPassengers(passengerTable_(point));
    }
}

uint32_t StationList::GenerateRandomPass(uint32_t maxPassengers, uint32_t groups) {
    uint32_t passenger_number = 0;
    std::vector<Point2D> station_points;

    // retrieve all stations
    for (const auto& pair : stations_) {
        station_points.push_back(pair.first);
    }

    // generate passengers
    for (auto& pair : stations_) {
        std::vector<Passenger> new_passengers;

        for(uint32_t n = 0; n != groups; n++){
            Point2D dest = station_points[rand() % station_points.size()];
            uint32_t pass = rand() % maxPassengers;
            passenger_number += pass;
            Passenger passenger{pair.first, dest, pass};

            new_passengers.push_back(passenger);

        }
        //TODO ogarnąć dlaczego to nie działa
        //stations_[pair.first].AddPassengers(new_passengers);
    }
    return passenger_number;
}
