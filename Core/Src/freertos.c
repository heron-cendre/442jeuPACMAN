/* DEBUT DU CODE UTILISATEUR EN-TETE */
/**
  ******************************************************************************
  * Nom du fichier          : freertos.c
  * Description             : Code pour les applications freertos
  ******************************************************************************
  * Attention
  *
  * Copyright (c) 2025 STMicroelectronics.
  * Tous droits reserves.
  *
  * Ce logiciel est sous licence selon les termes du fichier LICENSE dans le dossier racine de ce composant logiciel.
  * Si aucun fichier LICENSE n'est fourni avec ce logiciel, il est fourni TEL QUEL.
  *
  ******************************************************************************
  */
/* FIN DU CODE UTILISATEUR EN-TETE */

/* Inclusions ------------------------------------------------------------------*/
#include "FreeRTOS.h"
#include "task.h"
#include "main.h"
#include "cmsis_os.h"

/* Inclusions privees ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "maze.h"
#include "stm32746g_discovery_lcd.h"
#include <string.h>
#include <stdbool.h>
#include "adc.h"
#include "stm32f7xx_hal.h"
#include "stm32f7xx_hal_def.h"
#include "HorombeRGB565.h"
/* USER CODE END Includes */

/* Definition privee -----------------------------------------------------------*/
#define GAME_OVER_DELAY_MS 2000
#define LEVEL_COMPLETE_DELAY_MS 2000
#define BUTTON_DEBOUNCE_MS 200

/* Macro privee ---------------------------------------------------------------*/

/* Variables privees ----------------------------------------------------------*/
// Declarations externes
extern ADC_HandleTypeDef hadc1;
extern ADC_HandleTypeDef hadc3;
extern struct {
    int x;
    int y;
    bool eaten;
} points[];

// Mutex pour proteger l'etat du jeu partage
//osMutexId gameMutexHandle;

// Semaphore pour la synchronisation de l'affichage
osSemaphoreId displaySemHandle;

osThreadId tache_jeuHandle;
osThreadId tache_affichageHandle;
osThreadId tache_entreesHandle;
osThreadId tache_fantomeHandle;
osMessageQId queue_PacManHandle;
osMessageQId queue_pointsHandle;
osMessageQId joystickQueueHandle;
osMessageQId vieQueueHandle;

/* Prototypes de fonctions privees --------------------------------------------*/

void fonction_jeu(void const * argument);
void fonction_affichage(void const * argument);
void fonction_entrees(void const * argument);
void fonction_fantome(void const * argument);

void MX_FREERTOS_Init(void); /* (regle MISRA C 2004 8.1) */

/* Prototype GetIdleTaskMemory (lie a la prise en charge de l'allocation statique) */
void vApplicationGetIdleTaskMemory( StaticTask_t **ppxIdleTaskTCBBuffer, StackType_t **ppxIdleTaskStackBuffer, uint32_t *pulIdleTaskStackSize );

static StaticTask_t xIdleTaskTCBBuffer;
static StackType_t xIdleStack[configMINIMAL_STACK_SIZE];

void vApplicationGetIdleTaskMemory( StaticTask_t **ppxIdleTaskTCBBuffer, StackType_t **ppxIdleTaskStackBuffer, uint32_t *pulIdleTaskStackSize )
{
  *ppxIdleTaskTCBBuffer = &xIdleTaskTCBBuffer;
  *ppxIdleTaskStackBuffer = &xIdleStack[0];
  *pulIdleTaskStackSize = configMINIMAL_STACK_SIZE;
  /* Espace pour le code utilisateur */
}

/**
  * @brief  Initialisation de FreeRTOS
  * @param  Aucun
  * @retval Aucun
  */
