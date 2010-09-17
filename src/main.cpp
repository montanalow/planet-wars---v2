#include <algorithm>
#include <iostream>
#include <fstream>


#include "game_state.h"
#include "util.h"


// This is just the main game loop that takes care of communicating with the
// game engine for you. You don't have to understand or change the code below.
int main(int argc, char *argv[]) {
  std::string current_line;
  std::string game_state_data;
  while (true) {
    int c = std::cin.get();
    current_line += (char) c;
    if (c == '\n') {
//      std::cerr << current_line;
      if (current_line.length() >= 2 && current_line.substr(0, 2) == "go") {
        pw::game_state game_state(game_state_data);
        game_state_data = "";
        game_state.take_turn();
      } else {
        game_state_data += current_line;
      }
      current_line = "";
    }
  }
  return 0;
}
