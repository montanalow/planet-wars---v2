/* 
 * File:   fleet.h
 * Author: montana
 *
 * Created on September 11, 2010, 5:05 PM
 */

#ifndef FLEET_H
#define	FLEET_H

#include "planet.h"
#include "game_state.h"

namespace pw {

// This class stores details about one fleet. There is one of these classes
// for each fleet that is in flight at any given time.
class fleet {
public:
  // Initializes a fleet.
  fleet(
    int owner,
    int num_ships,
    int source_planet,
    int destination_planet,
    int total_trip_length,
    int turns_remaining,
    pw::game_state* game_state
  );

  // Returns the playerID of the owner of the fleet. Your player ID is always
  // 1. So if the owner is 1, you own the fleet. If the owner is 2 or some
  // other number, then this fleet belongs to your enemy.
  int owner() const;

  // Returns the number of ships that comprise this fleet.
  int num_ships() const;

  // Returns the ID of the planet where this fleet originated.
  int source_planet() const;
  const planet* source() const;

  // Returns the ID of the planet where this fleet is headed.
  int destination_planet() const;
  const planet* destination() const;

  // Calculates the state of the game when this fleet arrives at it's destination
  void extrapolate();

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
  const planet* _source;
  const planet* _destination;
  int _total_trip_length;
  int _turns_remaining;
};

}

#endif	/* FLEET_H */