void MX_FREERTOS_Init(void) {
  /* Initialisation utilisateur */

  /* RTOS_MUTEX utilisateur */
  // Creer le mutex pour la protection de l'etat du jeu
  //osMutexDef(gameMutex);
 // gameMutexHandle = osMutexCreate(osMutex(gameMutex));
  
  // Creer le semaphore pour la synchronisation de l'affichage
  osSemaphoreDef(displaySem);
  displaySemHandle = osSemaphoreCreate(osSemaphore(displaySem), 1);
  /* FIN RTOS_MUTEX utilisateur */

  /* RTOS_SEMAPHORES utilisateur */
  /* Ajouter des semaphores, ... */
  /* FIN RTOS_SEMAPHORES utilisateur */

  /* RTOS_TIMERS utilisateur */
  /* Demarrer les timers, ajouter de nouveaux, ... */
  /* FIN RTOS_TIMERS utilisateur */

  /* Creation des files d'attente */
  /* definition et creation de queue_PacMan */
  osMessageQDef(queue_PacMan, 16, uint16_t);
  queue_PacManHandle = osMessageCreate(osMessageQ(queue_PacMan), NULL);

  /* definition et creation de queue_points */
  osMessageQDef(queue_points, 16, uint16_t);
  queue_pointsHandle = osMessageCreate(osMessageQ(queue_points), NULL);

  /* definition et creation de joystickQueue */
  osMessageQDef(joystickQueue, 32, uint32_t);
  joystickQueueHandle = osMessageCreate(osMessageQ(joystickQueue), NULL);

  /* definition et creation de vieQueue */
  osMessageQDef(vieQueue, 16, uint16_t);
  vieQueueHandle = osMessageCreate(osMessageQ(vieQueue), NULL);

  /* RTOS_QUEUES utilisateur */
  /* Ajouter des files d'attente, ... */
  /* FIN RTOS_QUEUES utilisateur */

  /* Creation des threads */
  /* definition et creation de tache_jeu */
  osThreadDef(tache_jeu, fonction_jeu, osPriorityAboveNormal, 0, 128);
  tache_jeuHandle = osThreadCreate(osThread(tache_jeu), NULL);

  /* definition et creation de tache_affichage */
  osThreadDef(tache_affichage, fonction_affichage, osPriorityNormal, 0, 1024);
  tache_affichageHandle = osThreadCreate(osThread(tache_affichage), NULL);

  /* definition et creation de tache_entrees */
  osThreadDef(tache_entrees, fonction_entrees, osPriorityHigh, 0, 128);
  tache_entreesHandle = osThreadCreate(osThread(tache_entrees), NULL);

  /* definition et creation de tache_fantome */
  osThreadDef(tache_fantome, fonction_fantome, osPriorityNormal, 0, 128);
  tache_fantomeHandle = osThreadCreate(osThread(tache_fantome), NULL);

  /* RTOS_THREADS utilisateur */
  /* Ajouter des threads, ... */
  /* FIN RTOS_THREADS utilisateur */

}

/**
* @brief Fonction implementant le thread tache_jeu.
* @param argument: Non utilise
* @retval Aucun
*/
void fonction_jeu(void const * argument)
{
  /* Fonction utilisateur tache_jeu */

    static uint32_t joystick_v, joystick_h;
    for(;;) {
        switch(etat) {
            case STATE_MENU:
                // Controle des LED en fonction des boutons
                HAL_GPIO_WritePin(LED13_GPIO_Port, LED13_Pin, HAL_GPIO_ReadPin(BP1_GPIO_Port, BP1_Pin));
                HAL_GPIO_WritePin(LED14_GPIO_Port, LED14_Pin, HAL_GPIO_ReadPin(BP2_GPIO_Port, BP2_Pin));
                // Mesure des potentiometres et du joystick
                ReadSensors(&joystick_v, &joystick_h);
                // Transition vers l'etat 1 (jeu) si BP1 est enfonce
                if (!(HAL_GPIO_ReadPin(BP1_GPIO_Port, BP1_Pin))) {
                    HAL_Delay(200);
                    if (!(HAL_GPIO_ReadPin(BP1_GPIO_Port, BP1_Pin))) {
                        InitGame();
                        etat = STATE_GAME_RUNNING;
                    }
                }
                break;
            case STATE_GAME_RUNNING:


                uint32_t current_time = HAL_GetTick();
                // Mise a jour de Pac-Man toutes les 50ms
                if(current_time - last_update_time > 50) {
                    last_update_time = current_time;
                    ReadSensors(&joystick_v, &joystick_h);
                    MovePacMan(&pacman, joystick_h, joystick_v);
                    if (AllPointsEaten()) {
                        etat = STATE_LEVEL_COMPLETE;
                        state_change_delay = HAL_GetTick();
                        break;
                    }
                    if (pacman.lives <= 0) {
                        etat = STATE_GAME_OVER;
                        state_change_delay = HAL_GetTick();
                        break;
                    }
                }
                // Mise a jour des fantomes toutes les 75ms
                if(current_time - ghost_update_time > 75) {
                    ghost_update_time = current_time;
                    MoveGhosts(&pacman);
                }
                // Retour a l'etat 0 si BP1 est enfonce a nouveau
                if (!(HAL_GPIO_ReadPin(BP1_GPIO_Port, BP1_Pin))) {
                    HAL_Delay(200);
                    if (!(HAL_GPIO_ReadPin(BP1_GPIO_Port, BP1_Pin))) {
                        etat = STATE_MENU;
                    }
                }
                break;
            case STATE_GAME_OVER:
                if (HAL_GetTick() - state_change_delay > 2000) {
                    if (!(HAL_GPIO_ReadPin(BP1_GPIO_Port, BP1_Pin))) {
                        HAL_Delay(200);
                        if (!(HAL_GPIO_ReadPin(BP1_GPIO_Port, BP1_Pin))) {
                            etat = STATE_MENU;
                        }
                    }
                }
                break;
            case STATE_LEVEL_COMPLETE:
                if (HAL_GetTick() - state_change_delay > 2000) {
                    if (!(HAL_GPIO_ReadPin(BP1_GPIO_Port, BP1_Pin))) {
                        HAL_Delay(200);
                        if (!(HAL_GPIO_ReadPin(BP1_GPIO_Port, BP1_Pin))) {
                            ResetLevel(&pacman);
                            etat = STATE_GAME_RUNNING;
                        }
                    }
                }
                break;
        }
        vTaskDelay(pdMS_TO_TICKS(20));
    }
  /* Fin fonction utilisateur tache_jeu */
}

