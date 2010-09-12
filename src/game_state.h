// This file contains helper code that does all the boring stuff for you.
// The code in this file takes care of storing lists of planets and fleets, as
// well as communicating with the game engine. You can get along just fine
// without ever looking at this file. However, you are welcome to modify it
// if you want to.
#ifndef PLANET_WARS_H_
#define PLANET_WARS_H_

#include <string>
#include <vector>

namespace pw {class game_state;}

#include "fleet.h"
#include "planet.h"

namespace pw {


class game_state {
public:
  // Initializes the game state given a string containing game state data.
  game_state(const std::string& game_state_date);

  // Returns the number of planets on the map. Planets are numbered starting
  // with 0.
  int num_planets() const;

  // Returns the planet with the given planet_id. There are NumPlanets()
  // planets. They are numbered starting at 0.
  const planet& get_planet(int planet_id) const;

  // Returns the number of fleets.
  int num_fleets() const;

  // Returns the fleet with the given fleet_id. Fleets are numbered starting
  // with 0. There are NumFleets() fleets. fleet_id's are not consistent from
  // one turn to the next.
  const fleet& get_fleet(int fleet_id) const;

  // Returns a list of all the planets.
  std::vector<planet> planets() const;

  // Return a list of all the planets owned by the current player. By
  // convention, the current player is always player number 1.
  std::vector<planet> my_planets() const;

  // Return a list of all neutral planets.
  std::vector<planet> neutral_planets() const;

  // Return a list of all the planets owned by rival players. This excludes
  // planets owned by the current player, as well as neutral planets.
  std::vector<planet> enemy_planets() const;

  // Return a list of all the planets that are not owned by the current
  // player. This includes all enemy planets and neutral planets.
  std::vector<planet> not_my_planets() const;

  // Return a list of all the fleets.
  std::vector<fleet> fleets() const;

  // Return a list of all the fleets owned by the current player.
  std::vector<fleet> my_fleets() const;

  // Return a list of all the fleets owned by enemy players.
  std::vector<fleet> enemy_fleets() const;

  // Writes a string which represents the current game state. This string
  // conforms to the Point-in-Time format from the project Wiki.
  std::string to_string() const;

  // Returns the distance between two planets, rounded up to the next highest
  // integer. This is the number of discrete time steps it takes to get between
  // the two planets.
  int distance(int source_planet, int destination_planet) const;

  // Sends an order to the game engine. The order is to send num_ships ships
  // from source_planet to destination_planet. The order must be valid, or
  // else your bot will get kicked and lose the game. For example, you must own
  // source_planet, and you can't send more ships than you actually have on
  // that planet.
  void issue_order(pw::planet& source, pw::planet& destination, int num_ships);

  // Returns true if the named player owns at least one planet or fleet.
  // Otherwise, the player is deemed to be dead and false is returned.
  bool is_alive(int player_id) const;

  // Returns the number of ships that the given player has, either located
  // on planets or in flight.
  int num_ships(int player_id) const;

  // Sends a message to the game engine letting it know that you're done
  // issuing orders for now.
  void finish_turn() const;

private:
  // Parses a game state from a string. On success, returns 1. On failure,
  // returns 0.
  int parse_game_state_data(const std::string& game_state_data);

  // Store all the planets and fleets. OMG we wouldn't wanna lose all the
  // planets and fleets, would we!?
  std::vector<planet> _planets;
  std::vector<fleet> _fleets;
};

}
#endif
