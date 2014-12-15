#include "system.h"

#include <stdint.h>
#include <string.h>
#include <stddef.h>

#include "usb/usb.h"
#include "app_device_cdc_basic.h"
#include "usb_config.h"

static uint8_t readBuffer[CDC_DATA_OUT_EP_SIZE];
static uint8_t writeBuffer[CDC_DATA_IN_EP_SIZE];


void APP_DeviceCDCBasicDemoInitialize()
{
    CDCInitEP();

    line_coding.bCharFormat = 0;
    line_coding.bDataBits = 8;
    line_coding.bParityType = 0;
    line_coding.dwDTERate = 9600;
}

void APP_DeviceCDCBasicDemoTasks()
{
    /* If the user has pressed the button associated with this demo, then we
     * are going to send a "Button Pressed" message to the terminal.
     */
//    if(BUTTON_IsPressed(BUTTON_DEVICE_CDC_BASIC_DEMO) == true)
//    {
//        /* Make sure that we only send the message once per button press and
//         * not continuously as the button is held.
//         */
//        if(buttonPressed == false)
//        {
//            /* Make sure that the CDC driver is ready for a transmission.
//             */
//            if(mUSBUSARTIsTxTrfReady() == true)
//            {
//                putrsUSBUSART(buttonMessage);
//                buttonPressed = true;
//            }
//        }
//    }
//    else
//    {
//        /* If the button is released, we can then allow a new message to be
//         * sent the next time the button is pressed.
//         */
//        buttonPressed = false;
//    }

    /* Check to see if there is a transmission in progress, if there isn't, then
     * we can see about performing an echo response to data received.
     */
    if( USBUSARTIsTxTrfReady() == true)
    {
        uint8_t i;
        uint8_t numBytesRead;

        numBytesRead = getsUSBUSART(readBuffer, sizeof(readBuffer));

        /* For every byte that was read... */
        for(i=0; i<numBytesRead; i++)
        {
            switch(readBuffer[i])
            {
                /* If we receive new line or line feed commands, just echo
                 * them direct.
                 */
                case 0x0A:
                case 0x0D:
                    writeBuffer[i] = readBuffer[i];
                    break;

                /* If we receive something else, then echo it plus one
                 * so that if we receive 'a', we echo 'b' so that the
                 * user knows that it isn't the echo enabled on their
                 * terminal program.
                 */
                default:
                    writeBuffer[i] = readBuffer[i] + 1;
                    break;
            }
        }

        if(numBytesRead > 0)
        {
            /* After processing all of the received data, we need to send out
             * the "echo" data now.
             */
            putUSBUSART(writeBuffer,numBytesRead);
        }
    }

    CDCTxService();
}