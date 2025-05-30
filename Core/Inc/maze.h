/*
 * maze.h
 *
 *  Created on: Apr 8, 2025
 *      Author: abbd
 */


/*
 * maze.h
 *
 * Created on: Apr 8, 2025
 * Author: abbd
 * Modified: Apr 15, 2025
 */

#ifndef __MAZE_H
#define __MAZE_H

#include "stm32f7xx_hal.h"
#include "stm32746g_discovery_lcd.h"
#include "stm32746g_discovery_ts.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <math.h>

// Game states definition
typedef enum {
    STATE_MENU = 0,        // Initial state, splash screen
    STATE_GAME_RUNNING = 1, // Game is running
    STATE_GAME_OVER = 2,    // Game over state
    STATE_LEVEL_COMPLETE = 3 // Level completed state
} GameState;

// Nombre total de murs dans le labyrinthe
#define NUM_WALLS 27
#define NUM_GHOSTS 4
#define MAX_POINTS 200
#define LCD_LARGEUR 272
#define LCD_LONGUEUR 480

extern uint32_t last_update_time;
extern uint32_t ghost_update_time;
extern uint32_t state_change_delay;
extern uint32_t animation_time;

// Directions possibles
typedef enum {
    DIR_NONE = 0,
    DIR_RIGHT,
    DIR_DOWN,
    DIR_LEFT,
    DIR_UP
} Direction;

// Structure pour représenter un mur
typedef struct {
    int x;
    int y;
    int width;
    int height;
} WallRect;

// Structure pour représenter Pac-Man
typedef struct {
    int x;
    int y;
    int radius;
    Direction direction;
    float speed;
    int mouthAngle;
    int mouthDirection;
    int score;
    int lives;
} PacMan;

// Structure pour un point (pastille)
typedef struct {
    int x;
    int y;
    bool eaten;
} MazePoint;

// Structure pour un fantôme
typedef struct {
    int x;
    int y;
    int radius;
    Direction direction;
    float speed;
    uint32_t color;
    int targetX;
    int targetY;
    bool isScattered;
    int scatterModeTimer;
} Ghost;


void InitGame(void);
void DrawGhosts(void);
// Fonctions pour le labyrinthe
void InitWalls(void);
void DrawMaze(uint32_t color);

// Fonctions pour les points (pastilles)
void InitPoints(void);
void DrawPoints(uint32_t color);
bool AllPointsEaten(void);

// Fonctions pour Pac-Man
void InitPacMan(PacMan *pacman);
void DrawPacMan(PacMan *pacman, uint32_t color);
void MovePacMan(PacMan *pacman, uint32_t joy_h, uint32_t joy_v);
void DrawGameInfo(PacMan *pacman);
void CheckPointsCollision(PacMan *pacman);
void ResetLevel(PacMan *pacman);

// Fonctions pour les fantômes
void InitGhosts(void);
void DrawGhost(Ghost *ghost);
void MoveGhost(Ghost *ghost, PacMan *pacman);
void MoveGhosts(PacMan *pacman);
Direction GetBestGhostDirection(Ghost *ghost, PacMan *pacman);

// Fonctions utilitaires
bool IsPositionInWall(int x, int y, int width, int height);

// Fonctions pour les écrans de jeu
void ShowStartScreen(void);
void ShowGameOverScreen(int score);

void ReadSensors(uint32_t *joy_v, uint32_t *joy_h);

// Variables globales externes
extern WallRect walls[NUM_WALLS];
extern Ghost ghosts[NUM_GHOSTS];
extern uint32_t ghostColors[NUM_GHOSTS];
// Add externs for shared variables if not already present
extern uint32_t last_update_time;
extern uint32_t ghost_update_time;
extern uint32_t state_change_delay;
extern uint32_t animation_time;


// Shared game state variables
extern GameState etat;
extern PacMan pacman;
extern uint32_t joystick_h, joystick_v;



#endif /* __MAZE_H */
