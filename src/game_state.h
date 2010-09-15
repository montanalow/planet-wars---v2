// This file contains helper code that does all the boring stuff for you.
// The code in this file takes care of storing lists of planets and fleets, as
// well as communicating with the game engine. You can get along just fine
// without ever looking at this file. However, you are welcome to modify it
// if you want to.
#ifndef PLANET_WARS_H_
#define PLANET_WARS_H_

namespace pw {class game_state;}

#include <string>
#include <vector>

#include "fleet.h"
#include "planet.h"

namespace pw {

class game_state {
public:
  // Initializes the game state given a string containing game state data.
  game_state(const std::string& game_state_date);
  game_state(const game_state& game_state);
  
  // Returns a list of all the planets.
  const std::vector<pw::planet>& planets() const;
  // Return a list of planets owned by the player
  const std::vector<pw::planet>& allied_planets() const;
  // Return a list of planets owned by the neurtal party
  const std::vector<pw::planet>& neutral_planets() const;
  // Return a list of planets owned by the enemy
  const std::vector<pw::planet>& enemy_planets() const;

  // Return a list of all the fleets.
  const std::vector<pw::fleet>& fleets() const;
  // Return a list of fleets owned by the player
  const std::vector<pw::fleet>& allied_fleets() const;
  // Return a list of fleets owned by the enemy
  const std::vector<pw::fleet>& enemy_fleets() const;

  int max_fleet_time_remaining() const;
  
  // Writes a string which represents the current game state. This string
  // conforms to the Point-in-Time format from the project Wiki.
  std::string to_string() const;

  // Sends an order to the game engine. The order is to send ships ships
  // from source_planet to destination_planet. The order must be valid, or
  // else your bot will get kicked and lose the game. For example, you must own
  // source_planet, and you can't send more ships than you actually have on
  // that planet.
  void issue_order(pw::planet& source, pw::planet& destination, int ships);

  // Sends a message to the game engine letting it know that you're done
  // issuing orders for now.
  void finish_turn() const;

  game_state operator++(int); // postfix
  game_state& operator++(); // prefix
  game_state& operator=(const game_state& game_state);

private:
  void clear();
  void index();

  // Parses a game state from a string. On success, returns 1. On failure,
  // returns 0.
  int parse_game_state_data(const std::string& game_state_data);

  // Store all the planets and fleets.
  std::vector<pw::planet> _planets;
  std::vector<pw::fleet> _fleets;

  // indexes
  std::vector<pw::planet> _allied_planets;
  std::vector<pw::planet> _neutral_planets;
  std::vector<pw::planet> _enemy_planets;
  std::vector<pw::fleet> _allied_fleets;
  std::vector<pw::fleet> _enemy_fleets;
  int _max_fleet_time_remaining;
};

}
#endif
