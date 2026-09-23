#include <stdio.h>
#include <stdlib.h>
#include <xc.h>
#include "ADC.h"
#include "ChipConfig.h"
#include "IO.h"
#include "timer.h"
#include "PWM.h"
#include "Robot.h"
#include "main.h"
#include "ToolBox.h"
unsigned int stateRobot;

/*
#define Kex 130
#define Kc 400
#define Ka 282
 */
/****************************************************************************************************/
// Construction du mot binaire d'etat des capteurs
// Chaque bit vaut 1 si la distance mesuree par le capteur correspondant est
// inferieure au seuil d'arret (obstacle proche detecte), 0 sinon.
// Bit4=ExtremeGauche  Bit3=Gauche  Bit2=Centre  Bit1=Droit  Bit0=ExtremeDroit
/****************************************************************************************************/
unsigned char BuildObstacleWord(void) {
    unsigned char word = 0;

    if (robotState.distanceTelemetreExtremGauche < SEUIL_ARRET)
        word |= BIT_CAPTEUR_EXTREME_GAUCHE;
    if (robotState.distanceTelemetreGauche < SEUIL_ARRET)
        word |= BIT_CAPTEUR_GAUCHE;
    if (robotState.distanceTelemetreCentre < SEUIL_ARRET)
        word |= BIT_CAPTEUR_CENTRE;
    if (robotState.distanceTelemetreDroit < SEUIL_ARRET)
        word |= BIT_CAPTEUR_DROIT;
    if (robotState.distanceTelemetreExtremDroit < SEUIL_ARRET)
        word |= BIT_CAPTEUR_EXTREME_DROIT;

    return word; // valeur sur 5 bits, de 0b00000 a 0b11111 (0 a 31)
}

