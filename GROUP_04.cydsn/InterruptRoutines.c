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
    

CY_ISR(Custom_Timer_Count_ISR)
{
    // Read timer status register to pull interrupt line low
    Timer_ADC_ReadStatusRegister();
    // Increment counter in slave buffer
    slaveBuffer[1]++;
}

void EZI2C_ISR_ExitCallback(void)
{
if ((slaveBuffer[0] & 0b11)==0b11){
 Pin_LED_Write(1);
}
   
}
/* [] END OF FILE */
