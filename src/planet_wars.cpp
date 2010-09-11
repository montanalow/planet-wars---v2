#include "planet_wars.h"
#include <cmath>
#include <cstdlib>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

void string_util::tokenize(const std::string& s, const std::string& delimiters, std::vector<std::string>& tokens) {
    std::string::size_type lastPos = s.find_first_not_of(delimiters, 0);
    std::string::size_type pos = s.find_first_of(delimiters, lastPos);
    while (std::string::npos != pos || std::string::npos != lastPos) {
        tokens.push_back(s.substr(lastPos, pos - lastPos));
        lastPos = s.find_first_not_of(delimiters, pos);
        pos = s.find_first_of(delimiters, lastPos);
    }
}

std::vector<std::string> string_util::tokenize(const std::string& s, const std::string& delimiters) {
    std::vector<std::string> tokens;
    tokenize(s, delimiters, tokens);
    return tokens;
}

fleet::fleet(int owner,
        int num_ships,
        int source_planet,
        int destination_planet,
        int total_trip_length,
        int turns_remaining) {
    _owner = owner;
    _num_ships = num_ships;
    _source_planet = source_planet;
    _destination_planet = destination_planet;
    _total_trip_length = total_trip_length;
    _turns_remaining = turns_remaining;
}

int fleet::owner() const {
    return _owner;
}

int fleet::num_ships() const {
    return _num_ships;
}

int fleet::source_planet() const {
    return _source_planet;
}

int fleet::destination_planet() const {
    return _destination_planet;
}

int fleet::total_trip_length() const {
    return _total_trip_length;
}

int fleet::turns_remaining() const {
    return _turns_remaining;
}

planet::planet(int planet_id,
        int owner,
        int num_ships,
        int growth_rate,
        double x,
        double y) {
    _planet_id = planet_id;
    _owner = owner;
    _num_ships = num_ships;
    _growth_rate = growth_rate;
    _x = x;
    _y = y;
}

int planet::planet_id() const {
    return _planet_id;
}

int planet::owner() const {
    return _owner;
}

int planet::num_ships() const {
    return _num_ships;
}

int planet::growth_rate() const {
    return _growth_rate;
}

double planet::x() const {
    return _x;
}

double planet::y() const {
    return _y;
}

void planet::owner(int new_owner) {
    _owner = new_owner;
}

void planet::num_ships(int new_num_ships) {
    _num_ships = new_num_ships;
}

void planet::add_ships(int amount) {
    _num_ships += amount;
}

void planet::remove_ships(int amount) {
    _num_ships -= amount;
}

planet_wars::planet_wars(const std::string& gameState) {
    parse_game_state(gameState);
}

int planet_wars::num_planets() const {
    return _planets.size();
}

const planet& planet_wars::get_planet(int planet_id) const {
    return _planets[planet_id];
}

int planet_wars::num_fleets() const {
    return _fleets.size();
}

const fleet& planet_wars::get_fleet(int fleet_id) const {
    return _fleets[fleet_id];
}

std::vector<planet> planet_wars::planets() const {
    std::vector<planet> r;
    for (int i = 0; i < _planets.size(); ++i) {
        const planet& p = _planets[i];
        r.push_back(p);
    }
    return r;
}

std::vector<planet> planet_wars::my_planets() const {
    std::vector<planet> r;
    for (int i = 0; i < _planets.size(); ++i) {
        const planet& p = _planets[i];
        if (p.owner() == 1) {
            r.push_back(p);
        }
    }
    return r;
}

std::vector<planet> planet_wars::neutral_planets() const {
    std::vector<planet> r;
    for (int i = 0; i < _planets.size(); ++i) {
        const planet& p = _planets[i];
        if (p.owner() == 0) {
            r.push_back(p);
        }
    }
    return r;
}

std::vector<planet> planet_wars::enemy_planets() const {
    std::vector<planet> r;
    for (int i = 0; i < _planets.size(); ++i) {
        const planet& p = _planets[i];
        if (p.owner() > 1) {
            r.push_back(p);
        }
    }
    return r;
}

std::vector<planet> planet_wars::not_my_planets() const {
    std::vector<planet> r;
    for (int i = 0; i < _planets.size(); ++i) {
        const planet& p = _planets[i];
        if (p.owner() != 1) {
            r.push_back(p);
        }
    }
    return r;
}

std::vector<fleet> planet_wars::fleets() const {
    std::vector<fleet> r;
    for (int i = 0; i < _fleets.size(); ++i) {
        const fleet& f = _fleets[i];
        r.push_back(f);
    }
    return r;
}

std::vector<fleet> planet_wars::my_fleets() const {
    std::vector<fleet> r;
    for (int i = 0; i < _fleets.size(); ++i) {
        const fleet& f = _fleets[i];
        if (f.owner() == 1) {
            r.push_back(f);
        }
    }
    return r;
}

std::vector<fleet> planet_wars::enemy_fleets() const {
    std::vector<fleet> r;
    for (int i = 0; i < _fleets.size(); ++i) {
        const fleet& f = _fleets[i];
        if (f.owner() > 1) {
            r.push_back(f);
        }
    }
    return r;
}

std::string planet_wars::to_string() const {
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

int planet_wars::distance(int source_planet, int destination_planet) const {
    const planet& source = _planets[source_planet];
    const planet& destination = _planets[destination_planet];
    double dx = source.x() - destination.x();
    double dy = source.y() - destination.y();
    return (int) ceil(sqrt(dx * dx + dy * dy));
}

void planet_wars::issue_order(int source_planet,
        int destination_planet,
        int num_ships) const {
    std::cout << source_planet << " "
            << destination_planet << " "
            << num_ships << std::endl;
    std::cout.flush();
}

bool planet_wars::is_alive(int player_id) const {
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

int planet_wars::num_ships(int player_id) const {
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

int planet_wars::parse_game_state(const std::string& s) {
    _planets.clear();
    _fleets.clear();
    std::vector<std::string> lines = string_util::tokenize(s, "\n");
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
            planet p(planet_id++, // The ID of this planet
                    atoi(tokens[3].c_str()), // Owner
                    atoi(tokens[4].c_str()), // Num ships
                    atoi(tokens[5].c_str()), // Growth rate
                    atof(tokens[1].c_str()), // X
                    atof(tokens[2].c_str())); // Y
            _planets.push_back(p);
        } else if (tokens[0] == "F") {
            if (tokens.size() != 7) {
                return 0;
            }
            fleet f(atoi(tokens[1].c_str()), // Owner
                    atoi(tokens[2].c_str()), // Num ships
                    atoi(tokens[3].c_str()), // Source
                    atoi(tokens[4].c_str()), // Destination
                    atoi(tokens[5].c_str()), // Total trip length
                    atoi(tokens[6].c_str())); // Turns remaining
            _fleets.push_back(f);
        } else {
            return 0;
        }
    }
    return 1;
}

void planet_wars::finish_turn() const {
    std::cout << "go" << std::endl;
    std::cout.flush();
}
