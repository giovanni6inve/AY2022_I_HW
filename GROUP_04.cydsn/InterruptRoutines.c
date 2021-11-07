/* ========================================
 *
 * Copyright YOUR COMPANY, THE YEAR
 * All Rights Reserved
 * UNPUBLISHED, LICENSED SOFTWARE.
 *
 * CONFIDENTIAL AND PROPRIETARY INFORMATION
 * WHICH IS THE PROPERTY OF your company.
 *
 * ========================================
*/
#include "InterruptRoutines.h" 
#include "project.h"
#include "settings.h"
#define stop 0b00
#define ch0 0b01
#define ch1 0b10
#define both 0b11
extern uint8_t slaveBuffer[];
int flag_ch0, flag_ch1, Nsample;    
extern int32 value_digit[11];


CY_ISR(Custom_Timer_Count_ISR)
{
    // Read timer status register to pull interrupt line low
    Timer_ADC_ReadStatusRegister();
    
    Nsample = (slaveBuffer[0] >> 2) & 0b1111; //default is 5: 0101
    settings(flag_ch0, flag_ch1, Nsample);
    
}

void EZI2C_ISR_ExitCallback(void)
{   
    
    switch (slaveBuffer[0] & 0b11){
    case stop: //LED off and stop sampling
        Pin_LED_Write(0);
        flag_ch0=0;
        flag_ch1=0;
    break;
    case ch0: //LED off and sample only ch 0 (Temperature sensor)
        Pin_LED_Write(0);
        flag_ch0=1;
        flag_ch1=0;
    break; //LED off and sample only ch 1 (LDR)
    case ch1:
        Pin_LED_Write(0);
        flag_ch0=0;
        flag_ch1=1;
    break;
    case both: //LED on and sample both channels
        Pin_LED_Write(1);
        flag_ch0=1;
        flag_ch1=1;
    break;
    }
    
    //settings(flag_sample_ch0, flag_sample_ch1, Nsamples);
} //to write a different num of samples to avg you need to give a hex number for the whole control register 0 so to write ...
/* [] END OF FILE */
