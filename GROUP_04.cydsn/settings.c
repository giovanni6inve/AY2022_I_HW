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
#include <stdint.h>
#include "settings.h"
#include "project.h"
#define offset_temp_mv 750 //mV, for temperature sensor calbration
#define coef_temp 10 //mV/C, for temperature sensor calbration
#define offset_temp_c 25 //C , for temperature sensor calibration
#define offset_lux 0.1//lux, for LDR calibration
#define delta_lux 10000-0.1//lux for LDR calibration
#define delta_ohm 100-1000000//ohm for LDR calibration
#define offset_ohm 10000000//ohm for LDR calibration
#define vcc 5//V for LDR calibration
#define R 10000//ohm for LDR calibration
#define clk_freq 10000 //Hz this is the frequency of the timer's clock
#define comm_freq 50 //Hz this is the frequency at which the samples' average must be communicated (updated)
#define max_samples 15 //having 4 r/w bits in the CONTROL REGISTER 0 to specify the number of samples (within 50Hz to average) these may vary between 1 and 15
extern uint8_t slaveBuffer[]; //contains the slave's registers: CONTROL REGISTER 0, CONTROL REGISTER 1, MSB and LSB of channel 0 and MSB and LSB of channel 1 (notice: this is the correct order to communicate 16 bit values by I2C)
extern int32 value_digit[max_samples*2];//with 4 bits to do so 15 samples to averge may be desired at most, since it is for 2 sensors 30 
uint8 flag_avgOVF;
uint32 count_LEDovf=0;
int32 sum=0, sum_0=0;
//uint32_t* sum_temp = (int*)(&sum_0);
//uint32_t* sum_ldr = (int*)(&sum);

