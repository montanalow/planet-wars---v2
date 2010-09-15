#include <algorithm>
#include <iostream>
#include <fstream>


#include "game_state.h"
#include "util.h"

void make_move(pw::game_state& game_state) {
  std::vector<pw::planet> sources = game_state.allied_planets();
  std::vector<pw::planet> destinations = game_state.planets();
  for (size_t i = 0; i < sources.size(); ++i) {
    pw::planet& source = sources[i];
//    std::cerr << "source: " << source.id() << "\n";
    while (source.ships() > 1) {
      pw::planet* destination = NULL;
      double highest_value = 0;

      // find the destination with the highest value
      for (size_t j = 0; j < destinations.size(); ++j) {
        pw::planet& planet = destinations[j];
        double time = source.time_to(planet);
        pw::planet planet_at_arrival = planet.in(time);
//        std::cerr << "  planet:" << planet.id() << " time: " << time << "\n";
        if (planet_at_arrival.value() / (time / 2) > highest_value) {
          destination = &planet;
          highest_value = planet_at_arrival.value() / (time / 2);
        }
      }

      // find the min ships we need to send
      if (destination == NULL || destination->id() == source.id()) {
//        std::cerr << " no destination\n:";
        break;
      } else {
//        std::cerr << "destination: " << destination->id() << "\n";
        pw::planet d = destination->in(source.time_to(*destination));
        int ships = std::min(source.ships() - 1, d.ships() + 1);
        game_state.issue_order(source, *destination, ships);
      }
    }
  }



//  // 1) calculate future game states
//  std::vector<pw::game_state> game_states;
//  game_states.push_back(game_state);
//  while(game_state.max_fleet_time_remaining() > 0) {
//    game_states.push_back(++game_state);
//  }
//
//  // 2) find the largest forseable change in production for the cheapest ships in the closest time
//  pw::planet* target = NULL;
//  double target_value = 0;
//  int time = 0;
//  for (std::vector<pw::game_state>::const_iterator game_state = game_states.begin(); game_state < game_states.end(); ++game_state) {
//    for (std::vector<pw::planet>::const_iterator planet = game_state->allied_planets().begin(); planet < game_state->allied_planets().end(); ++planet) {
//      if (planet->owner() != 1) {
//        if (planet->growth_rate() / (planet->ships() * time + 1.0) > target_value) {
//          target_value = planet->growth_rate() / (planet->ships() * time + 1.0); // TODO denominator should be sum(source ships * source time to this destination)
//          target = &(*planet);
//        }
//      }
//    }
//    ++time;
//  }
//
//  if (target) {
//
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
//      double value = planet->growth_rate() * planet->owner() * (game_state.max_fleet_time_remaining() - time);
//    }
//  }
}

// This is just the main game loop that takes care of communicating with the
// game engine for you. You don't have to understand or change the code below.
int main(int argc, char *argv[]) {
  //std::cerr << "startup\n";
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
