#include "fleet.h"

pw::fleet::fleet(int owner, int ships, pw::planet& source, pw::planet& destination, int total_trip_time, int time_remaining, pw::game_state& game_state) :
  _owner(owner), _ships(ships), _source(source), _destination(destination), _total_trip_time(total_trip_time), _time_remaining(time_remaining), _game_state(game_state) {
  if (_time_remaining > _game_state._max_fleet_time_remaining) {
    _game_state._max_fleet_time_remaining = _time_remaining;
  }
}

int pw::fleet::owner() const {
  return _owner;
}

int pw::fleet::ships() const {
  return _ships;
}

const pw::planet& pw::fleet::source() const {
  return _source;
}

const pw::planet& pw::fleet::destination() const {
  return _destination;
}

int pw::fleet::total_trip_time() const {
  return _total_trip_time;
}

int pw::fleet::time_remaining() const {
  return _time_remaining;
}

void pw::fleet::time_remaining(int time_remaining) {
  _time_remaining = time_remaining;
}

void pw::fleet::source(pw::planet& source){
  _source = source;
}

void pw::fleet::destination(pw::planet& destination){
  _destination = destination;
}

void pw::fleet::game_state(const pw::game_state& game_state) {
  _game_state = game_state;
  _source = _game_state.planets()[_source.id()];
  _destination = _game_state.planets()[_destination.id()];
  if (_time_remaining > _game_state._max_fleet_time_remaining) {
    _game_state._max_fleet_time_remaining = _time_remaining;
  }
}

const pw::fleet& pw::fleet::operator=(const pw::fleet& fleet) {
  _owner = fleet._owner;
  _ships = fleet._ships;
  _source = fleet._source;
  _destination = fleet._destination;
  _total_trip_time = fleet._total_trip_time;
  _time_remaining = fleet._time_remaining;
  _game_state = fleet._game_state;
  return *this;
}
