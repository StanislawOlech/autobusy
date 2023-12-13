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
    using UpdateF = std::function<std::span<Passenger>(Point2D)>;

    void DeletePassengers(Point2D dest);

    [[nodiscard]] Passenger GetPassengers(Point2D dest);

    [[nodiscard]] bool HasPassengers(Point2D dest) { return passengers_[dest] != 0; }

    void Update();
    void SetUpdateFunction(UpdateF updateF) { UpdatePassengers_ = std::move(updateF); }

private:
    Point2D position_;
    std::unordered_map<Point2D, uint32_t> passengers_;

    UpdateF UpdatePassengers_;
};


#endif //AUTOBUS_STATION_HPP
