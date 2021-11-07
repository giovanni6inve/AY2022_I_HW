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
int32 value_digit[11];

int32 sum=0;
CY_ISR(Custom_Timer_Count_ISR)
{
    // Read timer status register to pull interrupt line low
    Timer_ADC_ReadStatusRegister();
    // Increment counter in slave buffer

    if ((flag_sample_ch0 == 1) &  ( flag_sample_ch1 == 1)){
        if(slaveBuffer[1]==1) AMux_Select(0);
        value_digit[slaveBuffer[1]] = ADC_DelSig_Read32();
        if (slaveBuffer[1]==5){
            
            for (int i=1;i<6;i++){
                sum += value_digit[i];
            }
            sum= sum/5.0;
            slaveBuffer[3]=(sum & 0xFFFF) >>8;
            slaveBuffer[4]= sum  & 0xFF;    //vanno comunicati ogni 50Hz
            sum=0;
            AMux_Select(1);
           
        }
        
        if (slaveBuffer[1]==10){
            for (int j=6;j<11;j++){
                sum += value_digit[j];
            }
            
            sum= sum/5.0;
            slaveBuffer[5]=(sum & 0xFFFF) >>8; 
            slaveBuffer[6]= sum & 0xFF;            
            AMux_Select(0);
            sum=0;
            slaveBuffer[1]=0;

        }
    
    
    }
            /* Temperature Sensor  */
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
        }    
        
        
    
    }
        
    if ((flag_sample_ch0==0) & ( flag_sample_ch1 == 1)){
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
        }    
    
    }    
    
    if ((flag_sample_ch0==0) &( flag_sample_ch1 == 0)){
        //Stop ADC or do nothing?         
        slaveBuffer[3]=0;
        slaveBuffer[4]=0;     
        slaveBuffer[5]=0;
        slaveBuffer[6]=0;    
    }    
    slaveBuffer[1]++;
}

void EZI2C_ISR_ExitCallback(void)
{   
    
    
    //slaveBuffer[1]=1;
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
        flag_sample_ch1=1;
    break;
    }
    
   
} //to write a different num of samples to avg you need to give a hex number for the whole control register 0 so to write ...
/* [] END OF FILE */
