#include "game_state.h"
#include "fleet.h"
pw::fleet::fleet(
  int owner,
  int num_ships,
  int source_id,
  int destination_id,
  int total_trip_length,
  int turns_remaining,
  pw::game_state* game_state
) :
  _owner(owner),
  _num_ships(num_ships),
  _source(&game_state->get_planet(source_id)),
  _destination(&game_state->get_planet(destination_id)),
  _total_trip_length(total_trip_length),
  _turns_remaining(turns_remaining) {
}

int pw::fleet::owner() const {
  return _owner;
}

int pw::fleet::num_ships() const {
  return _num_ships;
}

int pw::fleet::source_planet() const {
  return _source->id();
}

const pw::planet* pw::fleet::source() const {
  return _source;
}

int pw::fleet::destination_planet() const {
  return _destination->id();
}

const pw::planet* pw::fleet::destination() const {
  return _destination;
}

void pw::fleet::extrapolate() {
//  if( destination )
//  destination()->remove_ships( fleet.num_ships() );
//
}

int pw::fleet::total_trip_length() const {
  return _total_trip_length;
}

int pw::fleet::turns_remaining() const {
  return _turns_remaining;
}

