#include <iostream>
#include "planet_wars.h"

// The DoTurn function is where your code goes. The PlanetWars object contains
// the state of the game, including information about all planets and fleets
// that currently exist. Inside this function, you issue orders using the
// pw.IssueOrder() function. For example, to send 10 ships from planet 3 to
// planet 8, you would say pw.IssueOrder(3, 8, 10).
//
// There is already a basic strategy in place here. You can use it as a
// starting point, or you can throw it out entirely and replace it with your
// own. Check out the tutorials and articles on the contest website at
// http://www.ai-contest.com/resources.
void make_move(const planet_wars& pw) {
  // (1) If we currently have a fleet in flight, just do nothing.
  if (pw.my_fleets().size() >= 1) {
    return;
  }
  // (2) Find my strongest planet.
  int source = -1;
  double source_score = -999999.0;
  int source_num_ships = 0;
  std::vector<planet> my_planets = pw.my_planets();
  for (int i = 0; i < my_planets.size(); ++i) {
    const planet& p = my_planets[i];
    double score = (double)p.num_ships();
    if (score > source_score) {
      source_score = score;
      source = p.planet_id();
      source_num_ships = p.num_ships();
    }
  }
  // (3) Find the weakest enemy or neutral planet.
  int dest = -1;
  double dest_score = -999999.0;
  std::vector<planet> not_my_planets = pw.not_my_planets();
  for (int i = 0; i < not_my_planets.size(); ++i) {
    const planet& p = not_my_planets[i];
    double score = 1.0 / (1 + p.num_ships());
    if (score > dest_score) {
      dest_score = score;
      dest = p.planet_id();
    }
  }
  // (4) Send half the ships from my strongest planet to the weakest
  // planet that I do not own.
  if (source >= 0 && dest >= 0) {
    int num_ships = source_num_ships / 2;
    pw.issue_order(source, dest, num_ships);
  }
}

// This is just the main game loop that takes care of communicating with the
// game engine for you. You don't have to understand or change the code below.
int main(int argc, char *argv[]) {
  std::string current_line;
  std::string map_data;
  while (true) {
    int c = std::cin.get();
    current_line += (char)c;
    if (c == '\n') {
      if (current_line.length() >= 2 && current_line.substr(0, 2) == "go") {
        planet_wars pw(map_data);
        map_data = "";
        make_move(pw);
	pw.finish_turn();
      } else {
        map_data += current_line;
      }
      current_line = "";
    }
  }
  return 0;
}
