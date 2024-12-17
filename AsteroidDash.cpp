#include "AsteroidDash.h"
#include "CelestialObject.h"
#include "GameController.h"
#include "Player.h"

#include <fstream>
#include <iostream>
#include <sstream>
#include <vector>
#include <string>

// Constructor to initialize AsteroidDash with the given parameters
AsteroidDash::AsteroidDash(const string &space_grid_file_name,
                           const string &celestial_objects_file_name,
                           const string &leaderboard_file_name,
                           const string &player_file_name,
                           const string &player_name)

        : leaderboard_file_name(leaderboard_file_name), leaderboard(Leaderboard()) {

    read_player(player_file_name, player_name);  // Initialize player using the player.dat file
    read_space_grid(space_grid_file_name);  // Initialize the grid after the player is loaded
    read_celestial_objects(celestial_objects_file_name);  // Load celestial objects
    leaderboard.read_from_file(leaderboard_file_name);
}

// Function to read the space grid from a file
void AsteroidDash::read_space_grid(const string &input_file) {

    std::ifstream file(input_file);

    std::vector<std::vector<int>> matris; // 2D vector

    std::string line;
    while (std::getline(file, line)) {
        std::istringstream lineGoing(line);
        std::vector<int> lineData;
        int val;

        while (lineGoing >> val) {
            lineData.push_back(val);
        }

        space_grid.push_back(lineData);
    }

    file.close();
}

// Function to read the player from a file
void AsteroidDash::read_player(const string &player_file_name, const string &player_name) {

    std::ifstream file(player_file_name);

    int row, col;
    file >> row >> col;

    file.ignore();

    std::vector<std::vector<bool>> shape;
    std::string line;

    while (std::getline(file, line)) {
        std::vector<bool> row_values;
        for (char ch : line) {
            if (ch == '1') {
                row_values.push_back(true);
            } else if (ch == '0') {
                row_values.push_back(false);
            }
        }
        if (row_values.size() != 0) {
            shape.push_back(row_values);
        }
    }

    file.close();

    int max_ammo = 10;
    int lives = 3;

    player = new Player(shape, row, col, player_name, max_ammo, lives);
}

// Function to read celestial objects from a file
void AsteroidDash::read_celestial_objects(const string &input_file) {

    std::ifstream file(input_file);

    std::string line;
    std::vector<std::vector<bool>> obj_cells;
    bool is_shaping = false;
    std::string shapeType;

    //attributes
    ObjectType obj_type;
    int starting_row = 0;
    int time = 0;
    std::string effect;

    while (std::getline(file, line)) {
        line.erase(0, line.find_first_not_of(" \t"));

        if (line.empty()) continue;

        if (line.at(0) == '{' || line.at(0) == '[') {
            is_shaping = true;
            obj_cells.clear();

            while (is_shaping) {
                std::vector<bool> obj_cell_row;
                for (char ch : line) {
                    if (ch == '0' || ch == '1') {
                        obj_cell_row.push_back(ch == '1');
                    } else if (ch == ']' || ch == '}') {
                        while (std::getline(file, line)) {
                            if (line[0] == 's') {
                                starting_row = std::stoi(line.substr(2));
                            } else if (line[0] == 't') {
                                time = std::stoi(line.substr(2));
                                obj_type = ASTEROID;
                                if (ch == ']') {
                                    break;
                                }
                            } else if (line[0] == 'e') {
                                std::string effect = line.substr(2);
                                effect.erase(effect.find_last_not_of(" \n\r\t")+1);
                                obj_type = effect == "life" ? LIFE_UP : AMMO;
                                break;
                            }
                        }
                        is_shaping = false;
                    }
                }

                if (!obj_cell_row.empty()) {
                    obj_cells.push_back(obj_cell_row);
                }

                if (is_shaping) {
                    std::getline(file, line);
                    line.erase(0, line.find_first_not_of(" \t"));
                }
            }
        }

        //add new object to the linkedlist
        if (!is_shaping && !obj_cells.empty()) {
            CelestialObject* newCelestial = new CelestialObject(obj_cells, obj_type, starting_row, time);

            if (celestial_objects_list_head == nullptr) {
                celestial_objects_list_head = newCelestial;
            } else {
                CelestialObject* curr = celestial_objects_list_head;

                while (curr->next_celestial_object != nullptr) {
                    curr = curr->next_celestial_object;
                }
                curr->next_celestial_object = newCelestial;

                if (curr->right_rotation != nullptr) {
                    CelestialObject* newCurr = curr->right_rotation;
                    while (newCurr != curr) {
                        newCurr->next_celestial_object = newCelestial;
                        newCurr = newCurr->right_rotation;
                    }
                }
            }
            obj_cells.clear();
        }
    }
    file.close();
}

