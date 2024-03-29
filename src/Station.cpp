#include "Station.hpp"

#include <iostream>


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
    {
        passengers_[end] += count;

//        std::cout << "[Added] Start: " << start << " end: " << end << ", count=" << count << "\n";
    }
}

std::ostream& operator<<(std::ostream& oss, Station station) {
    oss << station.position_;
    return oss;
}

void Station::DebugPrint() const
{
    std::cout << "Pos: " << position_ << ", passengers:\n";
    for (auto &[dest, count] : passengers_)
        std::cout << "\tDest " << dest << ", count=" << count << "\n";
}

std::vector<Point2D> Station::GetPassengersDestination() const
{
    std::vector<Point2D> vec;

    std::transform(std::begin(passengers_), std::end(passengers_), std::back_inserter(vec),
                   [](auto pair){ return pair.first;});

    return std::move(vec);
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

void PassengerTable::DebugPrint() const
{
    std::cout << "PassengerTable\n";
    std::cout << "--------------------------\n";
    std::cout << "Current time: " << curr_time << "\n";
    for (auto &[point_start, vec2d] : table_)
    {
        int on_time = 0;
        for (auto &vec : vec2d)
        {
            for (auto [start, end, count] : vec)
                std::cout << "Time: " << on_time << " start: " << start << ", end: " << end << ", count: " << count << "\n";

            std::cout << "\n";
            ++on_time;
        }
    }
    std::cout << "--------------------------" << std::endl;
}

StationList::StationList(const PassengerTable::Table3D &passengerTable, uint32_t divider):
        stations_{}, passengerTable_{passengerTable} , passenger_divider_{divider}
{
    passenger_count_ = 0;
    for (auto &[point_start, vec2d] : passengerTable)
    {
        for (auto &vec : vec2d)
        {
            for (auto [start, end, count] : vec)
            {
                passenger_count_ += count;
            }
        }
    }

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

uint32_t StationList::GenerateRandomPass(std::mt19937 generator, uint32_t maxPassengers, uint32_t groups) {
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
            Point2D dest = station_points[generator() % station_points.size()];
            uint32_t pass = (dest != pair.first)? generator() % maxPassengers: 0;
            passenger_number += pass;
            Passenger passenger{pair.first, dest, pass};

            new_passengers.push_back(passenger);

        }
        stations_.at(pair.first).AddPassengers(new_passengers);
    }
    return passenger_number;
}

void StationList::Clear() {
    for (auto& pair : stations_){
        pair.second.Clear();
    }
}

void StationList::Restart()
{
//    std::cout << "Station List restart" << std::endl;

    passengerTable_.RestartTime();
    for (auto &[point, station]: stations_)
    {
        station.DividePassengers(passenger_divider_);
        station.AddPassengers(passengerTable_(point));
    }
}

void StationList::DebugPrint() const
{
    passengerTable_.DebugPrint();

    std::cout << "======\n";
    std::cout << "Passengers on stations" << std::endl;
    for (const auto&[pos, station] : stations_)
        station.DebugPrint();
    std::cout << "======\n";
}
