/*
 * maze.c
 *
 * Created on: Apr 8, 2025
 * Author: abbd
 * Modified: Apr 15, 2025
 */

#include "maze.h"
#include <stdbool.h>
#include "adc.h"
#include "dac.h"

// Definition des murs du labyrinthe
WallRect walls[NUM_WALLS];




// Constantes pour la geometrie du labyrinthe
#define COTE_CELLULE 36
#define NB_CELLULE_LONG 12
#define NB_CELLULE_HAUT 5
#define LARGEUR_MUR 5
#define X_DEBUT 2
#define Y_DEBUT 2

#define LONGUEUR_TOT (COTE_CELLULE * NB_CELLULE_LONG)
#define HAUTEUR_TOT (COTE_CELLULE * NB_CELLULE_HAUT)

MazePoint points[MAX_POINTS];
int pointCount = 0;

void InitWalls(void)
{
    int i = 0;

    // Bordures extérieures
    walls[i++] = (WallRect){X_DEBUT, Y_DEBUT, LONGUEUR_TOT, LARGEUR_MUR};
    walls[i++] = (WallRect){X_DEBUT, Y_DEBUT + HAUTEUR_TOT, LONGUEUR_TOT + LARGEUR_MUR, LARGEUR_MUR};
    walls[i++] = (WallRect){X_DEBUT, Y_DEBUT, LARGEUR_MUR, HAUTEUR_TOT};
    walls[i++] = (WallRect){X_DEBUT + LONGUEUR_TOT, Y_DEBUT, LARGEUR_MUR, HAUTEUR_TOT};

    // GAUCHE ÉCRAN
    walls[i++] = (WallRect){X_DEBUT + 1 * COTE_CELLULE, Y_DEBUT + 1 * COTE_CELLULE, LARGEUR_MUR, COTE_CELLULE};
    walls[i++] = (WallRect){X_DEBUT + 1 * COTE_CELLULE, Y_DEBUT + 1 * COTE_CELLULE, COTE_CELLULE, LARGEUR_MUR};

    walls[i++] = (WallRect){X_DEBUT + 1 * COTE_CELLULE, Y_DEBUT + 3 * COTE_CELLULE, LARGEUR_MUR, COTE_CELLULE};
    walls[i++] = (WallRect){X_DEBUT + 1 * COTE_CELLULE, Y_DEBUT + 4 * COTE_CELLULE, COTE_CELLULE, LARGEUR_MUR};

    walls[i++] = (WallRect){X_DEBUT + 3 * COTE_CELLULE, Y_DEBUT + 5, LARGEUR_MUR, COTE_CELLULE};
    walls[i++] = (WallRect){X_DEBUT + 3 * COTE_CELLULE, Y_DEBUT + 4 * COTE_CELLULE, LARGEUR_MUR, COTE_CELLULE};

    walls[i++] = (WallRect){X_DEBUT + 2 * COTE_CELLULE, Y_DEBUT + 2 * COTE_CELLULE, COTE_CELLULE, LARGEUR_MUR};
    walls[i++] = (WallRect){X_DEBUT + 2 * COTE_CELLULE, Y_DEBUT + 3 * COTE_CELLULE, COTE_CELLULE, LARGEUR_MUR};

    // MILIEU
    walls[i++] = (WallRect){X_DEBUT + 4 * COTE_CELLULE, Y_DEBUT + 1 * COTE_CELLULE, 4 * COTE_CELLULE, LARGEUR_MUR};
    walls[i++] = (WallRect){X_DEBUT + 4 * COTE_CELLULE, Y_DEBUT + 4 * COTE_CELLULE, 4 * COTE_CELLULE, LARGEUR_MUR};
    walls[i++] = (WallRect){X_DEBUT + 4 * COTE_CELLULE, Y_DEBUT + 2 * COTE_CELLULE, COTE_CELLULE, LARGEUR_MUR};
    walls[i++] = (WallRect){X_DEBUT + 7 * COTE_CELLULE, Y_DEBUT + 2 * COTE_CELLULE, COTE_CELLULE, LARGEUR_MUR};
    walls[i++] = (WallRect){X_DEBUT + 4 * COTE_CELLULE, Y_DEBUT + 2 * COTE_CELLULE, LARGEUR_MUR, COTE_CELLULE + LARGEUR_MUR};
    walls[i++] = (WallRect){X_DEBUT + 8 * COTE_CELLULE, Y_DEBUT + 2 * COTE_CELLULE, LARGEUR_MUR, COTE_CELLULE + LARGEUR_MUR};
    walls[i++] = (WallRect){X_DEBUT + 4 * COTE_CELLULE, Y_DEBUT + 3 * COTE_CELLULE, 4 * COTE_CELLULE, LARGEUR_MUR};

    // DROITE ÉCRAN
    walls[i++] = (WallRect){X_DEBUT + 11 * COTE_CELLULE, Y_DEBUT + 1 * COTE_CELLULE, LARGEUR_MUR, COTE_CELLULE};
    walls[i++] = (WallRect){X_DEBUT + 10 * COTE_CELLULE, Y_DEBUT + 1 * COTE_CELLULE, COTE_CELLULE, LARGEUR_MUR};

    walls[i++] = (WallRect){X_DEBUT + 11 * COTE_CELLULE, Y_DEBUT + 3 * COTE_CELLULE, LARGEUR_MUR, COTE_CELLULE};
    walls[i++] = (WallRect){X_DEBUT + 10 * COTE_CELLULE, Y_DEBUT + 4 * COTE_CELLULE, COTE_CELLULE +5, LARGEUR_MUR};

    walls[i++] = (WallRect){X_DEBUT + 9 * COTE_CELLULE, Y_DEBUT + 5, LARGEUR_MUR, COTE_CELLULE};
    walls[i++] = (WallRect){X_DEBUT + 9 * COTE_CELLULE, Y_DEBUT + 4 * COTE_CELLULE, LARGEUR_MUR, COTE_CELLULE};

    walls[i++] = (WallRect){X_DEBUT + 9 * COTE_CELLULE , Y_DEBUT + 2 * COTE_CELLULE, COTE_CELLULE, LARGEUR_MUR};
    walls[i++] = (WallRect){X_DEBUT + 9 * COTE_CELLULE , Y_DEBUT + 3 * COTE_CELLULE, COTE_CELLULE, LARGEUR_MUR};


}


