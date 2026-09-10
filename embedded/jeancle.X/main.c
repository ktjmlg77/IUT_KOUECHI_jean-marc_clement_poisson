#include <stdio.h>
#include <stdlib.h>
#include <xc.h>
#include "ChipConfig.h"
#include "IO.h"
#include "timer.h"
#include "PWM.h"

int main(void) {
    //Initialisation oscillateur
    InitOscillator();
    // Configuration des input et output (IO)
    InitIO();
    InitPWM();
    InitTimer1();
    InitTimer23();
    LED_BLANCHE_1 = 0;
    LED_BLEUE_1 = 0;
    LED_ORANGE_1 = 0;
    LED_ROUGE_1 = 0;
    LED_VERTE_1 = 0;
    PWM_EN = 1;

    PWMSetSpeedConsigne(D,20);
    PWMSetSpeedConsigne(G,20);
    // Boucle Principale
    while (1) {
           ;
    }

    // fin main
}


