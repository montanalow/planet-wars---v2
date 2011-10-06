#include <algorithm>
#include <cmath>
#include <iostream>
#include <fstream>

#include "planet.h"

pw::planet::planet(int id, double x, double y, int owner, int ships, int growth_rate, pw::game_state* game_state)
  : _id(id), _x(x), _y(y), _owner(owner), _ships(ships), _growth_rate(growth_rate), _game_state(game_state) {
}

pw::planet::planet(const pw::planet& planet)
  : _id(planet._id), _x(planet._x), _y(planet._y), _owner(planet._owner), _ships(planet._ships),  _growth_rate(planet._growth_rate), _game_state(planet._game_state) {
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

pw::fleet* pw::planet::launch(int ships, pw::planet* destination) {
  ships = std::max(0, std::min(_ships, ships));
//  std::cerr << "LAUNCH | planet: " << _id << " ships: " << ships << "/" << _ships << "\n";
  if (ships == 0) {
    return NULL;
  }

  _ships -= ships;
  pw::fleet* fleet = new pw::fleet(_owner, ships, this, destination, time_to(*destination), time_to(*destination), 0, _game_state);
  _game_state->fleets().push_back(fleet);
  if (_owner == 1) {
    _game_state->allied_fleets().push_back(fleet);
  } else {
    _game_state->enemy_fleets().push_back(fleet);
  }
  return fleet;
}

pw::fleet* pw::planet::reserve(int ships) {
  ships = std::max(0, std::min(_ships, ships));
//  std::cerr << "RESERVE | planet: " << _id << " ships: " << ships << "/" << _ships << "\n";
  if (_owner != 1 || ships == 0) {
    return NULL;
  }

  _ships -= ships;
  pw::fleet* fleet = new pw::fleet(1, ships, this, this, 0, 0, 0, _game_state);
  _game_state->fleets().push_back(fleet);
  _game_state->allied_fleets().push_back(fleet);
  return fleet;
}

pw::fleet* pw::planet::commit(int ships, pw::planet* destination, int time) {
  int future_ships = _growth_rate * time;
  int max_time = time;
  for (std::vector<pw::fleet*>::iterator fleet = _game_state->allied_fleets().begin(); fleet < _game_state->allied_fleets().end(); ++fleet) {
    if ((*fleet)->source()->id() == _id && (*fleet)->is_commitment() <= time) {
      if ( (*fleet)->commitment_remaining() > max_time) {
        // if the commitment is further in the future, we have more production to handle it
        max_time = (*fleet)->commitment_remaining();
        future_ships += ((*fleet)->commitment_remaining() - max_time) * _growth_rate;
      }
      // subtract any existing commitments from future production
      future_ships -= (*fleet)->ships();
    }
  }
  future_ships = std::max(0, future_ships);
  ships = std::max(0, std::min(_ships + future_ships, ships));

//  std::cerr << "COMMIT | planet: " << _id << " ships: " << ships << "/" << _ships << "\n";
  if (_owner != 1) { // you can commit 0 ships as a flag that this has been considered
    return NULL;
  }

  _ships -= ships;
  pw::fleet* fleet = new pw::fleet(1, ships, this, destination, time_to(*destination), time_to(*destination), time, _game_state);
  _game_state->fleets().push_back(fleet);
  _game_state->allied_fleets().push_back(fleet);
  return fleet;
}

void pw::planet::game_state(pw::game_state* game_state) {
  _game_state = game_state;
}

pw::planet* pw::planet::closest_ally(double min_distance) const{
  pw::planet* ally = NULL;
  double closest_distance = -1;
  for (std::vector<pw::planet*>::iterator planet = _game_state->allied_planets().begin(); planet < _game_state->allied_planets().end(); ++planet) {
    int distance = distance_to(**planet);
    if (distance > min_distance && (closest_distance == -1 || distance < closest_distance)) {
      ally = *planet;
      closest_distance = distance;
    }
  }
  return ally;
}

pw::planet* pw::planet::closest_enemy() const{
  pw::planet* enemy = NULL;
  double closest_distance = -1;
  for (std::vector<pw::planet*>::iterator planet = _game_state->enemy_planets().begin(); planet < _game_state->enemy_planets().end(); ++planet) {
    int distance = distance;
    if (closest_distance == -1 || distance < closest_distance) {
      enemy = *planet;
      closest_distance = distance;
    }
  }
  return enemy;
}

pw::planet* pw::planet::largest_enemy() const{
  pw::planet* enemy = NULL;
  double most_ships = -1;
  for (std::vector<pw::planet*>::iterator planet = _game_state->enemy_planets().begin(); planet < _game_state->enemy_planets().end(); ++planet) {
    if ((*planet)->ships() > most_ships) {
      enemy = *planet;
      most_ships = (*planet)->ships();
    }
  }
  return enemy;
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

  // add up reserves and commitments in orbit (0 time remaining friendly fleets)
  for (std::vector<pw::fleet*>::iterator fleet = _game_state->allied_fleets().begin(); fleet < _game_state->allied_fleets().end(); ++fleet) {
    if ((*fleet)->time_remaining() == 0 && (*fleet)->destination()->id() == _id) {
      // a new fleet in a holding pattern around this planet
      allied_ships += (*fleet)->ships();
    }
  }

  for (int t = 1; t <= time; ++t) {
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
////    std::cerr << "  planet: " << _id <<  " in: " << t << " owner: " << owner << " ships: " << ships << "\n";
  }
////  std::cerr << "  return in\n";
  return pw::planet(_id, _x, _y, owner, ships, _growth_rate, _game_state);
}

double pw::planet::value() const {
  switch (_owner){
    case 0:
      return (double) _growth_rate / _ships;
    case 1:
      return 0;
    case 2:
      // TODO Adjust expansion vs aggresion ratio depending if I'm winning in terms of production
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

bool pw::planet::operator<(const pw::planet& planet) const {
  return _growth_rate < planet._growth_rate;
}

bool pw::planet::compare(const pw::planet* a, const pw::planet* b) {
  return *a < *b;
}

void pw::planet::ships(int ships) {
  _ships = ships;
}