// Dessiner le labyrinthe
void DrawMaze(uint32_t color)
{
    BSP_LCD_SetTextColor(color);
    for (int i = 0; i < NUM_WALLS; i++) {
        BSP_LCD_FillRect(walls[i].x, walls[i].y, walls[i].width, walls[i].height);
    }
}

// Initialiser les points (pastilles)

void InitPoints(void)
{
    pointCount = 0;
    int y_debut_point = Y_DEBUT + COTE_CELLULE / 2;
    int x_debut_point = X_DEBUT + COTE_CELLULE / 2;
    int margin = 6; // Marge de sécurité pour éviter les collisions avec les murs

    // Créer une grille de points, en évitant les murs
    for (int y = y_debut_point; y < HAUTEUR_TOT; y += COTE_CELLULE / 2) {
        for (int x = x_debut_point; x < LONGUEUR_TOT; x += COTE_CELLULE / 2) {
            // Vérifier si la position n'est pas dans un mur avec une marge de sécurité
            if (!IsPositionInWall(x, y, margin, margin)) {
                points[pointCount].x = x;
                points[pointCount].y = y;
                points[pointCount].eaten = false;
                pointCount++;

                // Éviter de dépasser le maximum de points
                if (pointCount >= MAX_POINTS) {
                    return;
                }
            }
        }
    }
}

// Dessiner les points (pastilles)
void DrawPoints(uint32_t color)
{
    BSP_LCD_SetTextColor(color);

    for (int i = 0; i < pointCount; i++) {
        if (!points[i].eaten) {
            BSP_LCD_FillCircle(points[i].x, points[i].y, 2);
        }
    }
}

// Verifier si une position est dans un mur
bool IsPositionInWall(int x, int y, int width, int height)
{
    for (int i = 0; i < NUM_WALLS; i++) {
        WallRect w = walls[i];
        if (x < w.x + w.width &&
            x + width > w.x &&
            y < w.y + w.height &&
            y + height > w.y) {
            return true; // collision détectée
        }
    }
    return false;
}