/****************************************************************************************************/
// Boucle de gestion de l'evitement : lit les 5 capteurs, construit le mot binaire
// et pilote directement les moteurs, cas par cas, via un switch exhaustif sur les
// 32 valeurs possibles (0b00000 a 0b11111).
//
// Principe de gravite retenu pour chaque bit :
//   - capteur peripherique (EG ou ED) seul  -> poids 1 (menace faible, objet en bordure)
//   - capteur interieur    (G  ou D)  seul  -> poids 2 (menace forte, objet proche de la trajectoire)
//   - capteur centre (C)                    -> menace frontale directe
//
// Nombre de capteurs actifs (popcount) = niveau de gravite globale :
//   0 capteur  -> on avance a pleine vitesse
//   1 capteur  -> capteur peripherique seul : leger virage, on continue d'avancer
//                 capteur interieur ou centre seul : pivot sur place normal (VITESSE_PIVOT)
//   2 capteurs -> pivot sur place plus franc (VITESSE_PIVOT_FORT)
//                 EXCEPTION : EG+ED seuls (les deux peripheriques, rien au centre ni a
//                 l'interieur) -> le passage est degage, on avance tout droit
//   3 capteurs -> pivot sur place fort (VITESSE_PIVOT_MAX)
//   4 ou 5 capteurs -> le robot est reellement encercle : marche arriere (VITESSE_RECUL)
//
// Direction du pivot : toujours a l'oppose du cote le plus charge (poids gauche =
// 2*G+EG, poids droit = 2*D+ED). A egalite stricte, convention fixe et documentee :
// on pivote a droite (evite toute oscillation/indetermination).
/****************************************************************************************************/
void OperatingSystemLoop(void) {
    unsigned char obstacleWord = BuildObstacleWord();

    switch (obstacleWord) {

        /* ---------------------- 0 capteur actif : rien ---------------------- */
        case 0b00000: // rien
            PWMSetSpeedConsigne(VITESSE_CROISIERE, MOTEUR_DROIT);
            PWMSetSpeedConsigne(VITESSE_CROISIERE, MOTEUR_GAUCHE);
            stateRobot = STATE_AVANCE;
            break;

        /* --------------- 1 capteur actif : peripherique seul ----------------- */
        /* on avance toujours, simple correction de trajectoire                  */
        case 0b00001: // ED seul -> leger virage a gauche
            PWMSetSpeedConsigne(VITESSE_CROISIERE, MOTEUR_DROIT);
            PWMSetSpeedConsigne(VITESSE_VIRAGE_LEGER, MOTEUR_GAUCHE);
            stateRobot = STATE_AVANCE;
            break;
        case 0b10000: // EG seul -> leger virage a droite
            PWMSetSpeedConsigne(VITESSE_VIRAGE_LEGER, MOTEUR_DROIT);
            PWMSetSpeedConsigne(VITESSE_CROISIERE, MOTEUR_GAUCHE);
            stateRobot = STATE_AVANCE;
            break;

        /* ------- 1 capteur actif : interieur seul ou centre seul ------------ */
        /* menace plus serieuse : pivot sur place normal                       */
        case 0b00010: // D seul -> pivot sur place a gauche
            PWMSetSpeedConsigne(VITESSE_PIVOT, MOTEUR_DROIT);
            PWMSetSpeedConsigne(-VITESSE_PIVOT, MOTEUR_GAUCHE);
            stateRobot = STATE_TOURNE_SUR_PLACE_GAUCHE;
            break;
        case 0b01000: // G seul -> pivot sur place a droite
            PWMSetSpeedConsigne(-VITESSE_PIVOT, MOTEUR_DROIT);
            PWMSetSpeedConsigne(VITESSE_PIVOT, MOTEUR_GAUCHE);
            stateRobot = STATE_TOURNE_SUR_PLACE_DROITE;
            break;
        case 0b00100: // C seul -> pivot sur place a droite (convention, aucun cote favorise)
            PWMSetSpeedConsigne(-VITESSE_PIVOT, MOTEUR_DROIT);
            PWMSetSpeedConsigne(VITESSE_PIVOT, MOTEUR_GAUCHE);
            stateRobot = STATE_TOURNE_SUR_PLACE_DROITE;
            break;

        /* ------------------------ 2 capteurs actifs -------------------------- */
        case 0b10001: // EG+ED seuls -> EXCEPTION : rien au centre ni a l'interieur, voie libre, on avance
            PWMSetSpeedConsigne(VITESSE_CROISIERE, MOTEUR_DROIT);
            PWMSetSpeedConsigne(VITESSE_CROISIERE, MOTEUR_GAUCHE);
            stateRobot = STATE_AVANCE;
            break;
        case 0b00011: // D+ED (tout le cote droit) -> pivot fort a gauche
            PWMSetSpeedConsigne(VITESSE_PIVOT_FORT, MOTEUR_DROIT);
            PWMSetSpeedConsigne(-VITESSE_PIVOT_FORT, MOTEUR_GAUCHE);
            stateRobot = STATE_TOURNE_SUR_PLACE_GAUCHE;
            break;
        case 0b00101: // C+ED -> pivot fort a gauche
            PWMSetSpeedConsigne(VITESSE_PIVOT_FORT, MOTEUR_DROIT);
            PWMSetSpeedConsigne(-VITESSE_PIVOT_FORT, MOTEUR_GAUCHE);
            stateRobot = STATE_TOURNE_SUR_PLACE_GAUCHE;
            break;
        case 0b00110: // C+D -> pivot fort a gauche
            PWMSetSpeedConsigne(VITESSE_PIVOT_FORT, MOTEUR_DROIT);
            PWMSetSpeedConsigne(-VITESSE_PIVOT_FORT, MOTEUR_GAUCHE);
            stateRobot = STATE_TOURNE_SUR_PLACE_GAUCHE;
            break;
        case 0b10010: // EG+D -> pivot fort a gauche (D plus critique que EG)
            PWMSetSpeedConsigne(VITESSE_PIVOT_FORT, MOTEUR_DROIT);
            PWMSetSpeedConsigne(-VITESSE_PIVOT_FORT, MOTEUR_GAUCHE);
            stateRobot = STATE_TOURNE_SUR_PLACE_GAUCHE;
            break;
        case 0b01001: // G+ED -> pivot fort a droite (G plus critique que ED)
            PWMSetSpeedConsigne(-VITESSE_PIVOT_FORT, MOTEUR_DROIT);
            PWMSetSpeedConsigne(VITESSE_PIVOT_FORT, MOTEUR_GAUCHE);
            stateRobot = STATE_TOURNE_SUR_PLACE_DROITE;
            break;
        case 0b01010: // G+D (les deux interieurs) -> pivot fort a droite (convention, egalite)
            PWMSetSpeedConsigne(-VITESSE_PIVOT_FORT, MOTEUR_DROIT);
            PWMSetSpeedConsigne(VITESSE_PIVOT_FORT, MOTEUR_GAUCHE);
            stateRobot = STATE_TOURNE_SUR_PLACE_DROITE;
            break;
        case 0b01100: // G+C -> pivot fort a droite
            PWMSetSpeedConsigne(-VITESSE_PIVOT_FORT, MOTEUR_DROIT);
            PWMSetSpeedConsigne(VITESSE_PIVOT_FORT, MOTEUR_GAUCHE);
            stateRobot = STATE_TOURNE_SUR_PLACE_DROITE;
            break;
        case 0b10100: // EG+C -> pivot fort a droite
            PWMSetSpeedConsigne(-VITESSE_PIVOT_FORT, MOTEUR_DROIT);
            PWMSetSpeedConsigne(VITESSE_PIVOT_FORT, MOTEUR_GAUCHE);
            stateRobot = STATE_TOURNE_SUR_PLACE_DROITE;
            break;
        case 0b11000: // EG+G (tout le cote gauche) -> pivot fort a droite
            PWMSetSpeedConsigne(-VITESSE_PIVOT_FORT, MOTEUR_DROIT);
            PWMSetSpeedConsigne(VITESSE_PIVOT_FORT, MOTEUR_GAUCHE);
            stateRobot = STATE_TOURNE_SUR_PLACE_DROITE;
            break;

        /* ------------------------ 3 capteurs actifs -------------------------- */
        case 0b00111: // C+D+ED -> pivot max a gauche
            PWMSetSpeedConsigne(VITESSE_PIVOT_MAX, MOTEUR_DROIT);
            PWMSetSpeedConsigne(-VITESSE_PIVOT_MAX, MOTEUR_GAUCHE);
            stateRobot = STATE_TOURNE_SUR_PLACE_GAUCHE;
            break;
        case 0b10011: // EG+D+ED -> pivot max a gauche
            PWMSetSpeedConsigne(VITESSE_PIVOT_MAX, MOTEUR_DROIT);
            PWMSetSpeedConsigne(-VITESSE_PIVOT_MAX, MOTEUR_GAUCHE);
            stateRobot = STATE_TOURNE_SUR_PLACE_GAUCHE;
            break;
        case 0b01011: // G+D+ED -> pivot max a gauche
            PWMSetSpeedConsigne(VITESSE_PIVOT_MAX, MOTEUR_DROIT);
            PWMSetSpeedConsigne(-VITESSE_PIVOT_MAX, MOTEUR_GAUCHE);
            stateRobot = STATE_TOURNE_SUR_PLACE_GAUCHE;
            break;
        case 0b10110: // EG+C+D -> pivot max a gauche
            PWMSetSpeedConsigne(VITESSE_PIVOT_MAX, MOTEUR_DROIT);
            PWMSetSpeedConsigne(-VITESSE_PIVOT_MAX, MOTEUR_GAUCHE);
            stateRobot = STATE_TOURNE_SUR_PLACE_GAUCHE;
            break;
        case 0b01101: // G+C+ED -> pivot max a droite
            PWMSetSpeedConsigne(-VITESSE_PIVOT_MAX, MOTEUR_DROIT);
            PWMSetSpeedConsigne(VITESSE_PIVOT_MAX, MOTEUR_GAUCHE);
            stateRobot = STATE_TOURNE_SUR_PLACE_DROITE;
            break;
        case 0b01110: // G+C+D -> pivot max a droite (convention, egalite)
            PWMSetSpeedConsigne(-VITESSE_PIVOT_MAX, MOTEUR_DROIT);
            PWMSetSpeedConsigne(VITESSE_PIVOT_MAX, MOTEUR_GAUCHE);
            stateRobot = STATE_TOURNE_SUR_PLACE_DROITE;
            break;
        case 0b10101: // EG+C+ED -> pivot max a droite (convention, egalite)
            PWMSetSpeedConsigne(-VITESSE_PIVOT_MAX, MOTEUR_DROIT);
            PWMSetSpeedConsigne(VITESSE_PIVOT_MAX, MOTEUR_GAUCHE);
            stateRobot = STATE_TOURNE_SUR_PLACE_DROITE;
            break;
        case 0b11001: // EG+G+ED -> pivot max a droite
            PWMSetSpeedConsigne(-VITESSE_PIVOT_MAX, MOTEUR_DROIT);
            PWMSetSpeedConsigne(VITESSE_PIVOT_MAX, MOTEUR_GAUCHE);
            stateRobot = STATE_TOURNE_SUR_PLACE_DROITE;
            break;
        case 0b11010: // EG+G+D -> pivot max a droite
            PWMSetSpeedConsigne(-VITESSE_PIVOT_MAX, MOTEUR_DROIT);
            PWMSetSpeedConsigne(VITESSE_PIVOT_MAX, MOTEUR_GAUCHE);
            stateRobot = STATE_TOURNE_SUR_PLACE_DROITE;
            break;
        case 0b11100: // EG+G+C -> pivot max a droite
            PWMSetSpeedConsigne(-VITESSE_PIVOT_MAX, MOTEUR_DROIT);
            PWMSetSpeedConsigne(VITESSE_PIVOT_MAX, MOTEUR_GAUCHE);
            stateRobot = STATE_TOURNE_SUR_PLACE_DROITE;
            break;

        /* --------------- 4 ou 5 capteurs actifs : encercle -------------------- */
        /* plus aucun cote n'est sur : seule la marche arriere est sure          */
        case 0b01111: // G+C+D+ED
        case 0b10111: // EG+C+D+ED
        case 0b11011: // EG+G+D+ED
        case 0b11101: // EG+G+C+ED
        case 0b11110: // EG+G+C+D
        case 0b11111: // tous les capteurs
            PWMSetSpeedConsigne(-VITESSE_RECUL, MOTEUR_DROIT);
            PWMSetSpeedConsigne(-VITESSE_RECUL, MOTEUR_GAUCHE);
            stateRobot = STATE_RECULE;
            break;

        default:
            // Cas normalement impossible (mot toujours sur 5 bits) : securite defensive, arret moteur
            PWMSetSpeedConsigne(0, MOTEUR_DROIT);
            PWMSetSpeedConsigne(0, MOTEUR_GAUCHE);
            stateRobot = STATE_ARRET;
            break;
    }
}

