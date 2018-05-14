/*******************************************************************************
  MPLAB Harmony Application Source File
  
  Company:
    Microchip Technology Inc.
  
  File Name:
    app.c

  Summary:
    This file contains the source code for the MPLAB Harmony application.

  Description:
    This file contains the source code for the MPLAB Harmony application.  It 
    implements the logic of the application's state machine and it may call 
    API routines of other MPLAB Harmony modules in the system, such as drivers,
    system services, and middleware.  However, it does not call any of the
    system interfaces (such as the "Initialize" and "Tasks" functions) of any of
    the modules in the system or make any assumptions about when those functions
    are called.  That is the responsibility of the configuration-specific system
    files.
 *******************************************************************************/

// DOM-IGNORE-BEGIN
/*******************************************************************************
Copyright (c) 2013-2014 released Microchip Technology Inc.  All rights reserved.

Microchip licenses to you the right to use, modify, copy and distribute
Software only when embedded on a Microchip microcontroller or digital signal
controller that is integrated into your product or third party product
(pursuant to the sublicense terms in the accompanying license agreement).

You should refer to the license agreement accompanying this Software for
additional information regarding your rights and obligations.

SOFTWARE AND DOCUMENTATION ARE PROVIDED "AS IS" WITHOUT WARRANTY OF ANY KIND,
EITHER EXPRESS OR IMPLIED, INCLUDING WITHOUT LIMITATION, ANY WARRANTY OF
MERCHANTABILITY, TITLE, NON-INFRINGEMENT AND FITNESS FOR A PARTICULAR PURPOSE.
IN NO EVENT SHALL MICROCHIP OR ITS LICENSORS BE LIABLE OR OBLIGATED UNDER
CONTRACT, NEGLIGENCE, STRICT LIABILITY, CONTRIBUTION, BREACH OF WARRANTY, OR
OTHER LEGAL EQUITABLE THEORY ANY DIRECT OR INDIRECT DAMAGES OR EXPENSES
INCLUDING BUT NOT LIMITED TO ANY INCIDENTAL, SPECIAL, INDIRECT, PUNITIVE OR
CONSEQUENTIAL DAMAGES, LOST PROFITS OR LOST DATA, COST OF PROCUREMENT OF
SUBSTITUTE GOODS, TECHNOLOGY, SERVICES, OR ANY CLAIMS BY THIRD PARTIES
(INCLUDING BUT NOT LIMITED TO ANY DEFENSE THEREOF), OR OTHER SIMILAR COSTS.
 *******************************************************************************/
// DOM-IGNORE-END


// *****************************************************************************
// *****************************************************************************
// Section: Included Files 
// *****************************************************************************
// *****************************************************************************

#include "app.h"
#include "ST7735.h"             // TFTLCD functions
#include "i2c_master_noint.h"   // I2C function (on I2C2)
#include <stdio.h>              // C standard i/o functions



// *****************************************************************************
// *****************************************************************************
// Section: Global Data Definitions
// *****************************************************************************
// *****************************************************************************

// *****************************************************************************
/* Application Data

  Summary:
    Holds application data

  Description:
    This structure holds the application's data.

  Remarks:
    This structure should be initialized by the APP_Initialize function.
    
    Application strings and buffers are be defined outside this structure.
*/

APP_DATA appData;

// *****************************************************************************
// *****************************************************************************
// Section: Application Callback Functions
// *****************************************************************************
// *****************************************************************************

/* TODO:  Add any necessary callback functions.
*/

// *****************************************************************************
// *****************************************************************************
// Section: Application Local Functions
// *****************************************************************************
// *****************************************************************************


/* TODO:  Add any necessary local functions.
*/


// *****************************************************************************
// *****************************************************************************
// Section: Application Initialization and State Machine Functions
// *****************************************************************************
// *****************************************************************************

/*******************************************************************************
  Function:
    void APP_Initialize ( void )

  Remarks:
    See prototype in app.h.
 */

void APP_Initialize ( void )
{
    /* Place the App state machine in its initial state. */
    appData.state = APP_STATE_INIT;
    
    
    __builtin_disable_interrupts();
    TRISAbits.TRISA4 = 0;   // A4 as output (i2c ack check LED)
    TRISBbits.TRISB8 = 0;   // B8 as output (whoami LED)
    TRISBbits.TRISB4 = 1;   // B4 as input  (USER button)
    // init I2C2, used as master
    i2c_master_setup();
    
    // init TFTLCD
    LCD_init();
    
    // init IMU
    initIMU();
    __builtin_enable_interrupts();
    
    
    /* TODO: Initialize your application's state machine and other
     * parameters.
     */
}


/******************************************************************************
  Function:
    void APP_Tasks ( void )

  Remarks:
    See prototype in app.h.
 */

void APP_Tasks ( void )
{

    /* Check the application's current state. */
    switch ( appData.state )
    {
        /* Application's initial state. */
        case APP_STATE_INIT:
        {
            bool appInitialized = true;
       
        
            if (appInitialized)
            {
            
                appData.state = APP_STATE_SERVICE_TASKS;
            }
            break;
        }

        case APP_STATE_SERVICE_TASKS:
        {
            // HW1 Blink Code -->
//             _CP0_SET_COUNT(0);
//            while(1) {
//                //Turn off LED while user button is pressed
//                while(!PORTBbits.RB4) {
//                    LATACLR = 0x10;
//                }
//                //Invert LED every 1ms (1kHz)
//                if(_CP0_GET_COUNT() > 24000){
//                    LATAINV = 0x10;
//                    _CP0_SET_COUNT(0);
//                }
//            }
            LATAbits.LATA4 = 1;     // Turn on ack check LED
            LATBbits.LATB8 = 0;     // Turn off whoami blink
            char whoami_check = 0;
            char message[30];
            unsigned char data_store[14];
            int i = 0, j = 0, fillX = 0, fillY = 0;
            signed short data_conv[7];
   
            clearLCD();     // Color every pixel on LCD black

            while(1){
                _CP0_SET_COUNT(0);      // set clock to 0 for loop timing
                I2C_read_multiple(SLAVE_ADDR, 0x20, data_store, 14);    // sequentially read data from the IMU
       
                // convert read data to 16 bit signed shorts
                for(i = 0;i<7;i++){
                    data_conv[i] = (data_store[i*2+1] << 8) | data_store[i*2];
                }
            
                fillX = (int) (-(data_conv[4]*0.00006104)*50);  // calculate x bar fill
                fillY = (int) ((data_conv[6]*0.00006104)*50);   // calculate y bar fill
        
                drawCross(64,60,2,fillX,fillY,50,RED,CYAN);     // draw the accelerometer bars
        
                whoami_check = getReg(0x0F);               // check whoami regester to ensure communcation is working
        
                // blinks LED as long as the IMU is still communicating
                if(whoami_check == 0x69){
                   LATBINV = 0x100;
                }
                while(_CP0_GET_COUNT() < 24000000/20){;}        // wait a 20th of a second
            }
            
            break;
        }

        /* TODO: implement your application state machine.*/
        

        /* The default state should never be executed. */
        default:
        {
            /* TODO: Handle error in application's state machine. */
            break;
        }
    }
}

 

/*******************************************************************************
 End of File
 */
