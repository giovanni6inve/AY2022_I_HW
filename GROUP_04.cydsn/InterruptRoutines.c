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
uint8_t flag_sample_ch0, flag_sample_ch1, Nsamples;    
extern int32 value_digit[11];
int32 sum=0;

CY_ISR(Custom_Timer_Count_ISR)
{
    // Read timer status register to pull interrupt line low
    Timer_ADC_ReadStatusRegister();
    
    if ((flag_sample_ch0==1) &( flag_sample_ch1 == 1)){
        if (slaveBuffer[1]==1) AMux_Select(0);

        value_digit[slaveBuffer[1]] = ADC_DelSig_Read32();
        
        
        
        if (slaveBuffer[1]==5){
            for (int i=1;i<6;i++){
                sum += value_digit[i];
            }
            sum= sum/5.0;
            slaveBuffer[3]=(sum & 0xFFFF) >>8;
            slaveBuffer[4]= sum  & 0xFF;    //vanno comunicati ogni 50Hz
            //AMux_Select(1);
            sum=0;
        }
        if (slaveBuffer[1]==6) AMux_Select(1);
        if (slaveBuffer[1]==10){
            for (int j=6;j<11;j++){
                sum += value_digit[j];
            }
            
            sum= sum/5.0;
            slaveBuffer[5]=(sum & 0xFFFF) >>8; 
            slaveBuffer[6]= sum & 0xFF;            
            //AMux_Select(0);
            sum=0;
            slaveBuffer[1]=0;
        }
    
    
    }
    
    if ((flag_sample_ch0==1) &( flag_sample_ch1 == 0)){
        AMux_Select(0);
        value_digit[slaveBuffer[1]] = ADC_DelSig_Read32();
        slaveBuffer[5]=0;
        slaveBuffer[6]=0;
        
        
        if (slaveBuffer[1]==5){
            for (int i=1;i<6;i++){
                sum += value_digit[i];
            }
            sum= sum/5.0;

        }
        if (slaveBuffer[1]==10)  {  
            slaveBuffer[3]=(sum & 0xFFFF) >>8;
            slaveBuffer[4]= sum  & 0xFF;   
            sum=0;        
            slaveBuffer[1]=0;
            //AMux_Select(0);
        }    
        
        
    
    }
    
    if ((flag_sample_ch0==0) &( flag_sample_ch1 == 1)){
        AMux_Select(1);
        value_digit[slaveBuffer[1]] = ADC_DelSig_Read32();
        slaveBuffer[3]=0;
        slaveBuffer[4]=0;
        
        
        if (slaveBuffer[1]==5){
            for (int i=1;i<6;i++){
                sum += value_digit[i];
            }
            sum= sum/5.0;
        }
        if (slaveBuffer[1]==10)  {  
            slaveBuffer[5]=(sum & 0xFFFF) >>8;
            slaveBuffer[6]= sum  & 0xFF;   
            sum=0;        
            slaveBuffer[1]=0;
            //AMux_Select(0);
        }    
    
    }    
    
    if ((flag_sample_ch0==0) &( flag_sample_ch1 == 0)){
        //Stop ADC or do nothing?         
        slaveBuffer[3]=0;
        slaveBuffer[4]=0;
        slaveBuffer[5]=0;
        slaveBuffer[6]=0;
        //AMux_Select(0);
    
    }    
    
        
    
    slaveBuffer[1]++;
}

void EZI2C_ISR_ExitCallback(void)
{   
    
    Nsamples = (slaveBuffer[0] >> 2) & 0b1111; //default is 5: 0101
    switch (slaveBuffer[0] & 0b11){
    case stop: //LED off and stop sampling
        Pin_LED_Write(0);
        flag_sample_ch0=0;
        flag_sample_ch1=0;
    break;
    case ch0: //LED off and sample only ch 0 (Temperature sensor)
        Pin_LED_Write(0);
        flag_sample_ch0=1;
        flag_sample_ch1=0;
    break; //LED off and sample only ch 1 (LDR)
    case ch1:
        Pin_LED_Write(0);
        flag_sample_ch0=0;
        flag_sample_ch1=1;
    break;
    case both: //LED on and sample both channels
        Pin_LED_Write(1);
        flag_sample_ch0=1;
        flag_sample_ch1=1;
    break;
    }
    
    settings(flag_sample_ch0, flag_sample_ch1, Nsamples);
} //to write a different num of samples to avg you need to give a hex number for the whole control register 0 so to write ...
/* [] END OF FILE */
