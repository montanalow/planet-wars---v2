/* 
 * File:   planet.h
 * Author: montana
 *
 * Created on September 11, 2010, 5:02 PM
 */

#ifndef PLANET_H
#define	PLANET_H

namespace pw {class planet;}

#include "game_state.h"

namespace pw {

// Stores information about one planet. There is one instance of this class
// for each planet on the map.

class planet {
public:
  // Initializes a planet.
  planet(
    int id,
    double x,
    double y,
    int owner,
    int num_ships,
    int growth_rate,
    pw::game_state* game_state
  );

  // Returns the ID of this planets. Planets are numbered starting at zero.
  int id() const;

  // The position of the planet in space.
  double x() const;
  double y() const;

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

  // Use the following functions to set the properties of this planet. Note
  // that these functions only affect your program's copy of the game state.
  // You can't steal your opponent's planets just by changing the owner to 1
  // using the Owner(int) function! :-)
  void owner(int new_owner);
  void num_ships(int num_ships);
  void add_ships(int amount);
  void remove_ships(int amount);

  pw::planet in(int turns) const;
  double value_in(int turns) const;

  int turns_to(const pw::planet& planet) const;
  double distance_to(const pw::planet& planet) const;
  double manhattan_distance_to(const pw::planet& planet) const;
  double pythagorean_distance_to(const pw::planet& planet) const;
  double pythagorean_distance_squared_to(const pw::planet& planet) const;


private:
  int _id;
  double _x, _y;
  int _owner;
  int _num_ships;
  int _growth_rate;
  pw::game_state* _game_state;
};

}

#endif	/* PLANET_H */
