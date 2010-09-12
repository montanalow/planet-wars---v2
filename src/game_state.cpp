#include <cmath>
#include <cstdlib>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

#include "game_state.h"
#include "util.h"

pw::game_state::game_state(const std::string& game_state_data) {
  parse_game_state_data(game_state_data);
}

int pw::game_state::num_planets() const {
  return _planets.size();
}

const pw::planet& pw::game_state::get_planet(int id) const {
  return _planets[id];
}

int pw::game_state::num_fleets() const {
  return _fleets.size();
}

const pw::fleet& pw::game_state::get_fleet(int id) const {
  return _fleets[id];
}

std::vector<pw::planet> pw::game_state::planets() const {
  std::vector<pw::planet> r;
  for (int i = 0; i < _planets.size(); ++i) {
    const pw::planet& p = _planets[i];
    r.push_back(p);
  }
  return r;
}

std::vector<pw::planet> pw::game_state::my_planets() const {
  std::vector<pw::planet> r;
  for (int i = 0; i < _planets.size(); ++i) {
    const pw::planet& p = _planets[i];
    if (p.owner() == 1) {
      r.push_back(p);
    }
  }
  return r;
}

std::vector<pw::planet> pw::game_state::neutral_planets() const {
  std::vector<pw::planet> r;
  for (int i = 0; i < _planets.size(); ++i) {
    const pw::planet& p = _planets[i];
    if (p.owner() == 0) {
      r.push_back(p);
    }
  }
  return r;
}

std::vector<pw::planet> pw::game_state::enemy_planets() const {
  std::vector<pw::planet> r;
  for (int i = 0; i < _planets.size(); ++i) {
    const pw::planet& p = _planets[i];
    if (p.owner() > 1) {
      r.push_back(p);
    }
  }
  return r;
}

std::vector<pw::planet> pw::game_state::not_my_planets() const {
  std::vector<pw::planet> r;
  for (int i = 0; i < _planets.size(); ++i) {
    const pw::planet& p = _planets[i];
    if (p.owner() != 1) {
      r.push_back(p);
    }
  }
  return r;
}

std::vector<pw::fleet> pw::game_state::fleets() const {
  std::vector<pw::fleet> r;
  for (int i = 0; i < _fleets.size(); ++i) {
    const pw::fleet& f = _fleets[i];
    r.push_back(f);
  }
  return r;
}

std::vector<pw::fleet> pw::game_state::my_fleets() const {
  std::vector<pw::fleet> r;
  for (int i = 0; i < _fleets.size(); ++i) {
    const pw::fleet& f = _fleets[i];
    if (f.owner() == 1) {
      r.push_back(f);
    }
  }
  return r;
}

std::vector<pw::fleet> pw::game_state::enemy_fleets() const {
  std::vector<pw::fleet> r;
  for (int i = 0; i < _fleets.size(); ++i) {
    const pw::fleet& f = _fleets[i];
    if (f.owner() > 1) {
      r.push_back(f);
    }
  }
  return r;
}

std::string pw::game_state::to_string() const {
  std::stringstream s;
  for (unsigned int i = 0; i < _planets.size(); ++i) {
    const planet& p = _planets[i];
    s << "P " << p.x() << " " << p.y() << " " << p.owner()
            << " " << p.num_ships() << " " << p.growth_rate() << std::endl;
  }
  for (unsigned int i = 0; i < _fleets.size(); ++i) {
    const fleet& f = _fleets[i];
    s << "F " << f.owner() << " " << f.num_ships() << " "
            << f.source_planet() << " " << f.destination_planet() << " "
            << f.total_trip_length() << " " << f.turns_remaining() << std::endl;
  }
  return s.str();
}

int pw::game_state::distance(int source_planet, int destination_planet) const {
  const pw::planet& source = _planets[source_planet];
  const pw::planet& destination = _planets[destination_planet];
  double dx = source.x() - destination.x();
  double dy = source.y() - destination.y();
  return (int) ceil(sqrt(dx * dx + dy * dy));
}

void pw::game_state::issue_order(pw::planet& source, pw::planet& destination, int num_ships) {
  source.remove_ships(num_ships);
  fleet f(
    1,
    num_ships,
    source.id(),
    destination.id(),
    source.turns_to(destination),
    source.turns_to(destination),
    this
  );
  _fleets.push_back(f);
  std::cout << source.id() << " " << destination.id() << " " << num_ships << std::endl;
}

bool pw::game_state::is_alive(int player_id) const {
  for (unsigned int i = 0; i < _planets.size(); ++i) {
    if (_planets[i].owner() == player_id) {
      return true;
    }
  }
  for (unsigned int i = 0; i < _fleets.size(); ++i) {
    if (_fleets[i].owner() == player_id) {
      return true;
    }
  }
  return false;
}

int pw::game_state::num_ships(int player_id) const {
  int num_ships = 0;
  for (unsigned int i = 0; i < _planets.size(); ++i) {
    if (_planets[i].owner() == player_id) {
      num_ships += _planets[i].num_ships();
    }
  }
  for (unsigned int i = 0; i < _fleets.size(); ++i) {
    if (_fleets[i].owner() == player_id) {
      num_ships += _fleets[i].num_ships();
    }
  }
  return num_ships;
}

int pw::game_state::parse_game_state_data(const std::string& game_state_data) {
  _planets.clear();
  _fleets.clear();
  std::vector<std::string> lines = string_util::tokenize(game_state_data, "\n");
  int id = 0;
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
      planet p(
        id++, // The ID of this planet
        atof(tokens[1].c_str()), // X
        atof(tokens[2].c_str()), // Y
        atoi(tokens[3].c_str()), // Owner
        atoi(tokens[4].c_str()), // Num ships
        atoi(tokens[5].c_str()), // Growth rate
        this
      );
      _planets.push_back(p);
    } else if (tokens[0] == "F") {
      if (tokens.size() != 7) {
        return 0;
      }
      fleet f(
        atoi(tokens[1].c_str()), // Owner
        atoi(tokens[2].c_str()), // Num ships
        atoi(tokens[3].c_str()), // Source
        atoi(tokens[4].c_str()), // Destination
        atoi(tokens[5].c_str()), // Total trip length
        atoi(tokens[6].c_str()), // Turns remaining
        this
      );
      _fleets.push_back(f);
    } else {
      return 0;
    }
  }
  return 1;
}

void pw::game_state::finish_turn() const {
  std::cout << "go" << std::endl;
  std::cout.flush();
}
