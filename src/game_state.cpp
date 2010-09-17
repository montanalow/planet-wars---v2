#include <cmath>
#include <cstdlib>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>
#include <fstream>

#include "game_state.h"
#include "util.h"

pw::game_state::game_state(const std::string& game_state_data, int turn) :
  _planets(),
  _fleets(),
  _allied_planets(),
  _neutral_planets(),
  _enemy_planets(),
  _allied_fleets(),
  _enemy_fleets(),
  _max_fleet_time_remaining(0),
  _turn(turn) {
  parse_game_state_data(game_state_data);
}

pw::game_state::game_state(const pw::game_state& game_state, int turn) :
  _planets(game_state._planets.begin(), game_state._planets.end()),
  _fleets(game_state._fleets.begin(), game_state._fleets.end()),
  _allied_planets(game_state._allied_planets.begin(), game_state._allied_planets.end()),
  _neutral_planets(game_state._neutral_planets.begin(), game_state._neutral_planets.end()),
  _enemy_planets(game_state._enemy_planets.begin(), game_state._enemy_planets.end()),
  _allied_fleets(game_state._allied_fleets.begin(), game_state._allied_fleets.end()),
  _enemy_fleets(game_state._enemy_fleets.begin(), game_state._enemy_fleets.end()),
  _max_fleet_time_remaining(0),
  _turn(turn) { }

// data
std::vector<pw::planet*>& pw::game_state::planets() {
  return _planets;
}

std::vector<pw::fleet*>& pw::game_state::fleets() {
  return _fleets;
}

// indexes
std::vector<pw::planet*>& pw::game_state::allied_planets() {
  return _allied_planets;
}

std::vector<pw::planet*>& pw::game_state::neutral_planets() {
  return _neutral_planets;
}

std::vector<pw::planet*>& pw::game_state::enemy_planets() {
  return _enemy_planets;
}

std::vector<pw::fleet*>& pw::game_state::allied_fleets() {
  return _allied_fleets;
}

std::vector<pw::fleet*>& pw::game_state::enemy_fleets() {
  return _enemy_fleets;
}

int pw::game_state::max_fleet_time_remaining() const {
  return _max_fleet_time_remaining;
}

std::string pw::game_state::to_string() const {
  std::stringstream s;
  for (unsigned int i = 0; i < _planets.size(); ++i) {
    const pw::planet& planet = *_planets[i];
    s << "P " << planet.x() << " " << planet.y() << " " << planet.owner() << " " << planet.ships() << " " << planet.growth_rate() << std::endl;
  }
  for (unsigned int i = 0; i < _fleets.size(); ++i) {
    const fleet& f = *_fleets[i];
    s << "F " << f.owner() << " " << f.ships() << " " << f.source()->id() << " " << f.destination()->id() << " " << f.total_trip_time() << " " << f.time_remaining() << std::endl;
  }
  return s.str();
}

void pw::game_state::issue_order(int source_id, int destination_id, int ships) {
  pw::planet* source = _planets[source_id];
  pw::planet* destination = _planets[destination_id];
  if (ships >= source->ships()) {
//    std::cerr << "*********** ERROR: ordered too many ships ******************";
    return;
  }
  pw::fleet* fleet = new pw::fleet(1, ships, source, destination, source->time_to(*destination), source->time_to(*destination), this);
  source->remove_ships(fleet->ships());
  _fleets.push_back(fleet);
  _allied_fleets.push_back(fleet);
  std::cout << source_id << " " << destination_id << " " << ships << std::endl;
//  std::cerr << "order | source: " << source_id << " destination: " << destination_id << " ships: " << ships << std::endl;
}

void pw::game_state::reserve(int source_id, int ships) {
  _planets[source_id]->reserve(ships);
//  std::cerr << "reserve | source: " << source_id << " ships: " << ships << std::endl;
}

