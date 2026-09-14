/* 
 * File:   ADC.h
 * Author: E306_PC1
 *
 * Created on 14 septembre 2026, 08:22
 */

#ifndef ADC_H
#define	ADC_H

void InitADC1(void);
void ADC1StartConversionSequence(void);
unsigned int * ADCGetResult(void);
unsigned char ADCIsConversionFinished(void);
void ADCClearConversionFinishedFlag(void);

#endif	/* ADC_H */

