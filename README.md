# Asteroid Dash

## Project Objective

Asteroid Dash is a grid-based arcade-style game where players navigate a spacecraft through an asteroid belt while avoiding collisions and collecting power-ups. This project involves building the game's engine using **C++11**, emphasizing dynamic memory allocation, object-oriented design, and linked list structures.

Students are tasked with implementing key gameplay mechanics, including collision detection, projectile interactions, and a leaderboard system, while adhering to efficient memory management practices.

---

## Key Features

1. **Game Grid and Player Management**  
   - Dynamically allocated 2D matrix representing the game space.  
   - Player's spacecraft can move within grid bounds and shoot projectiles.  

2. **Celestial Object Management**  
   - Asteroids and power-ups are represented as celestial objects in a linked list.  
   - Dynamic rotation and shape changes for asteroids based on player interactions.  

3. **Collision Detection and Scoring**  
   - Real-time collision handling for asteroids, projectiles, and power-ups.  
   - Scoring system rewards players for survival, destroying asteroids, and collecting power-ups.  

4. **Leaderboard System**  
   - Tracks top 10 high scores across game sessions.  
   - Dynamically manages entries in a linked list.  

5. **Interactive Commands**  
   - Commands such as `MOVE_UP`, `SHOOT`, and `PRINT_GRID` allow real-time gameplay control.  

---

## Learning Outcomes

This project develops the following skills:

- **Linked Lists and Dynamic Memory Management**  
  - Implementing multilevel linked lists for celestial objects and their rotations.  

- **File Input/Output**  
  - Parsing grid and object data from files to initialize game components.  

- **Game Mechanics Design**  
  - Handling projectile-object interactions, rotations, and collision effects.  

- **Efficient Memory Usage**  
  - Ensuring no memory leaks and freeing dynamically allocated resources.  

- **Class Design and Object-Oriented Programming**  
  - Creating modular and reusable components for game logic.

---

## Usage

### Compilation
To compile the project, use the provided Makefile or manually compile with the following command:

```bash
# Manual compilation
g++ -std=c++11 -g main.cpp AsteroidDash.cpp CelestialObject.cpp GameController.cpp \
    LeaderboardEntry.cpp Leaderboard.cpp Player.cpp -o AsteroidDash
