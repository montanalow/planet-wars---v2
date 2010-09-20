#include "fleet.h"

pw::fleet::fleet(int owner, int ships, pw::planet* source, pw::planet* destination, int total_time, int travel_remaining, int commitment_remaining, pw::game_state* game_state) :
  _owner(owner), _ships(ships), _source(source), _destination(destination), _total_time(total_time), _travel_remaining(travel_remaining), _commitment_remaining(commitment_remaining), _game_state(game_state) {
}

int pw::fleet::owner() const {
  return _owner;
}

int pw::fleet::ships() const {
  return _ships;
}

pw::planet* pw::fleet::source() {
  return _source;
}

 pw::planet* pw::fleet::destination() {
  return _destination;
}

int pw::fleet::total_time() const {
  return _total_time;
}

int pw::fleet::time_remaining() const {
  return _travel_remaining + _commitment_remaining;
}

int pw::fleet::commitment_remaining() const {
  return _commitment_remaining;
}

bool pw::fleet::just_launched() const {
  // not reserves, and not commitments, and not prexisting fleets
  return _travel_remaining > 0 && _commitment_remaining == 0 && _total_time == _travel_remaining;
}

bool pw::fleet::is_commitment() const {
  // not reserves, and not commitments, and not prexisting fleets
  return _commitment_remaining > 0;
}

void pw::fleet::source(pw::planet* source){
  _source = source;
}

void pw::fleet::destination(pw::planet* destination){
  _destination = destination;
}

void pw::fleet::game_state(pw::game_state* game_state) {
  _game_state = game_state;
  _source = _game_state->planets()[_source->id()];
  _destination = _game_state->planets()[_destination->id()];
}

const pw::fleet& pw::fleet::operator=(const pw::fleet& fleet) {
  _owner = fleet._owner;
  _ships = fleet._ships;
  _source = fleet._source;
  _destination = fleet._destination;
  _total_time = fleet._total_time;
  _travel_remaining = fleet._travel_remaining;
  _game_state = fleet._game_state;
  return *this;
}

bool pw::fleet::operator<(const pw::fleet& fleet) const {
  return _travel_remaining < fleet._travel_remaining;
}

bool pw::fleet::compare(const pw::fleet* a, const pw::fleet* b) {
    return *a < *b;
}
