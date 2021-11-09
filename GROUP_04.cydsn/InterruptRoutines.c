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
#define clk_freq 10000 //Hz this is the frequency of the timer's clock
#define comm_freq 50 //Hz this is the frequency at which the samples' average must be communicated (updated)
extern uint8_t slaveBuffer[]; //contains the slave's registers: CONTROL REGISTER 0, CONTROL REGISTER 1, MSB and LSB of channel 0 and MSB and LSB of channel 1 (notice: this is the correct order to communicate 16 bit values by I2C)
int flag_ch0, flag_ch1, Nsample, flag_ch0_temp, flag_ch1_temp;    
extern int32 value_digit[max_samples*2]; //with 4 bits to do so 15 samples to averge may be desired at most, since it is for 2 sensors 30 
int8 count=-1; //increased at each ISR and reset to -1 (and immediately increased to 0) only when a cycle (update of slave buffer's averaged samples registes) is comlete

/*to be performed at each timer's overflow (timer's period is set as required by the user-configurable 
parameters)*/
CY_ISR(Custom_Timer_Count_ISR) 
{
    //Read timer status register to pull interrupt line low
    Timer_ADC_ReadStatusRegister();
    
    /*for robustess the user must wait for a cycle to finish before the required modifications to Nsamples
    or the status bits take action*/
    if (count==-1) {
        Nsample = (slaveBuffer[0] >> 2) & 0b1111; //default is 5: 0101    
        /*it makes no sense to average 0 samples any input to the bridge control panel's write between 00 and 03 is
        corrected to give the same output as 04 to 07 respectively*/
        /*notice no value above 3f may be given in input to the bridge control manel since the 2 status bits may be
        0b11 at most and the 4 ones for the number of samples 0b1111 (15 in dec), this makes 0b111111 which 
        corresponds to 3f in hex*/
        if (Nsample==0) Nsample=1;
        
        flag_ch0_temp=flag_ch0;
        flag_ch1_temp=flag_ch1;
        
        /* set the timer's period according to the number of samples to average each 50Hz per channel*/
        Timer_ADC_Stop(); //stop the timer to reset the period
        slaveBuffer[1]= clk_freq/(comm_freq*2*Nsample); //sampling is done at each timer's overflow so its period is set thus
    
        //period changes when the counter is reloded so we are sure the timer counts from 0 to overflow
        Timer_ADC_WritePeriod(slaveBuffer[1]); //done following ISR's (calls function settings) interrogation of what is written in the Bridge Control Panel
        Timer_ADC_Enable(); //reactivates timer once period is changed
    }
    count++;
    
 
    
    
    /*this function takes in input the status: which channels to sample, and how many samples to average and 
    writes the registers of the slave buffer with the avg sample values and sets the timer and the ADC in 
    order to satisfy the requirements set by these parameters */
    settings(flag_ch0_temp, flag_ch1_temp, Nsample); 
    
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
