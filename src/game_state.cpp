#include <cmath>
#include <cstdlib>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>
#include <fstream>

#include "game_state.h"
#include "util.h"

pw::game_state::game_state(const std::string& game_state_data) :
  _planets(),
  _fleets(),
  _allied_planets(),
  _neutral_planets(),
  _enemy_planets(),
  _allied_fleets(),
  _enemy_fleets(),
  _max_fleet_time_remaining(0) {
  parse_game_state_data(game_state_data);
}

pw::game_state::game_state(const pw::game_state& game_state) :
  _planets(game_state._planets.begin(), game_state._planets.end()),
  _fleets(game_state._fleets.begin(), game_state._fleets.end()),
  _allied_planets(game_state._allied_planets.begin(), game_state._allied_planets.end()),
  _neutral_planets(game_state._neutral_planets.begin(), game_state._neutral_planets.end()),
  _enemy_planets(game_state._enemy_planets.begin(), game_state._enemy_planets.end()),
  _allied_fleets(game_state._allied_fleets.begin(), game_state._allied_fleets.end()),
  _enemy_fleets(game_state._enemy_fleets.begin(), game_state._enemy_fleets.end()),
  _max_fleet_time_remaining(0) { }

// data
const std::vector<pw::planet>& pw::game_state::planets() const {
  return _planets;
}

const std::vector<pw::fleet>& pw::game_state::fleets() const {
  return _fleets;
}

// indexes
const std::vector<pw::planet>& pw::game_state::allied_planets() const {
  return _allied_planets;
}

const std::vector<pw::planet>& pw::game_state::neutral_planets() const {
  return _neutral_planets;
}

const std::vector<pw::planet>& pw::game_state::enemy_planets() const {
  return _enemy_planets;
}

const std::vector<pw::fleet>& pw::game_state::allied_fleets() const {
  return _allied_fleets;
}

const std::vector<pw::fleet>& pw::game_state::enemy_fleets() const {
  return _enemy_fleets;
}

int pw::game_state::max_fleet_time_remaining() const{
  return _max_fleet_time_remaining;
}

std::string pw::game_state::to_string() const {
  std::stringstream s;
  for (unsigned int i = 0; i < _planets.size(); ++i) {
    const pw::planet& planet = _planets[i];
    s << "P " << planet.x() << " " << planet.y() << " " << planet.owner() << " " << planet.ships() << " " << planet.growth_rate() << std::endl;
  }
  for (unsigned int i = 0; i < _fleets.size(); ++i) {
    const fleet& f = _fleets[i];
    s << "F " << f.owner() << " " << f.ships() << " " << f.source().id() << " " << f.destination().id() << " " << f.total_trip_time() << " " << f.time_remaining() << std::endl;
  }
  return s.str();
}

void pw::game_state::issue_order(pw::planet& source, pw::planet& destination, int ships) {
  source.remove_ships(ships);
  _fleets.push_back(pw::fleet(1, ships, source, destination, source.time_to(destination), source.time_to(destination), this));
  _allied_fleets.push_back(_fleets.back());
  std::cout << source.id() << " " << destination.id() << " " << ships << std::endl;
}

void pw::game_state::clear() {
  // clear data
  _planets.clear();
  _fleets.clear();
}

void pw::game_state::index() {
  // clear planet indexes
  _allied_planets.clear();
  _neutral_planets.clear();
  _enemy_planets.clear();
  // index planets
  for (std::vector<pw::planet>::iterator planet = _planets.begin(); planet < _planets.end(); ++planet) {
    switch (planet->owner()) {
      case 0:
        _neutral_planets.push_back(*planet);
        break;
      case 1:
        _allied_planets.push_back(*planet);
        break;
      case 2:
        _enemy_planets.push_back(*planet);
        break;
    }
  }

  // clear fleet indexes
  _allied_fleets.clear();
  _enemy_fleets.clear();
  _max_fleet_time_remaining = 0;
  // index fleets
  for (std::vector<pw::fleet>::iterator fleet = _fleets.begin(); fleet < _fleets.end(); ++fleet) {
    switch (fleet->owner()) {
      case 1:
        _allied_fleets.push_back(*fleet);
        break;
      case 2:
        _enemy_fleets.push_back(*fleet);
        break;
    }
    if (fleet->time_remaining() > _max_fleet_time_remaining) {
      _max_fleet_time_remaining = fleet->time_remaining();
    }
  }
}