void pw::game_state::clear() {
  // clear data
  for (std::vector<pw::planet*>::iterator planet = _planets.begin(); planet < _planets.end(); ++planet) {
    delete (*planet);
  }
  _planets.clear();
  for (std::vector<pw::fleet*>::iterator fleet = _fleets.begin(); fleet < _fleets.end(); ++fleet) {
    delete (*fleet);
  }
  _fleets.clear();
}

void pw::game_state::index() {
//  std::sort(_fleets.begin(), _fleets.end());

  // clear planet indexes
  _allied_planets.clear();
  _neutral_planets.clear();
  _enemy_planets.clear();
  // index planets
  for (std::vector<pw::planet*>::iterator planet = _planets.begin(); planet < _planets.end(); ++planet) {
    switch ((*planet)->owner()) {
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
  for (std::vector<pw::fleet*>::iterator fleet = _fleets.begin(); fleet < _fleets.end(); ++fleet) {
    switch ((*fleet)->owner()) {
      case 1:
        _allied_fleets.push_back(*fleet);
        break;
      case 2:
        _enemy_fleets.push_back(*fleet);
        break;
    }
    if ((*fleet)->time_remaining() > _max_fleet_time_remaining) {
      _max_fleet_time_remaining = (*fleet)->time_remaining();
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
      _planets.push_back(new pw::planet(planet_id++, x, y, owner, ships, growth_rate, this));
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
      _fleets.push_back(new pw::fleet(owner, ships, _planets[source_id], _planets[destination_id], total_trip_time, time_remaining, this));
    } else {
      return 0;
    }
  }
  index();
  return 1;
}

void pw::game_state::finish_turn() {
  std::cout << "go" << std::endl;
  std::cout.flush();
//  std::cerr << "*** finished turn ***\n";
//  std::cerr.flush();
}

pw::game_state& pw::game_state::operator=(const pw::game_state& game_state) {
  // deep copy planets and fleets
  for (std::vector<pw::planet*>::iterator planet = _planets.begin(); planet < _planets.end(); ++planet) {
    _planets.push_back(new pw::planet(**planet));
    _planets.back()->game_state(this);
  }
  _fleets.assign(game_state._fleets.begin(), game_state._fleets.end());
  for (std::vector<pw::fleet*>::iterator fleet = _fleets.begin(); fleet < _fleets.end(); ++fleet) {
    _fleets.push_back(new pw::fleet(**fleet));
    _fleets.back()->game_state(this);
  }
  return *this;
}

pw::game_state& pw::game_state::operator++() {
  // advance each planet to the next turn (in place)
  for (std::vector<pw::planet*>::iterator planet = _planets.begin(); planet < _planets.end(); ++planet) {
    // add newly produced ships to player controlled planets
    if ((*planet)->owner() > 0) {
      (*planet)->add_ships((*planet)->growth_rate());
    }

    // initialize ships on the planet
    int neutral_ships = 0, allied_ships = 0, enemy_ships = 0;
    switch ((*planet)->owner()) {
      case 0:
        neutral_ships = (*planet)->ships();
        break;
      case 1:
        allied_ships = (*planet)->ships();
        break;
      case 2:
        enemy_ships = (*planet)->ships();
        break;
    }

    // total up all ships arriving on the next turn
    for (std::vector<pw::fleet*>::iterator fleet = _fleets.begin(); fleet < _fleets.end(); ++fleet) {
      if ((*fleet)->destination() == (*planet) && (*fleet)->time_remaining() == 1) {
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
      (*planet)->owner(0);
      (*planet)->ships(neutral_ships - std::max(allied_ships, enemy_ships));
    } else if (allied_ships > neutral_ships && allied_ships > enemy_ships) {
      // allies win
      (*planet)->owner(1);
      (*planet)->ships(allied_ships - std::max(neutral_ships, enemy_ships));
    } else if (enemy_ships > allied_ships && enemy_ships > neutral_ships) {
      // enemies win
      (*planet)->owner(2);
      (*planet)->ships(enemy_ships - std::max(neutral_ships, allied_ships));
    } else {
      // no clear winner, all ships are annihilated and control remains the same
      (*planet)->ships(0);
    }
  }

  // copy all fleets that don't arrive somewhere next turn (since battles have already been taken care of), and update their time_remaining
  std::vector<pw::fleet*> next_fleets;
  for (std::vector<pw::fleet*>::iterator fleet = _fleets.begin(); fleet < _fleets.end(); ++fleet) {
    if ((*fleet)->time_remaining() > 1) { // if the fleet has time left on it
      (*fleet)->time_remaining((*fleet)->time_remaining() - 1);
      next_fleets.push_back(*fleet); // copy it to the next turn
    }
  }
  _fleets.swap(next_fleets);
  index();
  _turn++;
  return *this;
}

pw::game_state pw::game_state::operator++(int) {
  pw::game_state previous(*this);
  ++ * this;
  return previous;
}

void pw::game_state::take_turn() {
  // protect against the ragebot rush
  if (_turn == 1) {
    for (int i = 0; i < _allied_planets.size(); ++i ){
      pw::planet* planet = _allied_planets[i];
      planet->reserve(planet->ships()); // reserve 25% of the planets ships
    }
  }
//  std::cerr << "*** Defending against " << _enemy_fleets.size() << " enemy fleets ***\n";
  for (int i = 0; i < _enemy_fleets.size(); ++i ){
    pw::fleet* enemy_fleet = _enemy_fleets[i];
    pw::planet planet_before_invasion = enemy_fleet->destination()->in(enemy_fleet->time_remaining() - 1);
    if (planet_before_invasion.owner() == 1) { // invasion!
//    std::cerr << "  invading | source: " << enemy_fleet->source()->id() << " destination: " << enemy_fleet->destination()->id() << " ships: " << enemy_fleet->ships() << " time remaining: " << enemy_fleet->time_remaining() << "\n";
      pw::planet planet_after_invasion = enemy_fleet->destination()->in(enemy_fleet->time_remaining());
//      std::cerr << "  after invasion | owner: " << planet_after_invasion.owner() << " ships: " << planet_after_invasion.ships() << "\n";
      if (planet_after_invasion.owner() == 1) {
//        std::cerr << "1\n";
        reserve(planet_after_invasion.id(), std::max(0, enemy_fleet->destination()->ships() - planet_after_invasion.ships())); // reserve only as many ships as it takes to defend
      } else {
//        std::cerr << "2\n";
        // reserve everything
        reserve(planet_after_invasion.id(), enemy_fleet->destination()->ships());
        // find backup
        while (true) {
          const pw::planet* source = planet_after_invasion.closest_source();
          if (source) {
            int time = source->time_to(*enemy_fleet->destination());
            if (time > enemy_fleet->time_remaining()) {
              // oh noes the planet is lost
              if (source->ships() > enemy_fleet->destination()->in(time).ships()) {
                // at least we can clobber them, and can stop looking for reinforcements
//                std::cerr << "a\n";
                issue_order(source->id(), enemy_fleet->destination()->id(), enemy_fleet->destination()->in(time).ships() + 1);
                break;
              } else {
                // send everything this planet's got, and keep finding reinforcements
//                std::cerr << "b\n";
                issue_order(source->id(), enemy_fleet->destination()->id(), source->ships() - 1);
              }
            } else {
              // we'll get there first!
              if (source->ships() > planet_after_invasion.ships()) {
//                std::cerr << "c\n";
                issue_order(source->id(), enemy_fleet->destination()->id(), planet_after_invasion.ships());
                break;
              } else {
//                std::cerr << "d\n";
                // send everything this planet's got, and keep finding reinforcements
                issue_order(source->id(), enemy_fleet->destination()->id(), source->ships() - 1);
              }
            }
//            std::cerr << "  source:" << source->id() << " ships: " << source->ships() << " distance: " << source->distance_to(*enemy_fleet->destination()) << "\n";
//            std::cerr << "  planet:" << planet_after_invasion.id() << " time: " << enemy_fleet->time_remaining() << " ships: " << planet_after_invasion.ships() << " growth: " << planet_after_invasion.growth_rate() << " value: " << planet_after_invasion.value() / pow(enemy_fleet->time_remaining() + 1, 2) << "\n";
          } else {
//            std::cerr << "  no source\n";
            break;
          }
          planet_after_invasion = enemy_fleet->destination()->in(enemy_fleet->time_remaining());
        }
      }
    } else {
//      std::cerr << "  not an invader | source: " << enemy_fleet->source()->id() << " destination: " << enemy_fleet->destination()->id() << " ships: " << enemy_fleet->ships() << "\n";
    }
  }

  // attack their planets
//  std::cerr << "*** Attacking ***\n";
  for (int i = 0; i < _allied_planets.size(); ++i ){
    pw::planet* source = _allied_planets[i];
//    std::cerr << "source: " << source->id() << "\n";
    while (source->ships() > 1) {
      const pw::planet* destination = NULL;
      double highest_value = 0;

      // find the destination with the highest value
      for (size_t j = 0; j < _planets.size(); ++j) {
        pw::planet* planet = _planets[j];
        if (planet->id() == source->id()) {
          continue;
        }
        double time = source->time_to(*planet);
        pw::planet planet_at_arrival = planet->in(time);
//        std::cerr << "  planet:" << planet_at_arrival.id() << " owner: " << planet_at_arrival.owner() << " time: " << time << " ships: " << planet_at_arrival.ships() << " growth: " << planet_at_arrival.growth_rate() << " value: " << planet_at_arrival.value() / time << "\n";
        if (planet_at_arrival.value() / pow(time,2) > highest_value) {
          destination = planet;
          highest_value = planet_at_arrival.value() / pow(time,2);
        }
      }

      // find the min ships we need to send
      if (destination == NULL || destination->id() == source->id()) {
//        std::cerr << " no destination\n:";
        break;
      } else {
//        std::cerr << "destination: " << destination->id() << "\n";
  
        pw::planet d = destination->in(source->time_to(*destination));
        int ships = std::min(source->ships() - 1, d.ships() + 1 + (d.owner() == 2 ? d.growth_rate() : 0));
        issue_order(source->id(), destination->id(), ships);
      }
    }
  }
  finish_turn();

//  // 1) calculate future game states
//  std::vector<pw::game_state> game_states;
//  game_states.push_back(game_state);
//  while(game_state.max_fleet_time_remaining() > 0) {
//    game_states.push_back(++game_state);
//  }
//
//  // 2) find the largest forseable change in production for the cheapest ships in the closest time
//  pw::planet* target = NULL;
//  double target_value = 0;
//  int time = 0;
//  for (std::vector<pw::game_state>::const_iterator game_state = game_states.begin(); game_state < game_states.end(); ++game_state) {
//    for (std::vector<pw::planet>::const_iterator planet = game_state->allied_planets().begin(); planet < game_state->allied_planets().end(); ++planet) {
//      if (planet->owner() != 1) {
//        if (planet->growth_rate() / (planet->ships() * time + 1.0) > target_value) {
//          target_value = planet->growth_rate() / (planet->ships() * time + 1.0); // TODO denominator should be sum(source ships * source time to this destination)
//          target = &(*planet);
//        }
//      }
//    }
//    ++time;
//  }
//
//  if (target) {
//
//  }
//
//  for (size_t time = 0; time < game_states.size(); ++time) {
//    pw::game_state& game_state = game_states[time];
//    for (std::vector<pw::planet>::const_iterator planet = game_state.planets().begin(); planet < game_state.planets().end(); ++planet) {
//      int cost = 0;
//      switch (planet->owner()) {
//        case 0:
//          cost = planet->ships() * 2 * time;
//          break;
//        case 1:
//          cost = time;
//          break;
//        case 2:
//          cost = planet->ships() * time;
//          break;
//      }
//      double value = planet->growth_rate() * planet->owner() * (game_state.max_fleet_time_remaining() - time);
//    }
//  }
}