uint8 count=-1;

               // Test value
   
    /*this function takes in input the status: which channels to sample, and how many samples to average and 
    writes the registers of the slave buffer with the avg sample values and sets the timer and the ADC in 
    order to satisfy the requirements set by these parameters */
    void settings(int flag_sample_ch0, int flag_sample_ch1, int Nsamples){
    
    /* set the timer's period according to the number of samples to average each 50Hz per channel*/
    Timer_ADC_Stop(); //stop the timer to reset the period
    slaveBuffer[1]= clk_freq/(comm_freq*2*Nsamples); //sampling is done at each timer's overflow so its period is set thus
    //period changes when the counter is reloded so we are sure the timer counts from 0 to overflow
    Timer_ADC_WritePeriod(slaveBuffer[1]); //done following ISR's (calls function settings) interrogation of what is written in the Bridge Control Panel
    Timer_ADC_Enable(); //reactivates timer once period is changed

    if ((flag_sample_ch0 == 1) & (flag_sample_ch1 == 1)){ //status 11: built in LED is on and both channels are sampled
        if (count==0) AMux_Select(0); //connect ch0 with the ADC for Nsamples conversions
        
        /*ADC is set with 16 bit resolution, read it and save in value_digit the converted value which
        will then be overwritten first to convert into V (or mV) and then into lux or decgrees Celsius*/
        value_digit[count] = ADC_DelSig_Read32();
                
        if (value_digit[count]>65535) value_digit[count]=65535;
        if (value_digit[count]<0) value_digit[count]=0; //for robustness
        
        
        if (count==Nsamples-1){ //once Nsamples for the ch0 are collected (starting count from 0 Nsamples are obtained at count=Nsamples-1)
            for (int i=0;i<Nsamples;i++){
                
                //value_digit[i]=ADC_DelSig_CountsTo_mVolts(value_digit[i]);
                //value_digit[i]=(value_digit[i]-offset_temp_mv)/coef_temp + offset_temp_c;
                if (value_digit[i]>65535) value_digit[i]=65535;
                if (value_digit[i]<0) value_digit[i]=0; //limit the dynamic to positive values within 65535 for robustness
               
                
                sum_0 += value_digit[i]; //sum them
            }
            sum_0= sum_0/Nsamples; //to compute the average
            
            if (sum_0>65535) { //warn user of invalid value (see below)
                sum_0=65535;
                flag_avgOVF=1;  
            }
            
            AMux_Select(1); //connect ch1 to the ADC for Nsamples conversions after the ones for ch0 are done
        }
        
        if (count==Nsamples*2-1){ //also the Nsamples for ch1 are collected (we have Nsamples*2 at the count Nsamples*2-1 since count start from 0)
            for (int j=Nsamples;j<Nsamples*2;j++){
                
                //value_digit[j]=ADC_DelSig_CountsTo_Volts(value_digit[j]);
                //value_digit[j]= offset_lux + (delta_lux)*(R*(vcc-value_digit[j])/vcc*(1-(vcc-value_digit[j])/vcc)-offset_ohm)/delta_ohm ; 
                
                
                if (value_digit[j]>65535) value_digit[j]=65535;
                if (value_digit[j]<0) value_digit[j]=0; //limit the dynamic to positive values within 65535 for robustness
                
                sum += value_digit[j]; //sum them
            }
            
            sum= sum/Nsamples; //for average computation
            
            if (sum>65535) {  //warn user of invalid value (see below)
                sum=65535;
                flag_avgOVF=1;  
            }
            
            slaveBuffer[3]=(sum_0 & 0xFFFF) >>8; //since sum has 32 bits (output of read 32) we must extract here the MSB out of 2 bytes and right adjust it
            slaveBuffer[4]= sum_0 & 0xFF;   //LSB 
            sum_0=0; //reinitialize for the next 1/50Hz period
            
            slaveBuffer[5]=(sum & 0xFFFF) >>8; 
            slaveBuffer[6]= sum & 0xFF;            
            sum=0; //reinitialize for the next 1/50Hz period
            count=-1; //reinitialize for the next 1/50Hz period
        }
    
    
    }
    
     if ((flag_sample_ch0==1) &( flag_sample_ch1 == 0)){ //only the channel 0 is required
        AMux_Select(0);
        
        value_digit[count] = ADC_DelSig_Read32();
        if (value_digit[count]>65535) value_digit[count]=65535;
        if (value_digit[count]<0) value_digit[count]=0;
        
        slaveBuffer[5]=0;
        slaveBuffer[6]=0;
        
        
        if (count==Nsamples-1){ 
            for (int i=1;i<Nsamples;i++){
                
                //value_digit[i]=ADC_DelSig_CountsTo_mVolts(value_digit[i]);
                //value_digit[i]=(value_digit[i]-offset_temp_mv)/coef_temp + offset_temp_c;
                if (value_digit[i]>65535) value_digit[i]=65535;
                if (value_digit[i]<0) value_digit[i]=0; //limit the dynamic to positive values within 65535 for robustness
                
                sum_0 += value_digit[i];
            }
            sum_0= sum_0/Nsamples;
            if (sum_0>65535) { //warn user of invalid value (see below)
                sum_0=65535;
                flag_avgOVF=1;  
            }

        }
        
        
        
        
        /*in order to use the same timer period both for 2 or 1 channels to be sampled we finish sampling 
        after 10ms here but we wait for 20ms to pass (1/50Hz) to update the register*/
       if (count==2*Nsamples-1)  { 
            slaveBuffer[3]=(sum_0 & 0xFFFF) >>8;
            slaveBuffer[4]= sum_0  & 0xFF;   
            sum_0=0;        
            count=-1;
        }    
        
        
    
    }
    
    if ((flag_sample_ch0==0) &( flag_sample_ch1 == 1)){ //only the channel 1 samples are requested
        AMux_Select(1);
        
        value_digit[count] = ADC_DelSig_Read32();
        if (value_digit[count]>65535) value_digit[count]=65535;
        if (value_digit[count]<0) value_digit[count]=0;        
        
        slaveBuffer[3]=0;
        slaveBuffer[4]=0;
        
        
        if (count==Nsamples-1){
            for (int i=1;i<Nsamples;i++){
                
                //value_digit[i]=ADC_DelSig_CountsTo_Volts(value_digit[i]);
                //value_digit[i]=  offset_lux + (delta_lux)*(R*(vcc-value_digit[i])/vcc*(1-(vcc-value_digit[i])/vcc)-offset_ohm)/delta_ohm;
                if (value_digit[i]>65535) value_digit[i]=65535;
                if (value_digit[i]<0) value_digit[i]=0; //limit the dynamic to positive values within 65535 for robustness
                
                sum += value_digit[i];
            }
            sum= sum/Nsamples;
            if (sum>65535) { //warn user of invalid value (see below)
                sum=65535;
                flag_avgOVF=1;  
            }
            
        }
        
        /*in order to use the same timer period both for 2 or 1 channels to be sampled we finish sampling 
        after 10ms here but we wait for 20ms to pass (1/50Hz) to update the register*/
        
        
        if (count==2*Nsamples-1)  {  
            slaveBuffer[5]=(sum & 0xFFFF) >>8;
            slaveBuffer[6]= sum & 0xFF;   
            sum=0;        
            count=-1;
        }    
    
    }    
    
    if ((flag_sample_ch0==0) &( flag_sample_ch1 == 0)){//the user does not wish to receive either sensor's averaged value
        
        //stop ADC, and start it in others? check the register to check its activation or not???????????         
        
        //both bytes of both channels are set to 0
        slaveBuffer[3]=0;
        slaveBuffer[4]=0;
        slaveBuffer[5]=0;
        slaveBuffer[6]=0;
        
        count=-1; //in case the status bits are reset during any other configuration of them and not exactly at the cycle's end
    }    
    
    count++; //increase the count for each ISR on timer overflow

//LED to warn the user of register overflow
    if (flag_avgOVF==1){      //if either the sum of ch1 or ch0 samples overflows (goes beyond 655535) 
        Pin_avgOVF_Write(1); // the visualized value will be wrong: this LED turning on warns of such an eventuality
        count_LEDovf++;
        if(count_LEDovf==20000000){ //it remains turned on enough time for the user to see it (in any case of ISR period that may be required)
            Pin_avgOVF_Write(0);
            count_LEDovf=0;
            flag_avgOVF=0;
        }
    }
}
/* [] END OF FILE */
    
    
    
    
 
 // get "bits view" of test value


    
    
