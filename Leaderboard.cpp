#include "Leaderboard.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <iomanip>
#include <ctime>

// Read the stored leaderboard status from the given file such that the "head_leaderboard_entry" member
// variable will point to the highest all-times score, and all other scores will be reachable from it
// via the "next_leaderboard_entry" member variable pointer.
void Leaderboard::read_from_file(const std::string &filename) {
    std::ifstream file(filename);

    std::string line;
    while (std::getline(file, line)) {
        std::istringstream iss(line);
        int score;
        long timestamp;
        std::string name;

        iss >> score >> timestamp >> name;

        LeaderboardEntry* entry = new LeaderboardEntry(score, timestamp, name);

        insert(entry);
    }

    file.close();
}



// Write the latest leaderboard status to the given file in the format specified in the PA instructions
void Leaderboard::write_to_file(const string &filename) {
    std::ofstream file(filename);

    LeaderboardEntry* curr = head_leaderboard_entry;
    while (curr != nullptr) {
        file << curr->score << " " << curr->last_played << " " << curr->player_name << std::endl;
        curr = curr->next;
    }

    file.close();
}

// Print the current leaderboard status to the standard output in the format specified in the PA instructions
void Leaderboard::print_leaderboard() {
    std::cout << "Leaderboard\n-----------" << std::endl;
    int count = 0;
    LeaderboardEntry* curr = head_leaderboard_entry;
    while (curr != nullptr) {

        std::time_t time = curr->last_played;
        std::tm* timePtr = std::localtime(&time);

        std::cout << ++count << ". " << curr->score << " "
                  << std::put_time(timePtr, "%H:%M:%S/%d.%m.%Y") << " "
                  << curr->player_name << std::endl;

        curr = curr->next;
    }
}


//  Insert a new LeaderboardEntry instance into the leaderboard, such that the order of the high-scores
//  is maintained, and the leaderboard size does not exceed 10 entries at any given time (only the
//  top 10 all-time high-scores should be kept in descending order by the score).
void Leaderboard::insert(LeaderboardEntry* new_entry) {
    if (!head_leaderboard_entry || new_entry->score > head_leaderboard_entry->score) {
        new_entry->next = head_leaderboard_entry;
        head_leaderboard_entry = new_entry;
    } else {
        LeaderboardEntry* curr = head_leaderboard_entry;
        while (curr->next && curr->next->score >= new_entry->score) {
            curr = curr->next;
        }
        new_entry->next = curr->next;
        curr->next = new_entry;
    }

    int count = 0;
    LeaderboardEntry* curr = head_leaderboard_entry;
    LeaderboardEntry* prev = nullptr;
    while (curr && count < 10) {
        prev = curr;
        curr = curr->next;
        count++;
    }
    if (curr) {
        prev->next = nullptr;
        while (curr) {
            LeaderboardEntry* to_delete = curr;
            curr = curr->next;
            delete to_delete;
        }
    }
}

// Free dynamically allocated memory used for storing leaderboard entries
Leaderboard::~Leaderboard() {
    LeaderboardEntry* current = head_leaderboard_entry;
    while (current != nullptr) {
        LeaderboardEntry* to_delete = current;
        current = current->next;
        delete to_delete;
    }
}
