/* 
 * File:   PWM.h
 * Author: E306_PC1
 *
 * Created on 8 septembre 2026, 16:26
 */

#ifndef PWM_H
#define	PWM_H

#define G 1
#define D 2

#define PWMPER 24.0




void InitPWM(void);
void  PWMSetSpeedConsigne(char moteur,float vitessepourcent);
//void PWMSetSpeed(uint8_t moteur,float vitesseEnPourcents);

#endif	/* PWM_H */