// Initialiser Pac-Man
void InitPacMan(PacMan *pacman)
{
    pacman->x = Y_DEBUT + COTE_CELLULE / 2 + 2;; // Position X initiale (bord droit comme sur l'image)
    pacman->y = X_DEBUT + COTE_CELLULE / 2 + 2; // Position Y initiale
    pacman->radius = 11; // Rayon
    pacman->direction = DIR_LEFT; // Direction initiale (gauche)
    pacman->speed = 2; // Vitesse de déplacement
    pacman->mouthAngle = 45; // Angle d'ouverture de la bouche (en degrés)
    pacman->mouthDirection = 1; // Direction de l'animation de la bouche (1: ouverture, -1: fermeture)
    pacman->score = 0; // Score initial
    pacman->lives = 3; // Nombre de vies
}

// Dessiner Pac-Man avec animation de la bouche
void DrawPacMan(PacMan *pacman, uint32_t color)
{
    BSP_LCD_SetTextColor(color);

    // Animation de la bouche
    pacman->mouthAngle += pacman->mouthDirection * 5;
    if (pacman->mouthAngle >= 45) {
        pacman->mouthDirection = -1;
    } else if (pacman->mouthAngle <= 5) {
        pacman->mouthDirection = 1;
    }

    // Dessiner un cercle plein pour le corps
    BSP_LCD_FillCircle(pacman->x, pacman->y, pacman->radius);

    // Dessiner un triangle pour la bouche (en noir)
    BSP_LCD_SetTextColor(LCD_COLOR_BLACK);

    // Calculer les coordonnées du triangle représentant la bouche
    // selon la direction de Pac-Man
    int centerX = pacman->x;
    int centerY = pacman->y;
    int startAngle, endAngle;

    switch (pacman->direction) {
        case DIR_RIGHT:
            startAngle = 270 + pacman->mouthAngle/2;
            endAngle = 270 - pacman->mouthAngle/2;
            break;
        case DIR_LEFT:
            startAngle = 90 + pacman->mouthAngle/2;
            endAngle = 90 - pacman->mouthAngle/2;
            break;
        case DIR_UP:
            startAngle = 180 + pacman->mouthAngle/2;
            endAngle = 180 - pacman->mouthAngle/2;
            break;
        case DIR_DOWN:
            startAngle = 0 + pacman->mouthAngle/2;
            endAngle = 0 - pacman->mouthAngle/2;
            break;
    }

    // Convertir les angles en radians
    float startRad = startAngle * 3.14159f / 180.0f;
    float endRad = endAngle * 3.14159f / 180.0f;

    // Points du triangle
    int x1 = centerX;
    int y1 = centerY;
    int x2 = centerX + pacman->radius * cos(startRad);
    int y2 = centerY + pacman->radius * sin(startRad);
    int x3 = centerX + pacman->radius * cos(endRad);
    int y3 = centerY + pacman->radius * sin(endRad);

    // Dessiner le triangle de la bouche
    Point points[3]; // Use the Point struct from the LCD library

    // Fill the points array with your coordinates
    points[0].X = x1;
    points[0].Y = y1;
    points[1].X = x2;
    points[1].Y = y2;
    points[2].X = x3;
    points[2].Y = y3;

    // Now pass the properly structured array
    BSP_LCD_FillPolygon(points, 3);
}

// Afficher le score et les vies de Pac-Man
void DrawGameInfo(PacMan *pacman)
{
    char scoreText[20];
    sprintf(scoreText, "Score: %d", pacman->score);

    BSP_LCD_SetTextColor(LCD_COLOR_WHITE);
    BSP_LCD_SetBackColor(LCD_COLOR_BLACK);
    BSP_LCD_SetFont(&Font12);
    BSP_LCD_DisplayStringAt(10, 255, (uint8_t*)scoreText, LEFT_MODE);

    // Afficher les vies restantes
    char livesText[20];
    sprintf(livesText, "Vies: %d", pacman->lives);
    BSP_LCD_DisplayStringAt(380, 255, (uint8_t*)livesText, LEFT_MODE);
}

// Verifier si Pac-Man mange un point
void CheckPointsCollision(PacMan *pacman)
{
    for (int i = 0; i < pointCount; i++) {
        if (!points[i].eaten) {
            // Calculer la distance entre Pac-Man et le point
            int dx = pacman->x - points[i].x;
            int dy = pacman->y - points[i].y;
            int distance = sqrt(dx*dx + dy*dy);

            // Si Pac-Man est assez proche du point, le manger
            if (distance < pacman->radius + 3) {
                points[i].eaten = true;
                pacman->score += 10;
            }
        }
    }
}

// Verifier si tous les points ont ete manges
bool AllPointsEaten(void)
{
    for (int i = 0; i < pointCount; i++) {
        if (!points[i].eaten) {
            return false;
        }
    }
    return true;
}

