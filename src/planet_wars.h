// This file contains helper code that does all the boring stuff for you.
// The code in this file takes care of storing lists of planets and fleets, as
// well as communicating with the game engine. You can get along just fine
// without ever looking at this file. However, you are welcome to modify it
// if you want to.
#ifndef PLANET_WARS_H_
#define PLANET_WARS_H_

#include <string>
#include <vector>

// This is a utility class that parses strings.
class string_util {
 public:
  // Tokenizes a string s into tokens. Tokens are delimited by any of the
  // characters in delimiters. Blank tokens are omitted.
  static void tokenize(const std::string& s, const std::string& delimiters, std::vector<std::string>& tokens);

  // A more convenient way of calling the Tokenize() method.
  static std::vector<std::string> tokenize(const std::string& s, const std::string& delimiters = std::string(" "));
};

// This class stores details about one fleet. There is one of these classes
// for each fleet that is in flight at any given time.
class fleet {
 public:
  // Initializes a fleet.
  fleet(int owner,
        int num_ships,
        int source_planet = -1,
        int destination_planet = -1,
        int total_trip_length = -1,
        int turns_remaining = -1);

  // Returns the playerID of the owner of the fleet. Your player ID is always
  // 1. So if the owner is 1, you own the fleet. If the owner is 2 or some
  // other number, then this fleet belongs to your enemy.
  int owner() const;

  // Returns the number of ships that comprise this fleet.
  int num_ships() const;

  // Returns the ID of the planet where this fleet originated.
  int source_planet() const;

  // Returns the ID of the planet where this fleet is headed.
  int destination_planet() const;

  // Returns the total distance that is being traveled by this fleet. This
  // is the distance between the source planet and the destination planet,
  // rounded up to the nearest whole number.
  int total_trip_length() const;

  // Returns the number of turns until this fleet reaches its destination. If
  // this value is 1, then the fleet will hit the destination planet next turn.
  int turns_remaining() const;

 private:
  int _owner;
  int _num_ships;
  int _source_planet;
  int _destination_planet;
  int _total_trip_length;
  int _turns_remaining;
};

// Stores information about one planet. There is one instance of this class
// for each planet on the map.
class planet {
 public:
  // Initializes a planet.
  planet(int planet_id,
         int owner,
         int num_ships,
         int growth_rate,
         double x,
         double y);

  // Returns the ID of this planets. Planets are numbered starting at zero.
  int planet_id() const;

  // Returns the ID of the player that owns this planet. Your playerID is
  // always 1. If the owner is 1, this is your planet. If the owner is 0, then
  // the planet is neutral. If the owner is 2 or some other number, then this
  // planet belongs to the enemy.
  int owner() const;

  // The number of ships on the planet. This is the "population" of the planet.
  int num_ships() const;

  // Returns the growth rate of the planet. Unless the planet is neutral, the
  // population of the planet grows by this amount each turn. The higher this
  // number is, the faster this planet produces ships.
  int growth_rate() const;

  // The position of the planet in space.
  double x() const;
  double y() const;

  // Use the following functions to set the properties of this planet. Note
  // that these functions only affect your program's copy of the game state.
  // You can't steal your opponent's planets just by changing the owner to 1
  // using the Owner(int) function! :-)
  void owner(int new_owner);
  void num_ships(int new_num_ships);
  void add_ships(int amount);
  void remove_ships(int amount);

 private:
  int _planet_id;
  int _owner;
  int _num_ships;
  int _growth_rate;
  double _x, _y;
};

class planet_wars {
 public:
  // Initializes the game state given a string containing game state data.
  planet_wars(const std::string& game_state);

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
  void issue_order(int source_planet,
		  int destination_planet,
		  int num_ships) const;

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
  int parse_game_state(const std::string& s);

  // Store all the planets and fleets. OMG we wouldn't wanna lose all the
  // planets and fleets, would we!?
  std::vector<planet> _planets;
  std::vector<fleet> _fleets;
};

#endif