// Print the entire space grid
void AsteroidDash::print_space_grid() const {
    for (const auto& line : space_grid) {
        for (int i=0; i < line.size(); i++) {
            if (line[i] == 0) {
                std::cout << unoccupiedCellChar;
            } else {
                std::cout << occupiedCellChar;
            }
        }
        std::cout << std::endl;
    }
    std::cout << std::endl;
}


void AsteroidDash::place_objects() {
    reset_grid();

    CelestialObject* curr = celestial_objects_list_head;
    while (curr != nullptr) {
        if (game_time == curr->time_of_appearance-1) {
            if (!curr->appeared) {
                curr->appeared = true;
                curr->col = space_grid.at(0).size();
            }
        }
        if (curr->appeared) {
            for (int i=0; i < curr->shape.size(); i++) {
                for (int j=0; j < curr->shape.at(0).size(); j++) {
                    if (curr->col+j < space_grid.at(0).size()) {
                        space_grid[curr->starting_row+i][curr->col+j] = curr->shape[i][j];
                    }
                }
            }
        }
        curr=curr->next_celestial_object;
    }
}

void AsteroidDash::reset_grid() {
    for (int i=0; i < space_grid.size(); i++) {
        for (int j=0; j < space_grid.at(0).size(); j++) {
            space_grid[i][j] = 0;
        }
    }
}