// Reinitialiser le niveau quand tous les points sont manges
void ResetLevel(PacMan *pacman)
{
    for (int i = 0; i < pointCount; i++) {
        points[i].eaten = false;
    }

    // Ne pas réinitialiser le score, mais augmenter la difficulté
    pacman->speed += 0.5f;
}

// Deplacer Pac-Man en fonction du joystick
void MovePacMan(PacMan *pacman, uint32_t joy_h, uint32_t joy_v)
{
    int new_x = pacman->x;
    int new_y = pacman->y;
    int old_direction = pacman->direction;
    Direction new_direction = old_direction;

    // Déterminer la direction en fonction des valeurs du joystick
    // joy_h : 0 (droite) à 4090 (gauche)
    // joy_v : 4095 (haut) à 10 (bas)

    // Changement de direction horizontal
    if (joy_h > 3000) { // Gauche
        new_direction = DIR_LEFT;
    } else if (joy_h < 1000) { // Droite
        new_direction = DIR_RIGHT;
    }

    // Changement de direction vertical
    if (joy_v > 3000) { // Haut
        new_direction = DIR_UP;
    } else if (joy_v < 1000) { // Bas
        new_direction = DIR_DOWN;
    }

    // Si une nouvelle direction est choisie, essayer de l'appliquer
    if (new_direction != old_direction) {
        pacman->direction = new_direction;

        // Calculer la nouvelle position selon la nouvelle direction
        switch (pacman->direction) {
            case DIR_RIGHT:
                new_x += pacman->speed;
                break;
            case DIR_DOWN:
                new_y += pacman->speed;
                break;
            case DIR_LEFT:
                new_x -= pacman->speed;
                break;
            case DIR_UP:
                new_y -= pacman->speed;
                break;
        }

        // Vérifier s'il y a collision avec un mur dans la nouvelle direction
        if (IsPositionInWall(new_x - pacman->radius, new_y - pacman->radius,
                             2 * pacman->radius, 2 * pacman->radius)) {
            // Collision dans la nouvelle direction, revenir à l'ancienne
            pacman->direction = old_direction;
            new_x = pacman->x;
            new_y = pacman->y;
        }
    }

    // Déplacement dans la direction actuelle
    switch (pacman->direction) {
        case DIR_RIGHT:
            new_x += pacman->speed;
            break;
        case DIR_DOWN:
            new_y += pacman->speed;
            break;
        case DIR_LEFT:
            new_x -= pacman->speed;
            break;
        case DIR_UP:
            new_y -= pacman->speed;
            break;
    }

    // Vérifier s'il y a collision avec un mur
    if (!IsPositionInWall(new_x - pacman->radius, new_y - pacman->radius,
                          2 * pacman->radius, 2 * pacman->radius)) {
        // Pas de collision, déplacer Pac-Man
        pacman->x = new_x;
        pacman->y = new_y;
    }

    // Gestion du passage par les tunnels (si présents)
    // Exemple: sortir à droite, réapparaître à gauche
    if (pacman->x > 480 + pacman->radius) {
        pacman->x = 0 - pacman->radius;
    } else if (pacman->x < 0 - pacman->radius) {
        pacman->x = 480 + pacman->radius;
    }

    // Vérifier si Pac-Man mange un point
    CheckPointsCollision(pacman);

    // Vérifier si tous les points ont été mangés
    if (AllPointsEaten()) {
        ResetLevel(pacman);
    }
}


#define NUM_GHOSTS 4
Ghost ghosts[NUM_GHOSTS];
uint32_t ghostColors[NUM_GHOSTS] = {LCD_COLOR_RED, LCD_COLOR_CYAN, LCD_COLOR_MAGENTA, LCD_COLOR_ORANGE};
int ghostHomeX[NUM_GHOSTS] = {25, 450, 25, 450};
int ghostHomeY[NUM_GHOSTS] = {25, 25, 240, 240};

// Initialiser les fantomes
void InitGhosts(void)
{
    for (int i = 0; i < NUM_GHOSTS; i++) {
        ghosts[i].x = 240 + i*30 - 45; // Position initiale au centre, écartés horizontalement
        ghosts[i].y = 135;
        ghosts[i].radius = 10;
        ghosts[i].direction = i % 2 == 0 ? DIR_LEFT : DIR_RIGHT; // Directions alternées
        ghosts[i].speed = 1.5f;
        ghosts[i].color = ghostColors[i];
        ghosts[i].targetX = ghostHomeX[i];
        ghosts[i].targetY = ghostHomeY[i];
        ghosts[i].isScattered = true;
        ghosts[i].scatterModeTimer = 300; // ~5 secondes à 60 FPS
    }
}

