#include <algorithm>
#include <cmath>
#include <iostream>
#include <fstream>

#include "planet.h"

pw::planet::planet(int id, double x, double y, int owner, int ships, int growth_rate, const pw::game_state* game_state)
  : _id(id), _x(x), _y(y), _owner(owner), _ships(ships), _growth_rate(growth_rate), _game_state(game_state) {
}

pw::planet::planet(const pw::planet& planet)
  : _id(planet._id), _x(planet._x), _y(planet._y), _owner(planet._owner), _ships(planet._ships), _growth_rate(planet._growth_rate), _game_state(planet._game_state) {
}

int pw::planet::id() const {
  return _id;
}

double pw::planet::x() const {
  return _x;
}

double pw::planet::y() const {
  return _y;
}

int pw::planet::owner() const {
  return _owner;
}

int pw::planet::ships() const {
  return _ships;
}

int pw::planet::growth_rate() const {
  return _growth_rate;
}

void pw::planet::owner(int new_owner) {
  _owner = new_owner;
}

void pw::planet::ships(int ships) {
  _ships = ships;
}

void pw::planet::reserve(int ships) {
  _ships -= ships;
  _reserves += ships;
}

void pw::planet::add_ships(int amount) {
  _ships += amount;
}

void pw::planet::remove_ships(int amount) {
  _ships -= amount;
}

void pw::planet::game_state(pw::game_state* game_state) {
  _game_state = game_state;
}

pw::planet pw::planet::in(int time) const {
  // figure out how many ships and who the planet belongs to in X turns
  int owner = _owner;
  int ships = _ships;
  int player_zero = 0, player_one = 0, player_two = 0;

  switch (owner) {
    case 0:
      player_zero = ships;
      break;
    case 1:
      player_one = ships;
      break;
    case 2:
      player_two = ships;
      break;
  }

  const std::vector<pw::fleet>& fleets = _game_state->fleets();
  for (int t = 0; t <= time; ++t) {
//    std::cerr << "    time:" << t << "\n";
    for (int i = 0; i < fleets.size(); ++i) {
      // find all fleets arriving this turn, and add up their ships
      const pw::fleet& fleet = fleets[i];
      if (fleet.time_remaining() == t && fleet.destination().id() == _id) {
        switch (fleet.owner()) {
          case 1:
            player_one += fleet.ships();
            break;
          case 2:
            player_two += fleet.ships();
            break;
        }
      }
    }

    if (player_zero > player_one && player_zero > player_two) {
      owner = 0;
      player_zero = player_zero - std::max(player_one, player_two);
      player_one = 0;
      player_two = 0;
      ships = player_zero;
    } else if (player_one > player_zero && player_one > player_two) {
      owner = 1;
      player_zero = 0;
      player_one = player_one - std::max(player_zero, player_two) + _growth_rate;
      player_two = 0;
      ships = player_one;
    } else if (player_two > player_one && player_two > player_zero) {
      owner = 2;
      player_zero = 0;
      player_one = 0;
      player_two = player_two - std::max(player_zero, player_one) + _growth_rate;
      ships = player_two;
    }
  }
//  std::cerr << "  return in\n";
  return pw::planet(_id, _x, _y, owner, ships, _growth_rate, _game_state);
}

double pw::planet::value() const {
  switch (_owner){
    case 0:
      return (double) _growth_rate / _ships;
    case 1:
      return 0;
    case 2:
      return (double) (8 * _growth_rate) / _ships;
  }

  return 0; // failsafe
}

double pw::planet::value_in(int time) const {
  return value() / time;
}

int pw::planet::time_to(const pw::planet& planet) const {
  return (int) ceil(pythagorean_distance_to(planet));
}

double pw::planet::distance_to(const pw::planet& planet) const {
  return pythagorean_distance_to(planet);
}

double pw::planet::manhattan_distance_to(const pw::planet& planet) const{
  return abs(_x - planet._x) + abs(_y - planet._y);
}

double pw::planet::pythagorean_distance_to(const pw::planet& planet) const {
  return sqrt(pythagorean_distance_squared_to(planet));
}

double pw::planet::pythagorean_distance_squared_to(const pw::planet& planet) const {
  return pow(_x - planet._x, 2) + pow(_y - planet._y, 2);
}

const pw::planet& pw::planet::operator=(const pw::planet& planet) {
  _id = planet._id;
  _x = planet._x;
  _y = planet._y;
  _owner = planet._owner;
  _ships = planet._ships;
  _growth_rate = planet._growth_rate;
  _game_state = planet._game_state;
  return *this;
}

bool pw::planet::operator==(const pw::planet& planet) const {
  return _id == planet._id;
}
