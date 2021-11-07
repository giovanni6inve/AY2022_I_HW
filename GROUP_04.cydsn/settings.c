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
#include "project.h"
extern uint8_t slaveBuffer[];
extern int32 value_digit[11];

void settings(flag_sample_ch0, flag_sample_ch1, Nsamples){
    //timer    
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
    
}
/* [] END OF FILE */