// Dessiner un fantome
void DrawGhost(Ghost *ghost)
{
    BSP_LCD_SetTextColor(ghost->color);

    // Corps principal (moitié supérieure: cercle)
    BSP_LCD_FillCircle(ghost->x, ghost->y - ghost->radius/2, ghost->radius);

    // Moitié inférieure (rectangle avec ondulations)
    BSP_LCD_FillRect(ghost->x - ghost->radius, ghost->y - ghost->radius/2,
                    2 * ghost->radius, ghost->radius + 2);

    // Ondulations en bas (3 triangles)
    int baseY = ghost->y + ghost->radius/2 + 2;
    int triangleWidth = ghost->radius * 2 / 3;

    for (int i = 0; i < 3; i++) {
        int startX = ghost->x - ghost->radius + i * triangleWidth;

        // Create an array of Point structs
        Point points[3];

        // Fill the points with coordinates
        points[0].X = startX;
        points[0].Y = baseY;

        points[1].X = startX + triangleWidth/2;
        points[1].Y = baseY + 4;

        points[2].X = startX + triangleWidth;
        points[2].Y = baseY;

        BSP_LCD_FillPolygon(points, 3);

    }

    // Yeux (blancs)
    int eyeRadius = ghost->radius / 3;
    int eyeOffsetX = ghost->radius / 2;
    int eyeOffsetY = -ghost->radius / 3;

    BSP_LCD_SetTextColor(LCD_COLOR_WHITE);
    BSP_LCD_FillCircle(ghost->x - eyeOffsetX, ghost->y + eyeOffsetY, eyeRadius);
    BSP_LCD_FillCircle(ghost->x + eyeOffsetX, ghost->y + eyeOffsetY, eyeRadius);

    // Pupilles (noires) - elles regardent dans la direction du mouvement
    int pupilRadius = eyeRadius / 2;
    int pupilOffsetX = pupilRadius;

    switch (ghost->direction) {
        case DIR_RIGHT:
            pupilOffsetX = pupilRadius;
            break;
        case DIR_LEFT:
            pupilOffsetX = -pupilRadius;
            break;
        case DIR_UP:
            pupilOffsetX = 0;
            break;
        case DIR_DOWN:
            pupilOffsetX = 0;
            break;
    }

    BSP_LCD_SetTextColor(LCD_COLOR_BLACK);
    BSP_LCD_FillCircle(ghost->x - eyeOffsetX + pupilOffsetX, ghost->y + eyeOffsetY, pupilRadius);
    BSP_LCD_FillCircle(ghost->x + eyeOffsetX + pupilOffsetX, ghost->y + eyeOffsetY, pupilRadius);
}

// Dessiner tous les fantomes
void DrawGhosts(void)
{
    for (int i = 0; i < NUM_GHOSTS; i++) {
        DrawGhost(&ghosts[i]);
    }
}

void DrawGhostsBlackTemporarily(void)
{
    uint32_t originalColors[NUM_GHOSTS];

    for (int i = 0; i < NUM_GHOSTS; i++) {
        originalColors[i] = ghosts[i].color;
        ghosts[i].color = LCD_COLOR_BLACK;
        DrawGhost(&ghosts[i]);
        ghosts[i].color = originalColors[i]; // restaurer couleur originale
    }
}