int pw::game_state::parse_game_state_data(const std::string& game_state_data) {
  clear();

  std::vector<std::string> lines = string_util::tokenize(game_state_data, "\n");
  int planet_id = 0;
  for (unsigned int i = 0; i < lines.size(); ++i) {
    std::string& line = lines[i];
    size_t comment_begin = line.find_first_of('#');
    if (comment_begin != std::string::npos) {
      line = line.substr(0, comment_begin);
    }
    std::vector<std::string> tokens = string_util::tokenize(line);
    if (tokens.size() == 0) {
      continue;
    }
    if (tokens[0] == "P") {
      if (tokens.size() != 6) {
        return 0;
      }
      double x = atof(tokens[1].c_str());
      double y = atof(tokens[2].c_str());
      int owner = atoi(tokens[3].c_str());
      int ships = atoi(tokens[4].c_str());
      int growth_rate = atoi(tokens[5].c_str());
      _planets.push_back(pw::planet(planet_id++, x, y, owner, ships, growth_rate, this));
    } else if (tokens[0] == "F") {
      // create a fleet
      if (tokens.size() != 7) {
        return 0;
      }
      int owner = atoi(tokens[1].c_str());
      int ships = atoi(tokens[2].c_str());
      if (ships < 1) {
        continue;
      }
      int source_id = atoi(tokens[3].c_str());
      int destination_id = atoi(tokens[4].c_str());
      int total_trip_time = atoi(tokens[5].c_str());
      int time_remaining = atoi(tokens[6].c_str());
      _fleets.push_back(pw::fleet(owner, ships, _planets[source_id], _planets[destination_id], total_trip_time, time_remaining, this));
    } else {
      return 0;
    }
  }
  index();
  return 1;
}

void pw::game_state::finish_turn() const {
  std::cout << "go" << std::endl;
  std::cout.flush();
}

pw::game_state& pw::game_state::operator=(const pw::game_state& game_state) {
  // deep copy planets and fleets
  _planets.assign(game_state._planets.begin(), game_state._planets.end());
  for (std::vector<pw::planet>::iterator planet = _planets.begin(); planet < _planets.end(); ++planet) {
    planet->game_state(this);
  }
  _fleets.assign(game_state._fleets.begin(), game_state._fleets.end());
  for (std::vector<pw::fleet>::iterator fleet = _fleets.begin(); fleet < _fleets.end(); ++fleet) {
    fleet->game_state(this);
  }
  return *this;
}

pw::game_state& pw::game_state::operator++() {
  // advance each planet to the next turn (in place)
  for (std::vector<pw::planet>::iterator planet = _planets.begin(); planet < _planets.end(); ++planet) {
    // add newly produced ships to player controlled planets
    if (planet->owner() > 0) {
      planet->add_ships(planet->growth_rate());
    }

    // initialize ships on the planet
    int neutral_ships = 0, allied_ships = 0, enemy_ships = 0;
    switch (planet->owner()) {
      case 0:
        neutral_ships = planet->ships();
        break;
      case 1:
        allied_ships = planet->ships();
        break;
      case 2:
        enemy_ships = planet->ships();
        break;
    }

    // total up all ships arriving on the next turn
    for (std::vector<pw::fleet>::iterator fleet = _fleets.begin(); fleet < _fleets.end(); ++fleet) {
      if (fleet->destination() == (*planet) && fleet->time_remaining() == 1) {
        switch (fleet->owner()) {
          case 1:
            allied_ships += fleet->ships();
            break;
          case 2:
            enemy_ships += fleet->ships();
            break;
        }
      }
    }

    // set the owner of the planet, and the number of ships remaining
    if (neutral_ships > allied_ships && neutral_ships > enemy_ships) {
      // neutral powers win
      planet->owner(0);
      planet->ships(neutral_ships - std::max(allied_ships, enemy_ships));
    } else if (allied_ships > neutral_ships && allied_ships > enemy_ships) {
      // allies win
      planet->owner(1);
      planet->ships(allied_ships - std::max(neutral_ships, enemy_ships));
    } else if (enemy_ships > allied_ships && enemy_ships > neutral_ships) {
      // enemies win
      planet->owner(2);
      planet->ships(enemy_ships - std::max(neutral_ships, allied_ships));
    } else {
      // no clear winner, all ships are annihilated and control remains the same
      planet->ships(0);
    }
  }

  // copy all fleets that don't arrive somewhere next turn (since battles have already been taken care of), and update their time_remaining
  std::vector<pw::fleet> next_fleets;
  for (std::vector<pw::fleet>::iterator fleet = _fleets.begin(); fleet < _fleets.end(); ++fleet) {
    if (fleet->time_remaining() > 1) { // if the fleet has time left on it
      next_fleets.push_back(pw::fleet(fleet->owner(), fleet->ships(), _planets[fleet->source().id()], _planets[fleet->destination().id()], fleet->total_trip_time(), fleet->time_remaining() - 1, this)); // copy it to the next turn
    }
  }
  _fleets.swap(next_fleets);
  index();
  return *this;
}

pw::game_state pw::game_state::operator++(int) {
  pw::game_state previous(*this);
  ++ * this;
  return previous;
}