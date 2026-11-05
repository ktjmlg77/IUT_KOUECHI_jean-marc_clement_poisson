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
#define STATE_ATTENDRE 1000
// il y en a plein que j utilise plus

<<<<<<< Updated upstream
#define SEUIL_ARRET       25.0f  // cm : distance qui dï¿½clenche l'ï¿½vitement
#define SEUIL_DEGAGE      20.0f  // cm : distance pour considï¿½rer la voie libre (hystï¿½rï¿½sis)
#define VITESSE_CROISIERE 25.0f  // % PWM en ligne droite
#define VITESSE_PIVOT     10.0f  // % PWM en rotation pendant l'ï¿½vitement (contrainte capteurs)
/****************************************************************************************************/
// Mot binaire d'etat des 5 capteurs infrarouges (bit = 1 : obstacle detecte)
// Bit4 = Extreme Gauche | Bit3 = Gauche | Bit2 = Centre | Bit1 = Droit | Bit0 = Extreme Droit
/****************************************************************************************************/
#define BIT_CAPTEUR_EXTREME_GAUCHE (1 << 4)
#define BIT_CAPTEUR_GAUCHE         (1 << 3)
#define BIT_CAPTEUR_CENTRE         (1 << 2)
#define BIT_CAPTEUR_DROIT          (1 << 1)
#define BIT_CAPTEUR_EXTREME_DROIT  (1 << 0)
=======
#define SEUIL_ARRET       26.0f  // cm : distance qui déclenche l'évitement
#define SEUIL_DEGAGE      25.0f  // cm : distance pour considérer la voie libre (hystérésis)
#define VITESSE_CROISIERE 30.0f  // % PWM en ligne droite
#define VITESSE_PIVOT     10.0f  // % PWM en rotation pendant l'évitement (contrainte capteurs)
#define DUREE_PIVOT_MINI  1UL  // ms, temps mini de pivot avant de retester les capteurs
>>>>>>> Stashed changes

/****************************************************************************************************/
// Vitesses utilisees par la table de decision (main.c), graduees selon la gravite
/****************************************************************************************************/
#define VITESSE_VIRAGE_LEGER 18.0f // % PWM roue interieure : leger virage, on avance toujours (1 capteur peripherique)
#define VITESSE_PIVOT_FORT   15.0f // % PWM pivot sur place : 2 capteurs actifs
#define VITESSE_PIVOT_MAX    20.0f // % PWM pivot sur place : 3 capteurs actifs
#define VITESSE_RECUL        15.0f // % PWM marche arriere : 4 ou 5 capteurs actifs (robot encercle)

void OperatingSystemLoop(void);
unsigned char BuildObstacleWord(void);

#endif	/* MAIN_H */