int main(void) {
    //Initialisation oscillateur
    InitOscillator();
    // Configuration des input et output (IO)
    InitIO();
    InitPWM();
    InitTimer1();
    InitTimer4();
    InitTimer23();
    InitADC1();
    LED_BLANCHE_1 = 0;
    LED_BLEUE_1 = 0;
     LED_BLEUE_2 = 0;
    LED_BLANCHE_2 = 0;
    LED_ORANGE_1 = 0;
    LED_ROUGE_1 = 0;
    LED_VERTE_1 = 0;
    PWM_EN = 1;


//PWMSetSpeedConsigne(MOTEUR_DROIT, 20);
//PWMSetSpeedConsigne(MOTEUR_GAUCHE, 20);
// Boucle Principale

while(1) {
    if (ADCIsConversionFinished()) {
        ADCClearConversionFinishedFlag();
        unsigned int * result = ADCGetResult();
        float volts = ((float) result [0])* 3.3 / 4096;
        robotState.distanceTelemetreExtremGauche = 34 / volts - 5;
        LED_BLANCHE_1 = (robotState.distanceTelemetreExtremGauche > 30) ? 0 : 1;

        volts = ((float) result [1])* 3.3 / 4096;
        robotState.distanceTelemetreGauche = 34 / volts - 5;
        LED_BLEUE_1 = (robotState.distanceTelemetreGauche > 30) ? 0 : 1;
        
        volts = ((float) result [2])* 3.3 / 4096;
        robotState.distanceTelemetreCentre = 34 / volts - 5;
        LED_ORANGE_1 = (robotState.distanceTelemetreCentre > 30) ? 0 : 1;

        volts = ((float) result [3])* 3.3 / 4096;
        robotState.distanceTelemetreDroit = 34 / volts - 5;
        LED_ROUGE_1 = (robotState.distanceTelemetreDroit > 30) ? 0 : 1;
        
        volts = ((float) result [4])* 3.3 / 4096;
        robotState.distanceTelemetreExtremDroit = 34 / volts - 5;
        LED_VERTE_1 = (robotState.distanceTelemetreExtremDroit > 30) ? 0 : 1;
    }
    if (robotState.vitesseDroiteCommandeCourante <= -25 && robotState.vitesseGaucheCommandeCourante >= 25) {
        LED_BLANCHE_2 = 1;
    } else {
        LED_BLANCHE_2 = 0;
    }
    if(stateRobot!=PAS_D_OBSTACLE && stateRobot!=STATE_AVANCE && stateRobot!=STATE_AVANCE_EN_COURS)
    {
        LED_BLEUE_2 = 1;
    }else{
        LED_BLEUE_2 = 0;
    }

}

}


