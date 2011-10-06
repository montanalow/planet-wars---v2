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
  _check = new game_state(*this);
}

pw::game_state::game_state(const pw::game_state& game_state) :
  _planets(),
  _fleets(),
  _allied_planets(),
  _neutral_planets(),
  _enemy_planets(),
  _allied_fleets(),
  _enemy_fleets(),
  _max_fleet_time_remaining(0),
  _turn(game_state._turn),
  _check(NULL) {
  for (std::vector<pw::planet*>::const_iterator planet = game_state._planets.begin(); planet < game_state._planets.end(); ++planet) {
    pw::planet* copy = new pw::planet(**planet);
    copy->game_state(this);
    _planets.push_back(copy);
  }
  for (std::vector<pw::fleet*>::const_iterator fleet = game_state._fleets.begin(); fleet < game_state._fleets.end(); ++fleet) {
    pw::fleet* copy = new pw::fleet(**fleet);
    copy->game_state(this);
    _fleets.push_back(copy);
  }
  index();
}

pw::game_state::~game_state(){
  clear();
  delete _check;
}

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
    pw::planet& planet = *_planets[i];
    s << "P " << planet.x() << " " << planet.y() << " " << planet.owner() << " " << planet.ships() << " " << planet.growth_rate() << "\n";
  }
  for (unsigned int i = 0; i < _fleets.size(); ++i) {
    fleet& f = *_fleets[i];
    s << "F " << f.owner() << " " << f.ships() << " " << f.source()->id() << " " << f.destination()->id() << " " << f.total_time() << " " << f.time_remaining() << "\n";
  }
  return s.str();
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
//  std::sort(_planets.begin(), _planets.end(), pw::planet::compare);
  std::sort(_fleets.begin(), _fleets.end(), pw::fleet::compare);

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
      int total_time = atoi(tokens[5].c_str());
      int travel_remaining = atoi(tokens[6].c_str());
      _fleets.push_back(new pw::fleet(owner, ships, _planets[source_id], _planets[destination_id], total_time, travel_remaining, 0, this));
    } else {
      return 0;
    }
  }

  index();

  return 1;
}

void pw::game_state::finish_turn() {
  for (std::vector<pw::fleet*>::iterator fleet = _allied_fleets.begin(); fleet < _allied_fleets.end(); ++fleet) {
    if ((*fleet)->just_launched()) {
      pw::planet* check = _check->_planets[(*fleet)->source()->id()];
      check->ships(check->ships() - (*fleet)->ships());
      if (check->ships() < 0) {
//        std::cerr << "###### illegal move : too many ships ######\n" << (*fleet)->source()->id() << " " <<(*fleet)->destination()->id() << " " << (*fleet)->ships() << "\n";
      }
      if ((*fleet)->ships() == 0) {
//        std::cerr << "###### illegal move : no ships in fleet ######\n" << (*fleet)->source()->id() << " " <<(*fleet)->destination()->id() << " " << (*fleet)->ships() << "\n";
      } else {
        std::cout << (*fleet)->source()->id() << " " <<(*fleet)->destination()->id() << " " << (*fleet)->ships() << "\n";
      }
//      std::cerr << (*fleet)->source()->id() << " " <<(*fleet)->destination()->id() << " " << (*fleet)->ships() << "\n";
    }
  }
  std::cout << "go" << std::endl;
//  std::cerr << "go" << std::endl;
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
//  // advance each planet to the next turn (in place)
//  for (std::vector<pw::planet*>::iterator planet = _planets.begin(); planet < _planets.end(); ++planet) {
//    // add newly produced ships to player controlled planets
//    if ((*planet)->owner() > 0) {
//      (*planet)->add_ships((*planet)->growth_rate());
//    }
//
//    // initialize ships on the planet
//    int neutral_ships = 0, allied_ships = 0, enemy_ships = 0;
//    switch ((*planet)->owner()) {
//      case 0:
//        neutral_ships = (*planet)->ships();
//        break;
//      case 1:
//        allied_ships = (*planet)->ships();
//        break;
//      case 2:
//        enemy_ships = (*planet)->ships();
//        break;
//    }
//
//    // total up all ships arriving on the next turn
//    for (std::vector<pw::fleet*>::iterator fleet = _fleets.begin(); fleet < _fleets.end(); ++fleet) {
//      if ((*fleet)->destination() == (*planet) && (*fleet)->time_remaining() == 1) {
//        switch ((*fleet)->owner()) {
//          case 1:
//            allied_ships += (*fleet)->ships();
//            break;
//          case 2:
//            enemy_ships += (*fleet)->ships();
//            break;
//        }
//      }
//    }
//
//    // set the owner of the planet, and the number of ships remaining
//    if (neutral_ships > allied_ships && neutral_ships > enemy_ships) {
//      // neutral powers win
//      (*planet)->owner(0);
//      (*planet)->ships(neutral_ships - std::max(allied_ships, enemy_ships));
//    } else if (allied_ships > neutral_ships && allied_ships > enemy_ships) {
//      // allies win
//      (*planet)->owner(1);
//      (*planet)->ships(allied_ships - std::max(neutral_ships, enemy_ships));
//    } else if (enemy_ships > allied_ships && enemy_ships > neutral_ships) {
//      // enemies win
//      (*planet)->owner(2);
//      (*planet)->ships(enemy_ships - std::max(neutral_ships, allied_ships));
//    } else {
//      // no clear winner, all ships are annihilated and control remains the same
//      (*planet)->ships(0);
//    }
//  }
//
//  // copy all fleets that don't arrive somewhere next turn (since battles have already been taken care of), and update their time_remaining
//  std::vector<pw::fleet*> next_fleets;
//  for (std::vector<pw::fleet*>::iterator fleet = _fleets.begin(); fleet < _fleets.end(); ++fleet) {
//    if ((*fleet)->time_remaining() > 1) { // if the fleet has time left on it
//      (*fleet)->time_remaining((*fleet)->time_remaining() - 1);
//      next_fleets.push_back(*fleet); // copy it to the next turn
//    }
//  }
//  _fleets.swap(next_fleets);
//  index();
//  _turn++;
  return *this;
}

