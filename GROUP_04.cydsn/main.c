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
#include "InterruptRoutines.h"

#define SLAVE_BUFFER_SIZE 7
#define cinque 0b0101

#define stop 0b00

uint8_t slaveBuffer[SLAVE_BUFFER_SIZE];
int32 value_digit[11];

int main(void)
{
    /* Enable global interrupts. */
    CyGlobalIntEnable;
    
    isr_Timer_StartEx(Custom_Timer_Count_ISR);
        
    Timer_ADC_Start();
    
    
    /* Start EZI2C Component */
    EZI2C_Start();
   
    AMux_Start();
    AMux_Select(0);
    ADC_DelSig_Start();
 
    
    
    /* Place your initialization/startup code here (e.g. MyInst_Start()) */
    // Set up who am i register
    slaveBuffer[0] = cinque << 1 | stop;
    slaveBuffer[1] = 1;
    slaveBuffer[2] = 0xBC;
    slaveBuffer[3]=0;
    slaveBuffer[4]=0;
    slaveBuffer[5]=0;
    slaveBuffer[6]=0;
    EZI2C_SetBuffer1(SLAVE_BUFFER_SIZE, 2 ,slaveBuffer); //2 is the first read only byte in array
    
    for(;;)
    {
        /* Place your application code here. */
    }
}

/* [] END OF FILE */
