#include <algorithm>
#include <cmath>
#include <iostream>
#include <fstream>

#include "planet.h"

pw::planet::planet(
  int id,
  double x,
  double y,
  int owner,
  int num_ships,
  int growth_rate,
  pw::game_state* game_state
) :
  _id(id),
  _x(x),
  _y(y),
  _owner(owner),
  _num_ships(num_ships),
  _growth_rate(growth_rate),
  _game_state(game_state) {
};

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

int pw::planet::num_ships() const {
  return _num_ships;
}

int pw::planet::growth_rate() const {
  return _growth_rate;
}

void pw::planet::owner(int new_owner) {
  _owner = new_owner;
}

void pw::planet::num_ships(int num_ships) {
  _num_ships = num_ships;
}

void pw::planet::add_ships(int amount) {
  _num_ships += amount;
}

void pw::planet::remove_ships(int amount) {
  _num_ships -= amount;
}

pw::planet pw::planet::in(int turns) const {
  // figure out how many ships and who the planet belongs to in X turns
  int owner = _owner;
  int num_ships = _num_ships;
  int player_zero = 0, player_one = 0, player_two = 0;
//  logger << "    in id: " << _id << " | owner: " << owner << " | num ships: " << num_ships << std::endl;

  switch (owner) {
    case 0:
      player_zero = num_ships;
      break;
    case 1:
      player_one = num_ships;
      break;
    case 2:
      player_two = num_ships;
      break;
  }

  std::vector<pw::fleet> fleets = _game_state->fleets();
  for (int turn = 0; turn <= turns; ++turn) {
    for (int i = 0; i < fleets.size(); ++i) {
      // find all fleets arriving this turn, and add up their ships
      pw::fleet fleet = fleets[i];
      if (fleet.turns_remaining() == turn && fleet.destination()->id() == _id) {
        switch (fleet.owner()) {
          case 1:
            player_one += fleet.num_ships();
            break;
          case 2:
            player_two += fleet.num_ships();
            break;
        }
      }
    }

    if (player_zero > player_one && player_zero > player_two) {
      owner = 0;
      player_zero = player_zero - std::max(player_one, player_two);
      player_one = 0;
      player_two = 0;
      num_ships = player_zero;
    } else if (player_one > player_zero && player_one > player_two) {
      owner = 1;
      player_zero = 0;
      player_one = player_one - std::max(player_zero, player_two) + _growth_rate;
      player_two = 0;
      num_ships = player_one;
    } else if (player_two > player_one && player_two > player_zero) {
      owner = 2;
      player_zero = 0;
      player_one = 0;
      player_two = player_two - std::max(player_zero, player_one) + _growth_rate;
      num_ships = player_two;
    }
  }

  return pw::planet(_id, _x, _y, owner, num_ships, _growth_rate, _game_state);
}

double pw::planet::value_in(int turns) const {
  pw::planet planet = in(turns);
  switch (planet.owner()){
    case 0:
      return planet.growth_rate() - planet.num_ships();
    case 1:
      return -9999;
    case 2:
      return 2 * planet.growth_rate() - planet.num_ships();
  }

  return 0; // failsafe
}

int pw::planet::turns_to(const pw::planet& planet) const {
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

double pw::planet::pythagorean_distance_squared_to(const pw::planet& planet) const{
  return pow(_x - planet._x, 2) + pow(_y - planet._y, 2);
}