pw::game_state pw::game_state::operator++(int) {
  pw::game_state previous(*this);
  ++ * this;
  return previous;
}

void pw::game_state::take_turn() {
//    std::cerr << "*** pretend the enemy goes into an all out attack ***\n";
  for (int i = 0; i < _enemy_planets.size(); ++i) {
    // iterate over allied planets largest to smallest
    pw::planet* planet = _enemy_planets[i];
    pw::planet* closest_ally = planet->closest_ally();
    if (closest_ally) {
      planet->launch(planet->ships(), closest_ally);
    }
  }

//  std::cerr << "*** Reserving against " << _enemy_fleets.size() << " enemy fleets ***\n";
  for (int g = 5; g > 0; --g) {
    for (int i = 0; i < _enemy_fleets.size(); ++i ) {
      pw::fleet* enemy_fleet = _enemy_fleets[i];
      if (enemy_fleet->destination()->growth_rate() != g ) {
        continue;
      }
      if (enemy_fleet->destination()->owner() == 1) {
        pw::planet planet_before_invasion = enemy_fleet->destination()->in(enemy_fleet->time_remaining() - 1);
        if (planet_before_invasion.owner() == 1) { // invasion!
//          std::cerr << "  enemy fleet destination " << enemy_fleet->destination()->id() << " in " << enemy_fleet->time_remaining() << " turns with " << enemy_fleet->ships() <<  " ships \n";
          pw::planet planet_after_invasion = enemy_fleet->destination()->in(enemy_fleet->time_remaining());
//          std::cerr << "    defender has " << enemy_fleet->destination()->ships() << " ships (" << enemy_fleet->destination()->growth_rate() * enemy_fleet->time_remaining() <<  " growth) | after invasion owner: " << planet_after_invasion.owner() << " ships: " << planet_after_invasion.ships() << "\n";
          if (planet_after_invasion.owner() == 1) {
            // reserve only as many ships as it takes to defend
            enemy_fleet->destination()->reserve(enemy_fleet->destination()->ships() - planet_after_invasion.ships());
          } else {
            // reserve everything, we're gonna need backup
            enemy_fleet->destination()->reserve(enemy_fleet->destination()->ships());
          }
        }
      } else {
//        std::cerr << "  not an invader | destination: " << enemy_fleet->destination()->id() << " ships: " << enemy_fleet->ships() << "\n";
      }
    }
  }

//  std::cerr << "*** Backing up against " << _enemy_fleets.size() << " enemy fleets ***\n";
  for (int g = 5; g > 0; --g) {
    for (int i = 0; i < _enemy_fleets.size(); ++i ){
      pw::fleet* enemy_fleet = _enemy_fleets[i];
      if (enemy_fleet->destination()->growth_rate() != g ) {
        continue;
      }
      pw::planet planet_before_invasion = enemy_fleet->destination()->in(enemy_fleet->time_remaining() - 1);
      if (planet_before_invasion.owner() == 1) { // invasion!
//        std::cerr << "  enemy fleet destination: " << enemy_fleet->destination()->id() << " in " << enemy_fleet->time_remaining() << " turns with " << enemy_fleet->ships() <<  " ships \n";
        pw::planet planet_after_invasion = enemy_fleet->destination()->in(enemy_fleet->time_remaining());
//        std::cerr << "    defender has " << enemy_fleet->destination()->ships() << " ships (" << enemy_fleet->destination()->growth_rate() * enemy_fleet->time_remaining() <<  " growth) | after invasion owner: " << planet_after_invasion.owner() << " ships: " << planet_after_invasion.ships() << "\n";
        if (planet_after_invasion.owner() != 1) {
          // find backup
          double min_distance = 0;
          int need = planet_after_invasion.ships();
          while (need > 0) {
            pw::planet* source = enemy_fleet->destination()->closest_ally(min_distance); // TODO this should be the closest ally in the future
            if (source) {
              min_distance = source->distance_to(*enemy_fleet->destination());
              int time = source->time_to(*enemy_fleet->destination());
              int diff = enemy_fleet->time_remaining() - time;
              if (diff < 0) {
//                std::cerr << "    closest_ally " << source->id() << " is too far away: " << time << "\n";
                break;
              } else {
                pw::fleet* fleet = source->commit(need, enemy_fleet->destination(), diff);
                if (fleet) {
//                 std::cerr << "    got "<< fleet->ships() << " reinforcements from " << source->id() << "\n";
                  need -= fleet->ships();
                }
              }
            } else {
//              std::cerr << "  not enough reinforcements available\n";
              break;
            }
          }
        }
      } else {
//        std::cerr << "  not an invader | source: " << enemy_fleet->source()->id() << " destination: " << enemy_fleet->destination()->id() << " ships: " << enemy_fleet->ships() << "\n";
      }
    }
  }

  /* TODO
//  std::cerr << "*** Reserve against closest enemy ***\n";
  for (int i = 0; i < _enemy_planets.size(); ++i ){
    // iterate over allied planets largest to smallest
    pw::planet* enemy_planet = _enemy_planets[i];
    pw::planet* closest_ally = enemy_planet->closest_ally();
    if (closest_ally) {
      int time = enemy_planet->time_to(*closest_ally);
      int production = (closest_ally->growth_rate() * time - closest_ally->commitment());
      int commitment = closest_ally->reserve(enemy_planet->ships() - production) + production;
      int need = enemy_planet->ships() - commitment;
//      std::cerr << "  enemy planet: " << enemy_planet->id() << " ships: " << enemy_planet->ships() << "\n";
//      std::cerr << "    closest ally: " << closest_ally->id() << " ships: " << closest_ally->ships() << " distance: " << time << "\n";
//      std::cerr << "  reserving: " << commitment << " (production " << production << ")\n";
      double min_distance = 0;
      while (need > 0) {
        pw::planet* source = closest_ally->closest_ally(min_distance); // TODO this should be the closest ally in the future
        if (source) {
          min_distance = source->distance_to(*closest_ally);
          if (source->ships() < 1 ){
//            std::cerr << "    ally " << source->id() << " has no ships left" << "\n";
            continue;
          }

          int ally_time = source->time_to(*closest_ally);
          int diff = ally_time - time;
          if (diff > 0) {
//            std::cerr << "    ally " << source->id() << " is too far away: " << time << "\n";
            // oh noes the planet is lost, don't worry, we'll still attack it if we have any ships left after reinforcing
            break;
          } else {
            // we'll get there first!
            if (source->ships() >= need) {
              // this planet has got the balance covered
              if (diff == 0) {
//                std::cerr << "    fully reinforced (in the nick of time) | from: " << source->id() << " ships: " << need << " time: " << time << "\n";
                need = 0;
                issue_order(source->id(), closest_ally->id(), need);
              } else {
//                std::cerr << "    fully reinforced (with reserves) | from: " << source->id() << " reserves: " << need << " time: " << time << "\n";
                need -= source->reserve(need);
              }
            } else {
              // we'll need more backup after this planet
              if (diff == 0){
                // send everything this planet's got
//                std::cerr << "    partially reinforced (in the nick of time) | from: " << source->id() << " ships: " << source->ships() << " time: " << time << "\n";
                need -= source->ships();
                issue_order(source->id(), closest_ally->id(), source->ships());
              } else {
                // this planet is close enough to form reserves
//                std::cerr << "    partially reinforced (with reserves) | from: " << source->id() << " reserves: " << source->ships() << " comittment: " << std::min(source->growth_rate() * diff - source->commitment(), enemy_planet->ships()) << " time: " << time << "\n";
                need -= source->reserve(source->ships());
                need -= source->commit(std::min(source->growth_rate() * diff - source->commitment(), need));
              }
            }
          }
        } else {
//          std::cerr << "  no source\n";
          break;
        }
      }
    }
  }


//  std::cerr << "*** Reserve against future invaders invading a second target ***\n";
  for (int i = 0; i < _enemy_fleets.size(); ++i ){
    pw::fleet* enemy_fleet = _enemy_fleets[i];
    pw::planet before_invasion = enemy_fleet->destination()->in(enemy_fleet->time_remaining());
    pw::planet after_invasion = enemy_fleet->destination()->in(enemy_fleet->time_remaining());
    if (after_invasion.owner() > 1) {
//      std::cerr << "spare troops from fleet size: " << enemy_fleet->ships() << " destination: " << enemy_fleet->destination()->id() << "\n";
      // TODO: this should be the closest ally after this enemy_fleet lands, not now
      pw::planet* closest_ally = enemy_fleet->destination()->closest_ally();
      if (closest_ally) {
//        std::cerr << "closest ally: " << closest_ally->id() << "\n";
        int time = enemy_fleet->destination()->time_to(*closest_ally) + enemy_fleet->time_remaining();
        if (before_invasion.owner() == after_invasion.owner()) {
          closest_ally->commit(enemy_fleet->ships(), closest_ally, time);
        } else {
          closest_ally->commit(after_invasion.ships(), closest_ally, time);
        }
      }
    }
  }

  // make sure that at least half the homeworlds ships are reserved before attacking
  // this is a crappy fix for over spreading on turn 1 without enough info on enemy commitments
*/
  if (_turn == 1) {
//    std::cerr << "*** Homeworld early turn reserves ***\n";
    pw::planet* homeworld = _allied_planets[0];
    homeworld->reserve(homeworld->ships());// - (50 / _turn), 0));
  }


  // attack their planets
  // TODO attackers are still being outraced by their closer allies, not a big deal since it's usually more men in the front line
//  std::cerr << "*** Attacking ***\n";
  for (int i = 0; i < _allied_planets.size(); ++i ){
    pw::planet* source = _allied_planets[i];
//    std::cerr << "  source: " << source->id() << "\n";
    while (source->ships() > 0) {
      pw::planet* destination = NULL;
      double highest_value = 0;

      // find the destination with the highest value
      for (size_t j = 0; j < _planets.size(); ++j) {
        pw::planet* planet = _planets[j];
        if (planet->id() == source->id()) {
          continue;
        }
        double time = source->time_to(*planet);
        pw::planet planet_at_arrival = planet->in(time);
//        std::cerr << "  target:" << planet_at_arrival.id() << " owner: " << planet_at_arrival.owner() << " time: " << time << " ships: " << planet_at_arrival.ships() << " growth: " << planet_at_arrival.growth_rate() << " value: " << planet_at_arrival.value() / time << "\n";
        double value = planet_at_arrival.value() / pow(time,2);
        if (planet_at_arrival.ships() < source->ships() && planet_at_arrival.owner() == 0) {
          pw::planet* closest_enemy = planet->closest_enemy();
          if (closest_enemy) {
            value *= 16 * ((double) planet->time_to(*closest_enemy) / time);
          }
        }
        if (value > highest_value) {
          destination = planet;
          highest_value = value;
        }
      }

      // find the min ships we need to send
      if (destination == NULL || destination->id() == source->id()) {
//        std::cerr << "    no destination\n:";
        break;
      } else {

        pw::planet d = destination->in(source->time_to(*destination));
        int ships = d.ships() + 1 + (d.owner() == 2 ? d.growth_rate() : 0);
        for (int i = 0; i < _enemy_fleets.size(); ++i ){
          pw::fleet* enemy_fleet = _enemy_fleets[i];
          if (enemy_fleet->destination()->id() == destination->id() && enemy_fleet->time_remaining() > source->time_to(*destination)){
            ships += enemy_fleet->ships();
          }
        }
//        std::cerr << "    got target: " << destination->id() << " (" << highest_value << ")\n";
        source->launch(ships, destination);
      }
    }
  }
  finish_turn();
}
