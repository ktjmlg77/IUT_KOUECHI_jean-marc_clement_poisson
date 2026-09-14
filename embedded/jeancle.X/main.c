#include <stdio.h>
#include <stdlib.h>
#include <xc.h>
#include "ADC.h"
#include "ChipConfig.h"
#include "IO.h"
#include "timer.h"
#include "PWM.h"
#include "Robot.h"

int main(void) {
    //Initialisation oscillateur
    InitOscillator();
    // Configuration des input et output (IO)
    InitIO();
    InitPWM();
    InitTimer1();
    InitTimer23();
    InitADC1();
    LED_BLANCHE_1 = 0;
    LED_BLEUE_1 = 0;
    LED_ORANGE_1 = 0;
    LED_ROUGE_1 = 0;
    LED_VERTE_1 = 0;
    PWM_EN = 1;

    PWMSetSpeedConsigne(D, 20);
    PWMSetSpeedConsigne(G, 20);
    // Boucle Principale
    while (1) {
        if (ADCIsConversionFinished()) {
            ADCClearConversionFinishedFlag();
            unsigned int * result = ADCGetResult();
            float volts = ((float) result [0])* 3.3 / 4096;
            robotState.distanceTelemetreGauche = 34 / volts - 5;
            LED_BLANCHE_1 = (robotState.distanceTelemetreGauche > 30) ?  1 : 0;
            volts = ((float) result [1])* 3.3 / 4096;
            robotState.distanceTelemetreCentre = 34 / volts - 5;
            LED_BLEUE_1 = (robotState.distanceTelemetreCentre > 30) ?  1 : 0;
            volts = ((float) result [2])* 3.3 / 4096;
            robotState.distanceTelemetreDroit = 34 / volts - 5;
            LED_ORANGE_1 = (robotState.distanceTelemetreDroit > 30) ?  1 : 0;
        }
    }

    // fin main
}


