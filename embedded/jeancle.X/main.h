/* 
 * File:   main.h
 * Author: E306_PC1
 *
 * Created on 14 septembre 2026, 12:31
 */

#ifndef MAIN_H
#define	MAIN_H

#define STATE_ATTENTE 0
#define STATE_ATTENTE_EN_COURS 1
#define STATE_AVANCE 2
#define STATE_AVANCE_EN_COURS 3
#define STATE_TOURNE_GAUCHE 4
#define STATE_TOURNE_GAUCHE_EN_COURS 5
#define STATE_TOURNE_DROITE 6
#define STATE_TOURNE_DROITE_EN_COURS 7
#define STATE_TOURNE_SUR_PLACE_GAUCHE 8
#define STATE_TOURNE_SUR_PLACE_GAUCHE_EN_COURS 9
#define STATE_TOURNE_SUR_PLACE_DROITE 10
#define STATE_TOURNE_SUR_PLACE_DROITE_EN_COURS 11
#define STATE_ARRET 12
#define STATE_ARRET_EN_COURS 13
#define STATE_RECULE 14
#define STATE_RECULE_EN_COURS 15
#define PAS_D_OBSTACLE 0
#define OBSTACLE_A_GAUCHE 1
#define OBSTACLE_A_DROITE 2
#define OBSTACLE_EN_FACE 3
#define OBSTACLE_A_EXTREME_DROITE 4
#define OBSTACLE_A_EXTREME_GAUCHE 5
// il y en a plein que j utilise plus

#define SEUIL_ARRET       30.0f  // cm : distance qui déclenche l'évitement
#define SEUIL_DEGAGE      40.0f  // cm : distance pour considérer la voie libre (hystérésis)
#define VITESSE_CROISIERE 25.0f  // % PWM en ligne droite
#define VITESSE_PIVOT     10.0f  // % PWM en rotation pendant l'évitement (contrainte capteurs)
#define DUREE_PIVOT_MINI  300UL  // ms, temps mini de pivot avant de retester les capteurs

// void SetNextRobotStateInAutomaticMode(void);
// void consigneEvitement(void);
 void OperatingSystemLoop(void);
 unsigned char PositionObstacle(void);
 unsigned char VoieLibre(void);

#endif	/* MAIN_H */

