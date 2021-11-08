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
#define max_samples 15 //having 4 r/w bits in the CONTROL REGISTER 0 to specify the number of samples (within 50Hz to average) these may vary between 1 and 15
extern uint8_t slaveBuffer[]; //contains the slave's registers: CONTROL REGISTER 0, CONTROL REGISTER 1, MSB and LSB of channel 0 and MSB and LSB of channel 1 (notice: this is the correct order to communicate 16 bit values by I2C)
int flag_ch0, flag_ch1, Nsample;    
extern int32 value_digit[max_samples*2]; //with 4 bits to do so 15 samples to averge may be desired at most, since it is for 2 sensors 30 

/*to be performed at each timer's overflow (timer's period is set as required by the user-configurable 
parameters)*/
CY_ISR(Custom_Timer_Count_ISR) 
{
    //Read timer status register to pull interrupt line low
    Timer_ADC_ReadStatusRegister();
    
    Nsample = (slaveBuffer[0] >> 2) & 0b1111; //default is 5: 0101
    /*it makes no sense to average 0 samples any input to the bridge control panel's write between 00 and 03 is
    corrected to give the same output as 04 to 07 respectively*/
    /*notice no value above 3f may be given in input to the bridge control manel since the 2 status bits may be
    0b11 at most and the 4 ones for the number of samples 0b1111 (15 in dec), this makes 0b111111 which 
    corresponds to 3f in hex*/
    if (Nsample==0) Nsample=1; 
    
    
    /*this function takes in input the status: which channels to sample, and how many samples to average and 
    writes the registers of the slave buffer with the avg sample values and sets the timer and the ADC in 
    order to satisfy the requirements set by these parameters */
    settings(flag_ch0, flag_ch1, Nsample); 
    
}

void EZI2C_ISR_ExitCallback(void) //to be performed upon command from the bridge control panel
{   
    //check the contence og the least significant bits of the CONTROL REGISTER 0 (channels to sample)
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
    
}
/* [] END OF FILE */