// fin main




/*
    
uint8_t stateobs = 0;

void OperatingSystemLoop(void) {
    switch (stateRobot) {
        case STATE_ATTENTE:
            timestamp = 0;
            PWMSetSpeedConsigne(0, MOTEUR_DROIT);
            PWMSetSpeedConsigne(0, MOTEUR_GAUCHE);
            stateRobot = STATE_ATTENTE_EN_COURS;
        case STATE_ATTENTE_EN_COURS:
            if (timestamp > 1000)
                stateRobot = STATE_AVANCE;
            break;
        case STATE_AVANCE:
            //PWMSetSpeedConsigne(25, MOTEUR_DROIT);
            //PWMSetSpeedConsigne(25, MOTEUR_GAUCHE);
            stateRobot = STATE_AVANCE_EN_COURS;
            break;
        case STATE_AVANCE_EN_COURS:
            //SetNextRobotStateInAutomaticMode();
            consigneEvitement();
            if(Abs(robotState.vitesseDroiteCommandeCourante) < 5 && 
                    Abs(robotState.vitesseGaucheCommandeCourante) < 5 &&
                    robotState.distanceTelemetreCentre < 30 )
                stateRobot = STATE_RECULE ;
                      
            break;
        case STATE_RECULE:
           PWMSetSpeedConsigne(-15, MOTEUR_DROIT);
           PWMSetSpeedConsigne(-15, MOTEUR_GAUCHE);
           timestamp = 0;
           
           stateRobot = STATE_RECULE_EN_COURS;
            
            break;
        case STATE_RECULE_EN_COURS:
           
           if( timestamp > 600)
           stateRobot = STATE_TOURNE_SUR_PLACE_GAUCHE;
            
            break;    
        case STATE_TOURNE_GAUCHE:
           PWMSetSpeedConsigne(25, MOTEUR_DROIT);
           PWMSetSpeedConsigne(0, MOTEUR_GAUCHE);
            
           if(robotState.distanceTelemetreCentre  > 30)
           stateRobot = STATE_TOURNE_GAUCHE_EN_COURS;
            
            break;
        case STATE_TOURNE_GAUCHE_EN_COURS:
            // SetNextRobotStateInAutomaticMode();
            consigneEvitement();
             if(robotState.vitesseDroiteCommandeCourante == 0 && robotState.vitesseGaucheCommandeCourante == 0){
                stateRobot = STATE_TOURNE_GAUCHE;
            } else{
               stateRobot = STATE_AVANCE;  
            } 
            break;
        case STATE_TOURNE_DROITE:
            PWMSetSpeedConsigne(0, MOTEUR_DROIT);
            PWMSetSpeedConsigne(25, MOTEUR_GAUCHE);
            stateRobot = STATE_TOURNE_DROITE_EN_COURS;
            break;
        case STATE_TOURNE_DROITE_EN_COURS:
            //SetNextRobotStateInAutomaticMode();
            break;
        case STATE_TOURNE_SUR_PLACE_GAUCHE:
            PWMSetSpeedConsigne(10, MOTEUR_DROIT);
            PWMSetSpeedConsigne(-10, MOTEUR_GAUCHE);
            timestamp=0;
            stateRobot = STATE_TOURNE_SUR_PLACE_GAUCHE_EN_COURS;
            break;
        case STATE_TOURNE_SUR_PLACE_GAUCHE_EN_COURS:
            //SetNextRobotStateInAutomaticMode();
            if(timestamp>450)stateRobot = STATE_AVANCE;
            
            break;
        case STATE_TOURNE_SUR_PLACE_DROITE:
            PWMSetSpeedConsigne(-25, MOTEUR_DROIT);
            PWMSetSpeedConsigne(25, MOTEUR_GAUCHE);
            stateRobot = STATE_TOURNE_SUR_PLACE_DROITE_EN_COURS;
            break;
        case STATE_TOURNE_SUR_PLACE_DROITE_EN_COURS:
            //SetNextRobotStateInAutomaticMode();
            break;
        default:
            stateRobot = STATE_ATTENTE;
            break;
    }
}
unsigned char nextStateRobot = 0;

void consigneEvitement(void) {
    float f_ext_G = (robotState.distanceTelemetreExtremGauche < 60) ? (Kex / robotState.distanceTelemetreExtremGauche) : 0;
    float f_G = (robotState.distanceTelemetreGauche < 60) ? (Ka / robotState.distanceTelemetreGauche) : 0;
    float f_centre = (robotState.distanceTelemetreCentre < 60) ? (Kc / robotState.distanceTelemetreCentre) : 0;
    float f_D = (robotState.distanceTelemetreDroit < 60) ? (Ka / robotState.distanceTelemetreDroit) : 0;
    float f_ext_D = (robotState.distanceTelemetreExtremDroit < 60) ? (Kex / robotState.distanceTelemetreExtremDroit) : 0;
    
    int vitesse_gauche = 20 + (int)(f_ext_D + f_D)-(int)(f_ext_G + f_G)- f_centre;
    int vitesse_droite = 20 + (int)(f_ext_G + f_G)-(int)(f_ext_D + f_D)-f_centre;
    
    vitesse_gauche = (int)(LimitToInterval(vitesse_gauche , -30,30));
    vitesse_droite = (int)(LimitToInterval(vitesse_gauche , -30,30));
    
    PWMSetSpeedConsigne(vitesse_droite, MOTEUR_DROIT);
    PWMSetSpeedConsigne(vitesse_gauche, MOTEUR_GAUCHE);
}

void SetNextRobotStateInAutomaticMode() {
    unsigned char positionObstacle = PAS_D_OBSTACLE;
    //