// Trouver la meilleure direction pour un fantome
Direction GetBestGhostDirection(Ghost *ghost, PacMan *pacman)
{
    int targetX, targetY;

    // Si en mode dispersé, cibler le coin attribué
    if (ghost->isScattered) {
        targetX = ghost->targetX;
        targetY = ghost->targetY;
    } else {
        // Sinon, cibler Pac-Man
        targetX = pacman->x;
        targetY = pacman->y;
    }

    // Directions possibles (éviter le demi-tour)
    Direction possibleDirections[3];
    int dirCount = 0;

    // Exclure la direction opposée à la direction actuelle
    Direction oppositeDir;
    switch (ghost->direction) {
        case DIR_RIGHT: oppositeDir = DIR_LEFT; break;
        case DIR_LEFT: oppositeDir = DIR_RIGHT; break;
        case DIR_UP: oppositeDir = DIR_DOWN; break;
        case DIR_DOWN: oppositeDir = DIR_UP; break;
        default: oppositeDir = DIR_NONE; break;
    }

    // Tester les 4 directions
    for (Direction dir = DIR_RIGHT; dir <= DIR_UP; dir++) {
        if (dir == oppositeDir) continue;

        // Calculer la nouvelle position dans cette direction
        int new_x = ghost->x;
        int new_y = ghost->y;

        switch (dir) {
            case DIR_RIGHT: new_x += ghost->speed; break;
            case DIR_DOWN: new_y += ghost->speed; break;
            case DIR_LEFT: new_x -= ghost->speed; break;
            case DIR_UP: new_y -= ghost->speed; break;
        }

        // Vérifier s'il y a collision avec un mur
        if (!IsPositionInWall(new_x - ghost->radius, new_y - ghost->radius,
                              2 * ghost->radius, 2 * ghost->radius)) {
            // Direction valide, l'ajouter aux possibilités
            possibleDirections[dirCount++] = dir;
        }
    }

    // S'il n'y a aucune direction possible (improbable mais sécurité)
    if (dirCount == 0) {
        return ghost->direction;
    }

    // Trouver la meilleure direction parmi les possibles
    // (celle qui rapproche le plus de la cible)
    Direction bestDir = possibleDirections[0];
    float bestDistance = 999999.0f;

    for (int i = 0; i < dirCount; i++) {
        Direction dir = possibleDirections[i];
        int new_x = ghost->x;
        int new_y = ghost->y;

        switch (dir) {
            case DIR_RIGHT: new_x += 20; break;
            case DIR_DOWN: new_y += 20; break;
            case DIR_LEFT: new_x -= 20; break;
            case DIR_UP: new_y -= 20; break;
        }

        // Calculer la distance à la cible depuis cette nouvelle position
        int dx = targetX - new_x;
        int dy = targetY - new_y;
        float distance = sqrt(dx*dx + dy*dy);

        if (distance < bestDistance) {
            bestDistance = distance;
            bestDir = dir;
        }
    }

    return bestDir;
}

// Deplacer un fantome
void MoveGhost(Ghost *ghost, PacMan *pacman)
{
    // Mise à jour du mode dispersé
    if (ghost->isScattered) {
        ghost->scatterModeTimer--;
        if (ghost->scatterModeTimer <= 0) {
            ghost->isScattered = false;
            ghost->scatterModeTimer = 600; // ~10 secondes de poursuite
        }
    } else {
        ghost->scatterModeTimer--;
        if (ghost->scatterModeTimer <= 0) {
            ghost->isScattered = true;
            ghost->scatterModeTimer = 300; // ~5 secondes de dispersion
        }
    }

    // À une intersection (tous les ~20 pixels)
    if ((ghost->x % 20 == 0 && ghost->y % 20 == 0) || ghost->direction == DIR_NONE) {
        ghost->direction = GetBestGhostDirection(ghost, pacman);
    }

    // Calculer la nouvelle position sans déplacer directement le fantome
    int new_x = ghost->x;
    int new_y = ghost->y;

    switch (ghost->direction) {
        case DIR_RIGHT:
            new_x += ghost->speed;
            break;
        case DIR_DOWN:
            new_y += ghost->speed;
            break;
        case DIR_LEFT:
            new_x -= ghost->speed;
            break;
        case DIR_UP:
            new_y -= ghost->speed;
            break;
    }

    // Vérifier si le fantome est dans la zone de départ centrale (où ils sont initialisés)
    bool isInCentralArea = (ghost->x >= 180 && ghost->x <= 300 &&
                            ghost->y >= 110 && ghost->y <= 160);

    // Si le fantome est dans la zone centrale, lui permettre de sortir
    if (isInCentralArea) {
        ghost->x = new_x;
        ghost->y = new_y;
    }
    // Sinon, vérifier la collision avec les murs normalement
    else if (!IsPositionInWall(new_x - ghost->radius, new_y - ghost->radius,
                          2 * ghost->radius, 2 * ghost->radius)) {
        // Pas de collision, on peut déplacer le fantome
        ghost->x = new_x;
        ghost->y = new_y;
    } else {
        // En cas de collision, chercher une nouvelle direction immédiatement
        ghost->direction = GetBestGhostDirection(ghost, pacman);
    }

    // Gestion du passage par les tunnels (si présents)
    if (ghost->x > 480 + ghost->radius) {
        ghost->x = 0 - ghost->radius;
    } else if (ghost->x < 0 - ghost->radius) {
        ghost->x = 480 + ghost->radius;
    }

    // Vérifier collision avec Pac-Man
    int dx = ghost->x - pacman->x;
    int dy = ghost->y - pacman->y;
    float distance = sqrt(dx*dx + dy*dy);

    if (distance < ghost->radius + pacman->radius) {
        // Collision avec Pac-Man
        pacman->lives--;

        if (pacman->lives > 0) {
            // Réinitialiser les positions
            InitPacMan(pacman);
            InitGhosts();
        }
    }
}