/**
* @brief Fonction implementant le thread tache_affichage.
* @param argument: Non utilise
* @retval Aucun
*/
void fonction_affichage(void const * argument)
{
  /* Fonction utilisateur tache_affichage */
  static TS_StateTypeDef TS_State;
  static GameState lastEtat = -1;
  static bool points_erased[MAX_POINTS] = {0};
  for(;;) {
    //if(osMutexWait(gameMutexHandle, osWaitForever) == osOK) {
      GameState localEtat = etat;
      PacMan localPacman = pacman;
      //osMutexRelease(gameMutexHandle);

      // Detecter le changement d'etat pour reinitialiser les couches et le suivi
      if (localEtat != lastEtat) {
        if (localEtat == STATE_GAME_RUNNING) {
          BSP_LCD_SelectLayer(0);
          BSP_LCD_Clear(LCD_COLOR_BLACK);
          DrawMaze(LCD_COLOR_BLUE);
          DrawPoints(LCD_COLOR_YELLOW);
          BSP_LCD_SelectLayer(1);
          BSP_LCD_Clear(0x00000000); // Transparent
          memset(points_erased, 0, sizeof(points_erased));
        } else if (localEtat == STATE_MENU) {
          BSP_LCD_SelectLayer(0);
          BSP_LCD_Clear(LCD_COLOR_BLACK);
          BSP_LCD_SelectLayer(1);
          BSP_LCD_Clear(0x00000000);
        } else {
          BSP_LCD_SelectLayer(0);
          BSP_LCD_Clear(LCD_COLOR_BLACK);
          BSP_LCD_SelectLayer(1);
          BSP_LCD_Clear(0x00000000);
        }
        lastEtat = localEtat;
      }

      switch(localEtat) {
        case STATE_MENU:
          BSP_LCD_SelectLayer(1);
          BSP_LCD_SetTextColor(LCD_COLOR_WHITE);
          BSP_TS_GetState(&TS_State);
          if(TS_State.touchDetected) {
            int x = TS_State.touchX[0];
            int y = TS_State.touchY[0];
            int rayon = 2;
            if (((x - rayon >= 0) && (x + rayon < LCD_LONGUEUR) && (y - rayon >= 0) && (y + rayon < LCD_LARGEUR))) {
              BSP_LCD_FillCircle(x, y, rayon);
            }
          }
          if (HAL_GetTick() - animation_time > 500) {
            animation_time = HAL_GetTick();
            BSP_LCD_SelectLayer(0);
            BSP_LCD_Clear(LCD_COLOR_BLACK);
            BSP_LCD_SetTextColor(LCD_COLOR_YELLOW);
            BSP_LCD_SetBackColor(LCD_COLOR_BLACK);
            BSP_LCD_SetFont(&Font24);
            BSP_LCD_DisplayStringAt(140, 100, (uint8_t*)"PAC-MAN", LEFT_MODE);
            BSP_LCD_SetFont(&Font16);
            BSP_LCD_DisplayStringAt(140, 150, (uint8_t*)"Appuyez sur BP1", LEFT_MODE);
            BSP_LCD_DisplayStringAt(140, 170, (uint8_t*)"pour commencer", LEFT_MODE);
            static bool mouth_open = true;
            if (mouth_open) {
              BSP_LCD_FillCircle(240, 220, 15);
              mouth_open = false;
            } else {
              BSP_LCD_FillCircle(240, 220, 15);
              BSP_LCD_SetTextColor(LCD_COLOR_BLACK);
              Point tri[3] = {
                {240, 220},
                {240+15, 220-10},
                {240+15, 220+10}
              };
              BSP_LCD_FillPolygon(tri, 3);
              BSP_LCD_SetTextColor(LCD_COLOR_YELLOW);
              mouth_open = true;
            }
            BSP_LCD_SelectLayer(1);
            BSP_LCD_Clear(0x00000000);
          }
          break;
        case STATE_GAME_RUNNING:
          // 1. Effacer la couche transparente pour les objets en mouvement
          BSP_LCD_SelectLayer(1);
          BSP_LCD_Clear(0x00000000); // Transparent

          // 2. Dessiner Pac-Man et les fantomes a leur position actuelle

          DrawGameInfo(&pacman);
          DrawPacMan(&pacman, LCD_COLOR_YELLOW);
          DrawGhosts();

          // 3. Pour les points manges, dessiner un cercle noir sur la couche 0 (fond) si pas deja efface
          BSP_LCD_SelectLayer(0);
          for (int i = 0; i < MAX_POINTS; i++) {
            if (points[i].eaten && !points_erased[i]) {
              BSP_LCD_SetTextColor(LCD_COLOR_BLACK);
              BSP_LCD_FillCircle(points[i].x, points[i].y, 2);
              points_erased[i] = true;
            }
          }
          break;
        case STATE_GAME_OVER:
          BSP_LCD_SelectLayer(0);
          BSP_LCD_Clear(LCD_COLOR_BLACK);
          ShowGameOverScreen(localPacman.score);
          break;
        case STATE_LEVEL_COMPLETE:
          BSP_LCD_SelectLayer(0);
          BSP_LCD_Clear(LCD_COLOR_BLACK);
          BSP_LCD_SetTextColor(LCD_COLOR_GREEN);
          BSP_LCD_SetBackColor(LCD_COLOR_BLACK);
          BSP_LCD_SetFont(&Font24);
          BSP_LCD_DisplayStringAt(240, 100, (uint8_t*)"NIVEAU TERMINE!", CENTER_MODE);
          BSP_LCD_SetTextColor(LCD_COLOR_WHITE);
          BSP_LCD_SetFont(&Font16);
          char scoreText[30];
          sprintf(scoreText, "Score: %d", localPacman.score);
          BSP_LCD_DisplayStringAt(240, 150, (uint8_t*)scoreText, CENTER_MODE);
          BSP_LCD_DisplayStringAt(240, 190, (uint8_t*)"Appuyez sur BP1", CENTER_MODE);
          BSP_LCD_DisplayStringAt(240, 210, (uint8_t*)"pour continuer", CENTER_MODE);
          break;
      }
    }
    vTaskDelay(pdMS_TO_TICKS(20));
  }
  /* Fin fonction utilisateur tache_affichage */
}

/**
* @brief Fonction implementant le thread tache_entrees.
* @param argument: Non utilise
* @retval Aucun
*/
void fonction_entrees(void const * argument)
{
  /* Fonction utilisateur tache_entrees */
  // TODO: Implementer la logique d'entree ici
  for(;;) {
    vTaskDelay(pdMS_TO_TICKS(100));
  }
  /* Fin fonction utilisateur tache_entrees */
}

/**
* @brief Fonction implementant le thread tache_fantome.
* @param argument: Non utilise
* @retval Aucun
*/
void fonction_fantome(void const * argument)
{
  /* Fonction utilisateur tache_fantome */
  /* Boucle infinie */
  for(;;)
  {
    osDelay(1);
  }
  /* Fin fonction utilisateur tache_fantome */
}

/* Code d'application prive --------------------------------------------------*/
// Ajouter les prototypes manquants si non inclus depuis l'en-tete

// Ajouter les extern pour les variables partagees si pas deja presents
extern uint32_t last_update_time;
extern uint32_t ghost_update_time;
extern uint32_t state_change_delay;
extern uint32_t animation_time;



