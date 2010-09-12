#include <algorithm>
#include <iostream>
#include <fstream>


#include "game_state.h"
#include "util.h"

void make_move(pw::game_state& game_state, std::ofstream& logger) {
  std::vector<pw::planet> sources = game_state.my_planets();
  std::vector<pw::planet> destinations = game_state.planets();
  for (int i = 0; i < sources.size(); ++i) {
    pw::planet& source = sources[i];
    while (source.num_ships() > 0) {
      pw::planet* destination = NULL;
      double highest_value = -9999999;

      // find the destination with the highest value
      for (int i = 0; i < destinations.size(); ++i) {
        pw::planet& planet = destinations[i];
        double value = planet.value_in(source.turns_to(planet));

        if (value > highest_value) {
          destination = &planet;
          highest_value = value;
        }
      }

      // find the min ships we need to send
      pw::planet d = destination->in(source.turns_to(*destination));
      int num_ships = std::min(
        source.num_ships(),
        d.num_ships() + 1
      );
      
      if (destination == NULL || destination->id() == source.id()) {
        break;
      } else {
        game_state.issue_order(source, *destination, num_ships);
      }
    }
  }
}

// This is just the main game loop that takes care of communicating with the
// game engine for you. You don't have to understand or change the code below.
int main(int argc, char *argv[]) {
  std::ofstream logger("debug.log", std::ios::app);
  std::string current_line;
  std::string game_state_data;
  while (true) {
    int c = std::cin.get();
    current_line += (char) c;
    if (c == '\n') {
      if (current_line.length() >= 2 && current_line.substr(0, 2) == "go") {
        pw::game_state game_state(game_state_data);
        game_state_data = "";
        make_move(game_state, logger);
        game_state.finish_turn();
      } else {
        game_state_data += current_line;
      }
      current_line = "";
    }
  }
  return 0;
}
