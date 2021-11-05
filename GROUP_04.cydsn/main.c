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
uint8_t slaveBuffer[SLAVE_BUFFER_SIZE];

int main(void)
{
    CyGlobalIntEnable; /* Enable global interrupts. */

    /* Start EZI2C Component */
    EZI2C_Start();
    
    /* Start timer and associated ISR */
    Timer_ADC_Start();
    isr_Timer_StartEx(Custom_Timer_Count_ISR);
    /* Place your initialization/startup code here (e.g. MyInst_Start()) */
    // Set up who am i register
    slaveBuffer[2] = 0xBC;
    
    EZI2C_SetBuffer1(SLAVE_BUFFER_SIZE, 2 ,slaveBuffer); //2 is the first read only byte in array
    
    for(;;)
    {
        /* Place your application code here. */
    }
}

/* [] END OF FILE */
