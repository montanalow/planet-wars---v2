#include <algorithm>
#include <cmath>
#include <iostream>
#include <fstream>

#include "planet.h"

pw::planet::planet(int id, double x, double y, int owner, int ships, int growth_rate, pw::game_state* game_state)
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
  ships = std::min(_ships, ships);
  _reserves += ships;
  _ships -= ships;
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

const pw::planet* pw::planet::closest_source() const{
  const pw::planet* source = NULL;
  double closest_distance = -1;
  for (std::vector<pw::planet*>::iterator planet = _game_state->planets().begin(); planet < _game_state->planets().end(); ++planet) {
    if ((*planet)->owner() == 1 && (*planet)->ships() > 1 && (closest_distance == -1 || distance_to(**planet) < closest_distance)) {
      source = *planet;
      closest_distance = distance_to(**planet);
    }
  }
  return source;
}

pw::planet pw::planet::in(int time) const {
  int owner = _owner;
  int ships = _ships;
  
  // initialize ships on the planet
  int neutral_ships = 0, allied_ships = 0, enemy_ships = 0;
  switch (owner) {
    case 0:
      neutral_ships = _ships;
      break;
    case 1:
      allied_ships = _ships;
      break;
    case 2:
      enemy_ships = _ships;
      break;
  }

  for (int t = 0; t <= time; ++t) {
    // add newly produced ships to player controlled planets
    switch (owner) {
      case 1:
        allied_ships += _growth_rate;
        break;
      case 2:
        enemy_ships += _growth_rate;
        break;
    }

    // total up all ships arriving on the next turn
    for (std::vector<pw::fleet*>::iterator fleet = _game_state->fleets().begin(); fleet < _game_state->fleets().end(); ++fleet) {
      if ((*fleet)->destination()->id() == _id && (*fleet)->time_remaining() == t) {
        switch ((*fleet)->owner()) {
          case 1:
            allied_ships += (*fleet)->ships();
            break;
          case 2:
            enemy_ships += (*fleet)->ships();
            break;
        }
      }
    }

    // set the owner of the planet, and the number of ships remaining
    if (neutral_ships > allied_ships && neutral_ships > enemy_ships) {
      // neutral powers win
      ships = neutral_ships = neutral_ships - std::max(allied_ships, enemy_ships);
      allied_ships = enemy_ships = 0;
      owner = 0;
    } else if (allied_ships > neutral_ships && allied_ships > enemy_ships) {
      // allies win
      ships = allied_ships = allied_ships - std::max(neutral_ships, enemy_ships);
      neutral_ships = enemy_ships = 0;
      owner = 1;
    } else if (enemy_ships > allied_ships && enemy_ships > neutral_ships) {
      // enemies win
      ships = enemy_ships = enemy_ships - std::max(neutral_ships, allied_ships);
      neutral_ships = allied_ships = 0;
      owner = 2;
    } else {
      // no clear winner, all ships are annihilated and control remains the same
      ships = neutral_ships = allied_ships = enemy_ships = 0;
    }
//    std::cerr << "  planet: " << _id <<  " in: " << t << " owner: " << owner << " ships: " << ships << "\n";
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
      return (double) (2 * _growth_rate) / _ships;
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
