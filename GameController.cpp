#include "GameController.h"

#include <fstream>

#include "CelestialObject.h"
#include "AsteroidDash.h"

// Simply instantiates the game
GameController::GameController(
        const string &space_grid_file_name,
        const string &celestial_objects_file_name,
        const string &leaderboard_file_name,
        const string &player_file_name,
        const string &player_name

) {
    game = new AsteroidDash(space_grid_file_name, celestial_objects_file_name, leaderboard_file_name, player_file_name,
                            player_name);
}

void GameController::print_stats(unsigned long high_score) {
    cout << "Tick: " << game->game_time << endl;
    cout << "Lives: " << game->player->lives << endl;
    cout << "Ammo: " << game->player->current_ammo << endl;
    cout << "Score: " << game->current_score << endl;
    cout << "High Score: " << high_score << endl;
}

// Reads commands from the given input file, executes each command in a game tick
void GameController::play(const string &commands_file) {
    std::ifstream file(commands_file);

    std::string line;

    unsigned long high_score = 0;
    if (game->leaderboard.head_leaderboard_entry != nullptr){
        high_score = game->leaderboard.head_leaderboard_entry->score;
    }

    while (std::getline(file, line)) {
        game->update_space_grid();
        if (!game->game_over) {
            line.erase(line.find_last_not_of(" \n\r\t")+1);

            if (line == "MOVE_DOWN") {
                game->player->move_down(game->space_grid.size());
            } else if (line == "MOVE_UP") {
                game->player->move_up();
            } else if (line == "MOVE_RIGHT") {
                game->player->move_right(game->space_grid.at(0).size());
            } else if (line == "MOVE_LEFT") {
                game->player->move_left();
            } else if (line == "SHOOT") {
                game->shoot();
            } else if (line == "NOP") {
                //continue
            } else if (line == "PRINT_GRID") {
                print_stats(high_score);
                game->print_space_grid();
            }
        } else {
            cout << "GAME OVER!" << endl;
            print_stats(high_score);
            cout << "Player: " << game->player->player_name << endl;
            game->print_space_grid();
            game->leaderboard.insert(new LeaderboardEntry(game->current_score, time(nullptr), game->player->player_name));
            game->leaderboard.write_to_file(game->leaderboard_file_name);
            game->leaderboard.print_leaderboard();
            break;
        }
        game->game_time++;
    }
    if (!game->game_over) {
        cout << "GAME FINISHED! No more commands!" << endl;
        print_stats(high_score);
        cout << "Player: " << game->player->player_name << endl;
        game->print_space_grid();
        game->leaderboard.insert(new LeaderboardEntry(game->current_score, time(nullptr), game->player->player_name));
        game->leaderboard.write_to_file(game->leaderboard_file_name);
        game->leaderboard.print_leaderboard();
    }
}

// Destructor to delete dynamically allocated member variables here
GameController::~GameController() {
    delete game;
}
