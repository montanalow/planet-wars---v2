/* 
 * File:   fleet.h
 * Author: montana
 *
 * Created on September 11, 2010, 5:05 PM
 */

#ifndef FLEET_H
#define	FLEET_H

namespace pw{class fleet;}

#include "game_state.h"
#include "planet.h"

namespace pw {

// This class stores details about one fleet. There is one of these classes
// for each fleet that is in flight at any given time.
class fleet {
public:
  // Initializes a fleet.
  fleet(int owner, int ships, pw::planet& source, pw::planet& destination, int total_trip_time, int time_remaining, const pw::game_state* game_state);

  // Returns the playerID of the owner of the fleet. Your player ID is always
  // 1. So if the owner is 1, you own the fleet. If the owner is 2 or some
  // other number, then this fleet belongs to your enemy.
  int owner() const;

  // Returns the number of ships that comprise this fleet.
  int ships() const;

  // Returns the ID of the planet where this fleet originated.
  const pw::planet& source() const;

  // Returns the ID of the planet where this fleet is headed.
  const pw::planet& destination() const;

  // Returns the total trip time that is being traveled by this fleet. This
  // is the distance between the source planet and the destination planet,
  // rounded up to the nearest whole number.
  int total_trip_time() const;

  // Returns the number of turns until this fleet reaches its destination. If
  // this value is 1, then the fleet will hit the destination planet next turn.
  int time_remaining() const;

  void time_remaining(int time_remaining);
  void source(pw::planet& source);
  void destination(pw::planet& destination);
  void game_state(const pw::game_state* game_state);

  const pw::fleet& operator=(const pw::fleet& fleet);

private:
  int _owner;
  int _ships;
  pw::planet& _source;
  pw::planet& _destination;
  int _total_trip_time;
  int _time_remaining;
  const pw::game_state* _game_state;
};

}

#endif	/* FLEET_H */