// Function to update the space grid with player, celestial objects, and any other changes
// It is called in every game tick before moving on to the next tick.
void AsteroidDash::update_space_grid() {

    int width = player->spacecraft_shape.at(0).size();
    int height = player->spacecraft_shape.size();

    for (int i=0; i < projectiles.size(); i++) {
        projectiles[i][1]++;
    }

    CelestialObject* curr = celestial_objects_list_head;
    // move grid left
    while (curr != nullptr) {
        if (curr->appeared) {
            curr->col--;

            // if the object got out of the grid
            if (curr->col == (-1) * curr->shape.at(0).size()) {
                // Silinecek nesne baş ise, başı güncelle
                if (curr == celestial_objects_list_head) {
                    celestial_objects_list_head = curr->next_celestial_object;
                } else {
                    CelestialObject* newCurr = celestial_objects_list_head;
                    while (newCurr->next_celestial_object != curr) {
                        newCurr = newCurr->next_celestial_object;
                    }
                    newCurr->next_celestial_object = curr->next_celestial_object;
                    CelestialObject* subNew = newCurr->right_rotation;
                    while (subNew != newCurr && subNew!=nullptr) {
                        subNew->next_celestial_object = curr->next_celestial_object;
                        subNew = subNew->right_rotation;
                    }
                    subNew->next_celestial_object = curr->next_celestial_object;
                }

                CelestialObject* toDelete = curr;
                curr = curr->next_celestial_object;
                CelestialObject::delete_rotations(toDelete);
                delete toDelete;
                continue;
            }
        }
        curr = curr->next_celestial_object;
    }

    place_objects();

    // place projectiles if exist, also shooting updates are done here
    if (!projectiles.empty()) {
        int prjct=0;
        for (; prjct < projectiles.size();) {
            if ((projectiles.at(prjct))[1]+1 > space_grid.at(0).size()) {
                projectiles.erase(projectiles.begin()+prjct);
            } else {
                bool dont_increase = false;
                int prj_row = (projectiles.at(prjct))[0];
                int prj_col = (projectiles.at(prjct))[1];
                string toCollide = "none";

                if (space_grid[prj_row][prj_col] || space_grid[prj_row][prj_col-1]) {
                    if (space_grid[prj_row][prj_col]) {
                        toCollide = "this";
                    } else {
                        toCollide = "pre";
                    }
                }

                if (!(toCollide == "none")) {
                    if (toCollide == "this") {
                        CelestialObject* curr = celestial_objects_list_head;
                        while (curr != nullptr) {
                            if (curr->object_type == 0 &&
                                prj_row >= curr->starting_row &&
                                prj_row < curr->starting_row + curr->shape.size() &&
                                prj_col >= curr->col &&
                                prj_col <= curr->col + curr->shape.at(0).size()) {

                                    current_score += 10;

                                    curr->shape[prj_row - curr->starting_row][prj_col - curr->col] = false;

                                    if (isAllFalse(curr->shape)) {
                                        for (int i=0; i<curr->shape.size(); i++) {
                                            for (int j=0; j<curr->shape.at(0).size(); j++) {
                                                if (curr->shape[i][j]) {
                                                    current_score += 100;
                                                }
                                            }
                                        }
                                    } else {
                                        if (curr->shape.size()%2 == 0) {
                                            if (prj_row - curr->starting_row < curr->shape.size()/2) {
                                                curr->shape = curr->get_right_rotation(curr->shape);
                                            } else {
                                                curr->shape = curr->get_left_rotation(curr->shape);
                                            }
                                            curr->create_rotations_again();

                                            if (curr->right_rotation != nullptr) {
                                                CelestialObject* newCurr = curr->right_rotation;
                                                while (newCurr != curr) {
                                                    newCurr->next_celestial_object = curr->next_celestial_object;
                                                    newCurr = newCurr->right_rotation;
                                                }
                                            }
                                        } else {
                                            if (prj_row - curr->starting_row != curr->shape.size()/2) {
                                                if (prj_row - curr->starting_row < curr->shape.size()/2) {
                                                    curr->shape = curr->get_right_rotation(curr->shape);
                                                } else {
                                                    curr->shape = curr->get_left_rotation(curr->shape);
                                                }
                                                curr->create_rotations_again();

                                                if (curr->right_rotation != nullptr) {
                                                    CelestialObject* newCurr = curr->right_rotation;
                                                    while (newCurr != curr) {
                                                        newCurr->next_celestial_object = curr->next_celestial_object;
                                                        newCurr = newCurr->right_rotation;
                                                    }
                                                }
                                            }
                                        }
                                    }
                                    projectiles.erase(projectiles.begin()+prjct);
                                    dont_increase = true;
                                    break;
                                }
                            curr = curr->next_celestial_object;
                        }
                    } else {
                        CelestialObject* curr = celestial_objects_list_head;
                        while (curr != nullptr) {
                            if (curr->object_type == 0 &&
                                prj_row >= curr->starting_row &&
                                prj_row < curr->starting_row + curr->shape.size() &&
                                prj_col-1 >= curr->col &&
                                prj_col-1 <= curr->col + curr->shape.at(0).size()) {

                                    current_score += 10;

                                    curr->shape[prj_row - curr->starting_row][prj_col - curr->col-1] = false;

                                    if (isAllFalse(curr->shape)) {
                                        for (int i=0; i<curr->shape.size(); i++) {
                                            for (int j=0; j<curr->shape.at(0).size(); j++) {
                                                if (curr->shape[i][j]) {
                                                    current_score += 100;
                                                }
                                            }
                                        }
                                    } else {
                                        if (curr->shape.size()%2 == 0) {
                                            if (prj_row - curr->starting_row < curr->shape.size()/2) {
                                                curr->shape = curr->get_right_rotation(curr->shape);
                                            } else {
                                                curr->shape = curr->get_left_rotation(curr->shape);
                                            }
                                            curr->create_rotations_again();

                                            if (curr->right_rotation != nullptr) {
                                                CelestialObject* newCurr = curr->right_rotation;
                                                while (newCurr != curr) {
                                                    newCurr->next_celestial_object = curr->next_celestial_object;
                                                    newCurr = newCurr->right_rotation;
                                                }
                                            }
                                        } else {
                                            if (prj_row - curr->starting_row != curr->shape.size()/2) {
                                                if (prj_row - curr->starting_row < curr->shape.size()/2) {
                                                    curr->shape = curr->get_right_rotation(curr->shape);
                                                } else {
                                                    curr->shape = curr->get_left_rotation(curr->shape);
                                                }
                                                curr->create_rotations_again();

                                                if (curr->right_rotation != nullptr) {
                                                    CelestialObject* newCurr = curr->right_rotation;
                                                    while (newCurr != curr) {
                                                        newCurr->next_celestial_object = curr->next_celestial_object;
                                                        newCurr = newCurr->right_rotation;
                                                    }
                                                }
                                            }
                                        }
                                    }
                                    projectiles.erase(projectiles.begin()+prjct);
                                    dont_increase = true;
                                    break;
                                }
                            curr = curr->next_celestial_object;
                        }
                    }
                }
                if (!dont_increase)prjct++;
            }
        }
    }

    place_objects();

    // collision check
    bool collision_detected = false;
    CelestialObject* collided_object = nullptr;
    for (int j = 0; j < height; j++) {
        for (int i = 0; i < width; i++) {
            if (player->spacecraft_shape[j][i] && space_grid[player->position_row + j][player->position_col + i]) {
                collision_detected = true;

                CelestialObject* curr = celestial_objects_list_head;
                while (curr != nullptr) {
                    int row_a = player->position_row + j;
                    int col_a = player->position_col + i;
                    if (row_a >= curr->starting_row &&
                        row_a <= curr->starting_row + curr->shape.size() &&
                        col_a >= curr->col &&
                        col_a <= curr->col + curr->shape.at(0).size()) {
                        collided_object = curr;

                        CelestialObject* prev = nullptr;
                        CelestialObject* current = celestial_objects_list_head;
                        while (current != nullptr) {
                            if (current == collided_object) {
                                if (prev) {
                                    CelestialObject* subNew = prev->right_rotation;
                                    while (subNew != prev && subNew!=nullptr) {
                                        subNew->next_celestial_object = current->next_celestial_object;
                                        subNew = subNew->right_rotation;
                                    }
                                    subNew->next_celestial_object = current->next_celestial_object;
                                } else {
                                    celestial_objects_list_head = current->next_celestial_object;
                                }
                                switch (curr->object_type) {
                                    case 0:
                                        for (int i=0; i<current->shape.size(); i++) {
                                            for (int j=0; j<current->shape.at(0).size(); j++) {
                                                if (curr->shape[i][j]) {
                                                    current_score += 100;
                                                }
                                            }
                                        }
                                        player->lives--;
                                        if (player->lives == 0) {
                                            game_over = true;
                                        }
                                        break;
                                    case 1:
                                        player->lives++;
                                        break;
                                    case 2:
                                        if (player->current_ammo + 1 <= player->max_ammo) {
                                            player->current_ammo++;
                                        }
                                        break;
                                }
                                CelestialObject *next = curr->next_celestial_object;
                                CelestialObject::delete_rotations(current);
                                delete current;
                                curr = next;
                                break;
                            }
                            prev = current;
                            current = current->next_celestial_object;
                        }
                    }
                    else curr = curr->next_celestial_object;
                }
                break;
            }
        }
        if (collision_detected) break;
    }

    place_objects();
    
    for (int i=0; i < projectiles.size(); i++) {
        int prj_row = (projectiles.at(i))[0];
        int prj_col = (projectiles.at(i))[1];
        space_grid[prj_row][prj_col] = true;
    }

    // place player
    if (!game_over) {
        for (int j = 0; j < height; j++) {
            for (int i = 0; i < width; i++) {
                if (player->spacecraft_shape[j][i]) {
                    space_grid[player->position_row + j][player->position_col + i] = true;
                }
            }
        }
    }
}

// Corresponds to the SHOOT command.
// It should shoot if the player has enough ammo.
// It should decrease the player's ammo
void AsteroidDash::shoot() {
    if (player->current_ammo > 0) {
        int mid_row = player->position_row + player->spacecraft_shape.size()/2;
        int rightmost = player->position_col + player->spacecraft_shape.at(0).size()-1;
        vector<int> pos;
        pos.push_back(mid_row);
        pos.push_back(rightmost);
        projectiles.push_back(pos);
        player->current_ammo--;
    }
}

bool AsteroidDash::isAllFalse(const std::vector<std::vector<bool>>& shape) {
    for (const auto& row : shape) {
        for (bool value : row) {
            if (value) {
                return false;
            }
        }
    }
    return true;
}

// Destructor. Remove dynamically allocated member variables here.
AsteroidDash::~AsteroidDash() {
    CelestialObject* curr = celestial_objects_list_head;
    while (curr != nullptr) {
        CelestialObject* to_delete = curr;
        curr = curr->next_celestial_object;
        delete to_delete;
    }
    delete player;
}
