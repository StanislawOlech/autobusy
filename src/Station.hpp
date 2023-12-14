#ifndef AUTOBUS_STATION_HPP
#define AUTOBUS_STATION_HPP

#include "Graph.hpp"

#include <functional>
#include <utility>
#include <optional>
#include <random>
#include <unordered_set>


/**
 * @param start - stacja startowa
 * @param end - stacja końcowa
 * @param count - liczba pasażerów
 */
struct Passenger
{
    Point2D start;
    Point2D end;
    uint32_t count{};
};

bool EqualStartEnd(Passenger passenger1, Passenger passenger2);

/**
 * @class
 * Stacja z pasażerami, na początku nie ma żadnych pasażerów
 * dodawanie pasażerów przez AddPassenger
 * dzielenie całkowitoliczbowe pozostałych pasażerów przez DividePassenger
 */
class Station
{
public:
    explicit Station(Point2D position);

    void DeletePassengers(Point2D dest);

    [[nodiscard]] Passenger GetPassengers(Point2D dest);

    [[nodiscard]] uint32_t CountPassengers() const;

    [[nodiscard]] bool HasPassengers(Point2D dest) { return passengers_[dest] != 0; }

    void DividePassengers(uint32_t divider);

    void AddPassengers(std::span<Passenger> new_passengers);

private:
    Point2D position_;
    std::unordered_map<Point2D, uint32_t> passengers_;
};


/** @class
 * Struktura pamięta nowych pasażerów w każdej turze dla każdego pasażera
 * Musi pamiętać czas
 * Zarządzana przez StationList
 */
struct PassengerTable
{
    using Table3D = std::unordered_map<Point2D, std::vector<std::vector<Passenger>>>;

    explicit PassengerTable(Table3D table): table_{std::move(table)} {}

    std::span<Passenger> operator()(Point2D station);

    [[nodiscard]] std::size_t size2D(Point2D station) const { return table_.at(station).size(); }

    void UpdateTime() { ++curr_time; }

private:
    Table3D table_;
    std::size_t curr_time = 0;
};


/** @class
 * Obiekt przechowujący informację o stacjach
 * Taki kontener tylko na sterydach
 */
class StationList
{
public:

    explicit StationList(PassengerTable::Table3D passengerTable, uint32_t divider = 2);

    void Create(Point2D position);

    bool Has(Point2D position) { return stations_.contains(position); }
    Station& Get(Point2D position)  { return stations_.at(position); }

    void Update();

private:
    std::unordered_map<Point2D, Station> stations_;

    PassengerTable passengerTable_;
    uint32_t passenger_divider_;
};

#endif //AUTOBUS_STATION_HPP
