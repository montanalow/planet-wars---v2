#include <algorithm>
#include <iostream>
#include <fstream>


#include "game_state.h"
#include "util.h"

void make_move(pw::game_state& game_state) {
  std::vector<pw::planet> sources = game_state.planets();
  std::vector<pw::planet> destinations = game_state.planets();
  for (size_t i = 0; i < sources.size(); ++i) {
    pw::planet& source = sources[i];
    pw::logger::log << "source: " << source.id() << "\n";
    pw::logger::log.flush();
    if (source.owner() != 1) {
      continue;
    }
    while (source.ships() > 0) {
      pw::logger::log << " ships:" << source.ships() << "\n";
      pw::logger::log.flush();
      
      pw::planet* destination = NULL;
      double highest_value = -9999999;

      // find the destination with the highest value
      for (size_t j = 0; j < destinations.size(); ++j) {
        pw::planet& planet = destinations[j];
        double value = planet.value_in(source.time_to(planet));
        if (value > highest_value) {
          destination = &planet;
          highest_value = value;
        }
      }
      pw::logger::log << " a:" << source.ships() << "\n";
      pw::logger::log.flush();

      // do we have enough shipsfind the min ships we need to send
      pw::planet d = destination->in(source.time_to(*destination));
      int ships = std::min(source.ships(), d.ships() + 1);
      if (destination == NULL || destination->id() == source.id()) {
        pw::logger::log << "break\n";
        pw::logger::log.flush();
        break;
      } else {
        pw::logger::log << "source: " << source.id() << " | destination: " << destination->id() << "\n";
        pw::logger::log.flush();
        game_state.issue_order(source, *destination, ships);
      }
    }
  }



  // find objectives for all known future game states
  // 1) calculate future game states
//  std::vector<pw::game_state> game_states;
//  game_states.push_back(game_state);
//  while(game_state._max_fleet_time_remaining > 0) {
//    game_states.push_back(++game_state);
//  }
//
//  for (size_t time = 0; time < game_states.size(); ++time) {
//    pw::game_state& game_state = game_states[time];
//    for (std::vector<pw::planet>::const_iterator planet = game_state.planets().begin(); planet < game_state.planets().end(); ++planet) {
//      int cost = 0;
//      switch (planet->owner()) {
//        case 0:
//          cost = planet->ships() * 2 * time;
//          break;
//        case 1:
//          cost = time;
//          break;
//        case 2:
//          cost = planet->ships() * time;
//          break;
//      }
//      double target_value = planet->growth_rate() * (planet->owner() + 1) - time;
//    }
//  }
}

// This is just the main game loop that takes care of communicating with the
// game engine for you. You don't have to understand or change the code below.
int main(int argc, char *argv[]) {
  pw::logger::log << "startup\n";
  std::string current_line;
  std::string game_state_data;
  while (true) {
    int c = std::cin.get();
    current_line += (char) c;
    if (c == '\n') {
      if (current_line.length() >= 2 && current_line.substr(0, 2) == "go") {
        pw::game_state game_state(game_state_data);
        game_state_data = "";
        make_move(game_state);
        game_state.finish_turn();
      } else {
        game_state_data += current_line;
      }
      current_line = "";
    }
  }
  return 0;
}
