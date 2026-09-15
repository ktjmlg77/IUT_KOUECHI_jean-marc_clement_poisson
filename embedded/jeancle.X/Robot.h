#ifndef ROBOT_H
#define ROBOT_H


typedef struct robotStateBITS {
    unsigned char taskEnCours;
    float vitesseGaucheConsigne;
    float vitesseGaucheCommandeCourante;
    float vitesseDroiteConsigne;
    float vitesseDroiteCommandeCourante;
    float distanceTelemetreDroit;
    float distanceTelemetreCentre;
    float distanceTelemetreGauche;
    float distanceTelemetreExtremGauche;
    float distanceTelemetreExtremDroit;
} ROBOT_STATE_BITS;


extern volatile ROBOT_STATE_BITS robotState;

void PWMUpdateSpeed(void);

#endif /* ROBOT_H */

