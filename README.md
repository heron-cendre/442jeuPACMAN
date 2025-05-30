# 442jeuPACMAN
mini-projet


Voici le schéma synoptique du projet :

![Schéma synoptique](images/schema_principe.jpg)
## Fichiers principaux

- `maze.c` / `maze.h` : sous-fonctions d'affichage, gestion des entités
- `freertos.c` : tâches FreeRTOS, machine à états principale
- `main.c` : initialisation matérielle, démarrage du scheduler

## Machine à états

Le jeu fonctionne selon 4 états principaux :
- **STATE_MENU** : écran d’accueil, attente du joueur
- **STATE_GAME_RUNNING** : jeu en cours
- **STATE_LEVEL_COMPLETE** : niveau terminé
- **STATE_GAME_OVER** : fin de partie
![Etats](images/machine_a_etat.jpg)
Les transitions se font via les boutons ou selon les événements du jeu (tous les points mangés, plus de vies…).

## Deux taches

La tache gérant la logique du jeu :

![tache gérant le bon déroulement du jeu](images/fonction_jeu.jpg)

Elle appelle de nombreuses fonctions déjà codées
La tache gérant l'affichage sur l'écran LCD :

![tache gérant l'affichage du jeu](images/fonction_affichage.jpg)

## Pistes d'amélioration 
