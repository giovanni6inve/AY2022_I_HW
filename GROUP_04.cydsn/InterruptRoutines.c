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

extern uint8_t slaveBuffer[];
uint8_t flag_sample_ch0, flag_sample_ch1;    

CY_ISR(Custom_Timer_Count_ISR)
{
    // Read timer status register to pull interrupt line low
    Timer_ADC_ReadStatusRegister();
    // Increment counter in slave buffer
    slaveBuffer[1]++;
}

void EZI2C_ISR_ExitCallback(void)
{
    switch (slaveBuffer[0] & 0b11){
    case 0b00: //LED off and stop sampling
        Pin_LED_Write(0);
        flag_sample_ch0=0;
        flag_sample_ch1=0;
    break;
    case 0b01: //LED off and sample only ch 0 (Temperature sensor)
        Pin_LED_Write(0);
        flag_sample_ch0=1;
        flag_sample_ch1=0;
    break; //LED off and sample only ch 1 (LDR)
    case 0b10:
        Pin_LED_Write(0);
        flag_sample_ch0=0;
        flag_sample_ch1=1;
    break;
    case 0b11: //LED on and sample both channels
        Pin_LED_Write(1);
        flag_sample_ch0=1;
        flag_sample_ch0=1;
    break;
    }
    
   
} //to write a different num of samples to avg you need to give a hex number for the whole control register 0 so to write ...
/* [] END OF FILE */
