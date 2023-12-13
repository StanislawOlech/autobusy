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


/**
 * @class
 * Stacja a pasażerami, przyjmuje funkcję do dodawania nowych pasażerów
 */
class Station
{
public:
    using AddF = std::function<std::span<Passenger>(Point2D)>;
    using DeleteF = std::function<uint32_t(void)>; // Dzielnik liczb pozostałych pasażerów


    Station(Point2D position, const AddF& add_f, const DeleteF& deleteF);

    void DeletePassengers(Point2D dest);

    [[nodiscard]] Passenger GetPassengers(Point2D dest);

    [[nodiscard]] uint32_t CountPassengers() const;

    [[nodiscard]] bool HasPassengers(Point2D dest) { return passengers_[dest] != 0; }

    void Update();

private:
    Point2D position_;
    std::unordered_map<Point2D, uint32_t> passengers_;

    const AddF& AddPassengers_;
    const DeleteF& DeletePassengers_;
};


/** @class
 * Struktura pamięta nowych pasażerów w każdej turze dla każdego pasażera
 */
struct PassengerTable
{
    using Table3D = std::unordered_map<Point2D, std::vector<std::vector<Passenger>>>;

    explicit PassengerTable(Table3D table): table_{std::move(table)} {}

    std::span<Passenger> operator()(Point2D station);

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
    explicit StationList(PassengerTable passengerTable): stations_{}, passengerTable_{std::move(passengerTable)} {}

    void Create(Point2D position) { stations_.emplace(position, Station(position, passengerTable_, deleteF_)); }

    bool Has(Point2D position) { return stations_.contains(position); }
    Station& Get(Point2D position)  { return stations_.at(position); }

    void SetDeleter(Station::DeleteF deleteF) { deleteF = deleteF_; }

    void Update();

private:
    std::unordered_map<Point2D, Station> stations_;

    PassengerTable passengerTable_;
    Station::DeleteF deleteF_ = [](){ return 2; }; // Dzielnik liczb pozostałych pasażerów

};

#endif //AUTOBUS_STATION_HPP