// Deplacer tous les fantomes
void MoveGhosts(PacMan *pacman)
{
    for (int i = 0; i < NUM_GHOSTS; i++) {
        MoveGhost(&ghosts[i], pacman);
    }
}

// Fonction pour l'ecran de demarrage
void ShowStartScreen(void)
{
    BSP_LCD_Clear(LCD_COLOR_BLACK);

    BSP_LCD_SetTextColor(LCD_COLOR_YELLOW);
    BSP_LCD_SetBackColor(LCD_COLOR_BLACK);
    BSP_LCD_SetFont(&Font24);
    BSP_LCD_DisplayStringAt(120, 100, (uint8_t*)"PAC-MAN", CENTER_MODE);

    BSP_LCD_SetFont(&Font16);
    BSP_LCD_DisplayStringAt(120, 150, (uint8_t*)"Appuyez sur le joystick", CENTER_MODE);
    BSP_LCD_DisplayStringAt(120, 170, (uint8_t*)"pour commencer", CENTER_MODE);
}

// Fonction pour l'ecran de fin de partie
void ShowGameOverScreen(int score)
{
    BSP_LCD_Clear(LCD_COLOR_BLACK);

    BSP_LCD_SetTextColor(LCD_COLOR_RED);
    BSP_LCD_SetBackColor(LCD_COLOR_BLACK);
    BSP_LCD_SetFont(&Font24);
    BSP_LCD_DisplayStringAt(120, 100, (uint8_t*)"GAME OVER", CENTER_MODE);

    BSP_LCD_SetTextColor(LCD_COLOR_WHITE);
    BSP_LCD_SetFont(&Font16);

    char scoreText[20];
    sprintf(scoreText, "Score final: %d", score);
    BSP_LCD_DisplayStringAt(120, 150, (uint8_t*)scoreText, CENTER_MODE);

    BSP_LCD_DisplayStringAt(120, 190, (uint8_t*)"Appuyez sur le joystick", CENTER_MODE);
    BSP_LCD_DisplayStringAt(120, 210, (uint8_t*)"pour recommencer", CENTER_MODE);
}

void InitGame(void)
{
    // Initialiser les structures de jeu
    InitWalls();
    InitPoints();
    InitPacMan(&pacman);
    InitGhosts();

    // Réinitialiser les temps
    extern uint32_t last_update_time;
    extern uint32_t animation_time;
    extern uint32_t ghost_update_time;
    last_update_time = HAL_GetTick();
    animation_time = HAL_GetTick();
    ghost_update_time = HAL_GetTick();
}

/**
 * @brief  Reads sensor values (joystick)
 * @param  joy_v: Pointer to vertical joystick variable
 * @param  joy_h: Pointer to horizontal joystick variable
 * @retval None
 */
void ReadSensors( uint32_t *joy_v, uint32_t *joy_h)
{
		ADC_ChannelConfTypeDef sConfig = {0};
		sConfig.Rank = ADC_REGULAR_RANK_1;
		sConfig.SamplingTime = ADC_SAMPLETIME_3CYCLES;


		// Joystick vertical
		sConfig.Channel = ADC_CHANNEL_8;
		HAL_ADC_ConfigChannel(&hadc3, &sConfig);
		HAL_ADC_Start(&hadc3);
		while (HAL_ADC_PollForConversion(&hadc3, 100) != HAL_OK);
		*joy_v = HAL_ADC_GetValue(&hadc3);

		// Joystick horizontal
		HAL_ADC_Start(&hadc1);
		while (HAL_ADC_PollForConversion(&hadc1, 100) != HAL_OK);
		*joy_h = HAL_ADC_GetValue(&hadc1);
}
