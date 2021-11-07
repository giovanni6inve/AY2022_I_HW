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
#include "settings.h"
#include "project.h"
#define overflow 65535
#define freq 50
#define Period_Timer_ADC 10*10^(-3) 
#define max_samples 15
extern uint8_t slaveBuffer[];
extern int32 value_digit[max_samples*2+1];
int32 sum=0;
uint16 nReset;
//extern uint8_t flag_sample_ch0, flag_sample_ch1, Nsamples;

    void settings(flag_sample_ch0, flag_sample_ch1, Nsamples){
    Timer_ADC_Stop();
    nReset=overflow - overflow/(2*Nsamples*freq*Period_Timer_ADC);
    //Timer_ADC_WritePeriod(100);
    Timer_ADC_WriteCounter(nReset);
    Timer_ADC_Enable();

    if ((flag_sample_ch0==1) &( flag_sample_ch1 == 1)){
        if (slaveBuffer[1]==1) AMux_Select(0);

        value_digit[slaveBuffer[1]] = ADC_DelSig_Read32();
        
        
        
        if (slaveBuffer[1]==Nsamples){
            for (int i=1;i<=Nsamples;i++){
                sum += value_digit[i];
            }
            sum= sum/Nsamples;
            slaveBuffer[3]=(sum & 0xFFFF) >>8;
            slaveBuffer[4]= sum  & 0xFF;    //vanno comunicati ogni 50Hz
            sum=0;
            AMux_Select(1);
        }
        if (slaveBuffer[1]==Nsamples*2){
            for (int j=Nsamples+1;j<=Nsamples*2;j++){
                sum += value_digit[j];
            }
            
            sum= sum/Nsamples;
            slaveBuffer[5]=(sum & 0xFFFF) >>8; 
            slaveBuffer[6]= sum & 0xFF;            
            sum=0;
            slaveBuffer[1]=0;
        }
    
    
    }
    
    if ((flag_sample_ch0==1) &( flag_sample_ch1 == 0)){
        AMux_Select(0);
        value_digit[slaveBuffer[1]] = ADC_DelSig_Read32();
        slaveBuffer[5]=0;
        slaveBuffer[6]=0;
        
        
        if (slaveBuffer[1]==Nsamples){
            for (int i=1;i<=Nsamples;i++){
                sum += value_digit[i];
            }
            sum= sum/Nsamples;

        }
        if (slaveBuffer[1]==2*Nsamples)  {  
            slaveBuffer[3]=(sum & 0xFFFF) >>8;
            slaveBuffer[4]= sum  & 0xFF;   
            sum=0;        
            slaveBuffer[1]=0;
        }    
        
        
    
    }
    
    if ((flag_sample_ch0==0) &( flag_sample_ch1 == 1)){
        AMux_Select(1);
        value_digit[slaveBuffer[1]] = ADC_DelSig_Read32();
        slaveBuffer[3]=0;
        slaveBuffer[4]=0;
        
        
        if (slaveBuffer[1]==Nsamples){
            for (int i=1;i<=Nsamples;i++){
                sum += value_digit[i];
            }
            sum= sum/Nsamples;
        }
        if (slaveBuffer[1]==2*Nsamples)  {  
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
/* [] END OF FILE */
