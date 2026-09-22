
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
unsigned char stateRobot;

/*
#define Kex 130
#define Kc 400
#define Ka 282
 */
// ca ca detecte la direction en prenant en compte d abord le plus preoccupant
unsigned char PositionObstacle(void) {
    if (robotState.distanceTelemetreCentre < SEUIL_ARRET)
        return OBSTACLE_EN_FACE;
    if (robotState.distanceTelemetreDroit < SEUIL_ARRET)
        return OBSTACLE_A_DROITE;
    if (robotState.distanceTelemetreGauche < SEUIL_ARRET)
        return OBSTACLE_A_GAUCHE;
    if (robotState.distanceTelemetreExtremDroit < SEUIL_ARRET)
        return OBSTACLE_A_EXTREME_DROITE;
    if (robotState.distanceTelemetreExtremGauche < SEUIL_ARRET)
        return OBSTACLE_A_EXTREME_GAUCHE;
    return PAS_D_OBSTACLE;
}
// PS j ai tente une nouvelle ecriture ici en gros 
//ca agis comme une fonction et renvoi 1 si le return et vrai 

unsigned char VoieLibre(void) {
    return (robotState.distanceTelemetreCentre > SEUIL_DEGAGE) &&
            (robotState.distanceTelemetreDroit > SEUIL_DEGAGE) &&
            (robotState.distanceTelemetreGauche > SEUIL_DEGAGE) &&
            (robotState.distanceTelemetreExtremDroit > SEUIL_DEGAGE) &&
            (robotState.distanceTelemetreExtremGauche > SEUIL_DEGAGE);
}
// et ca j ai modifier pour faire comme le prof mais a ma sauce aves des truc de cachan comme le timestamp
// j ai demande a claude de commente j avais la flemme XD
void OperatingSystemLoop(void) {
    switch (stateRobot) {

        case STATE_AVANCE:
            PWMSetSpeedConsigne(VITESSE_CROISIERE, MOTEUR_DROIT);
            PWMSetSpeedConsigne(VITESSE_CROISIERE, MOTEUR_GAUCHE);
            stateRobot = STATE_AVANCE_EN_COURS;
            break;

        case STATE_AVANCE_EN_COURS:
        {
            unsigned char obstacle = PositionObstacle();
            if (obstacle == OBSTACLE_A_DROITE || obstacle == OBSTACLE_A_EXTREME_DROITE) {
                stateRobot = STATE_TOURNE_SUR_PLACE_GAUCHE; // on s'écarte vers la gauche
            } else if (obstacle == OBSTACLE_A_GAUCHE || obstacle == OBSTACLE_A_EXTREME_GAUCHE) {
                stateRobot = STATE_TOURNE_SUR_PLACE_DROITE; // on s'écarte vers la droite
            } else if (obstacle == OBSTACLE_EN_FACE) {
                // obstacle pile devant : on pivote du côté le plus dégagé
                stateRobot = (robotState.distanceTelemetreGauche >= robotState.distanceTelemetreDroit)
                        ? STATE_TOURNE_SUR_PLACE_GAUCHE
                        : STATE_TOURNE_SUR_PLACE_DROITE;
            }
            // sinon on reste en STATE_AVANCE_EN_COURS, rien à faire
            break;
        }

        case STATE_TOURNE_SUR_PLACE_GAUCHE:
            PWMSetSpeedConsigne(VITESSE_PIVOT, MOTEUR_DROIT);
            PWMSetSpeedConsigne(-VITESSE_PIVOT, MOTEUR_GAUCHE);
            timestamp = 0;
            stateRobot = STATE_TOURNE_SUR_PLACE_GAUCHE_EN_COURS;
            break;

        case STATE_TOURNE_SUR_PLACE_GAUCHE_EN_COURS:
            if (timestamp > DUREE_PIVOT_MINI && VoieLibre())
                stateRobot = STATE_AVANCE;
            // sinon on continue de pivoter (on reste dans cet état)
            break;

        case STATE_TOURNE_SUR_PLACE_DROITE:
            PWMSetSpeedConsigne(-VITESSE_PIVOT, MOTEUR_DROIT);
            PWMSetSpeedConsigne(VITESSE_PIVOT, MOTEUR_GAUCHE);
            timestamp = 0;
            stateRobot = STATE_TOURNE_SUR_PLACE_DROITE_EN_COURS;
            break;

        case STATE_TOURNE_SUR_PLACE_DROITE_EN_COURS:
            if (timestamp > DUREE_PIVOT_MINI && VoieLibre())
                stateRobot = STATE_AVANCE;
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
        LED_BLANCHE_1 = (robotState.distanceTelemetreExtremGauche > 30) ? 1 : 0;

        volts = ((float) result [1])* 3.3 / 4096;
        robotState.distanceTelemetreGauche = 34 / volts - 5;

        volts = ((float) result [2])* 3.3 / 4096;
        robotState.distanceTelemetreCentre = 34 / volts - 5;
        LED_BLEUE_1 = (robotState.distanceTelemetreCentre > 30) ? 1 : 0;

        volts = ((float) result [3])* 3.3 / 4096;
        robotState.distanceTelemetreDroit = 34 / volts - 5;

        volts = ((float) result [4])* 3.3 / 4096;
        robotState.distanceTelemetreExtremDroit = 34 / volts - 5;
        LED_ORANGE_1 = (robotState.distanceTelemetreExtremDroit > 30) ? 1 : 0;
    }
    if (robotState.vitesseDroiteCommandeCourante <= -25 && robotState.vitesseGaucheCommandeCourante >= 25) {
        LED_VERTE_1 = 1;
    } else {
        LED_VERTE_1 = 0;
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
    //éDtermination de la position des obstacles en fonction des ééètlmtres
    
    if (robotState.distanceTelemetreDroit < 30 &&
            robotState.distanceTelemetreCentre > 20 &&
            robotState.distanceTelemetreGauche > 30) //Obstacle àdroite
        positionObstacle = OBSTACLE_A_DROITE;
    else if (robotState.distanceTelemetreDroit > 30 &&
            robotState.distanceTelemetreCentre > 20 &&
            robotState.distanceTelemetreGauche < 30) //Obstacle àgauche
        positionObstacle = OBSTACLE_A_GAUCHE;
    else if (robotState.distanceTelemetreCentre < 20) //Obstacle en face
        positionObstacle = OBSTACLE_EN_FACE;
    else if (robotState.distanceTelemetreDroit > 30 &&
            robotState.distanceTelemetreCentre > 20 &&
            robotState.distanceTelemetreGauche > 30) //pas d?obstacle
        positionObstacle = PAS_D_OBSTACLE;
     
    if ((stateobs == 0 )&& ((robotState.distanceTelemetreExtremDroit <= 10) || (robotState.distanceTelemetreDroit <= 30 )||
            (robotState.distanceTelemetreCentre <= 30 ) || (robotState.distanceTelemetreGauche <= 30) ||
            (robotState.distanceTelemetreExtremGauche <= 10))) {
        stateobs = 1;
        positionObstacle = OBSTACLE_EN_FACE;

    } else if ((stateobs == 1) && ((robotState.distanceTelemetreDroit > 10 )||
            (robotState.distanceTelemetreCentre > 10) || (robotState.distanceTelemetreGauche > 10))) {
        positionObstacle = PAS_D_OBSTACLE;
    }


    //éDtermination de lé?tat àvenir du robot
    if (positionObstacle == PAS_D_OBSTACLE)
        nextStateRobot = STATE_AVANCE;
    else if (positionObstacle == OBSTACLE_A_DROITE)
        nextStateRobot = STATE_TOURNE_GAUCHE;
    else if (positionObstacle == OBSTACLE_A_GAUCHE)
        nextStateRobot = STATE_TOURNE_DROITE;
    else if (positionObstacle == OBSTACLE_EN_FACE)
        nextStateRobot = STATE_TOURNE_SUR_PLACE_GAUCHE;
    //Si l?on n?est pas dans la transition de lé?tape en cours
    if (nextStateRobot != stateRobot - 1)
        stateRobot = nextStateRobot;
}

 */