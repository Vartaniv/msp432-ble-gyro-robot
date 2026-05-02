/* @author Vedi Vartani
 *
 */
#include <stdint.h>
#include <math.h>
#include <stdio.h>
#include "msp.h"

#include "inc/Clock.h"
#include "inc/CortexM.h"
#include "inc/GPIO.h"
#include "inc/EUSCI_A0_UART.h"
#include "inc/Motor.h"
#include "inc/BLE_UART.h"

#define MOTOR_DUTY_30_PERCENT 4500
#define CONTROLLER_PACKET_SIZE 5

void Handle_Controller_Command(char packet[])
{
    char start_byte  = packet[0];
    char packet_id   = packet[1];
    char button_code = packet[2];
    char button_mode = packet[3];

    printf("Start: %c, Type: %c, Button: %c, Mode: %c\n",
           start_byte,
           packet_id,
           button_code,
           button_mode);

    // Confirm that this is a valid Bluefruit button packet
    if ((start_byte != '!') || (packet_id != 'B'))
    {
        printf("Invalid controller packet\n");
        return;
    }

    // Released button means stop immediately
    if (button_mode == '0')
    {
        printf("Controller button released\n");
        Motor_Stop();
        return;
    }

    // Pressed button means perform movement
    if (button_mode == '1')
    {
        switch (button_code)
        {
            case '5':
                printf("Forward command\n");
                Motor_Forward(MOTOR_DUTY_30_PERCENT, MOTOR_DUTY_30_PERCENT);
                break;

            case '6':
                printf("Backward command\n");
                Motor_Backward(MOTOR_DUTY_30_PERCENT, MOTOR_DUTY_30_PERCENT);
                break;

            case '7':
                printf("Left turn command\n");
                Motor_Left(MOTOR_DUTY_30_PERCENT, MOTOR_DUTY_30_PERCENT);
                break;

            case '8':
                printf("Right turn command\n");
                Motor_Right(MOTOR_DUTY_30_PERCENT, MOTOR_DUTY_30_PERCENT);
                break;

            default:
                printf("Unrecognized button code\n");
                Motor_Stop();
                break;
        }
    }
}

void Send_Fixed_UART_Test_Bytes(void)
{
    uint8_t test_values[4] = {0xAA, 0x55, 0xFF, 0x00};

    for (int i = 0; i < 4; i++)
    {
        BLE_UART_OutChar(test_values[i]);
        Clock_Delay1ms(1000);
    }
}

uint8_t Read_Selected_Switch_Byte(void)
{
    uint8_t switch_value = Get_PMOD_SWT_Status() & 0x0F;

    if (switch_value == 0x01)
    {
        return 0xAA;
    }
    else if (switch_value == 0x02)
    {
        return 0x10;
    }
    else if (switch_value == 0x04)
    {
        return 0xF0;
    }
    else if (switch_value == 0x08)
    {
        return 0x0F;
    }

    return 0x00;
}

int main(void)
{
    static char rx_packet[CONTROLLER_PACKET_SIZE];
    int packet_index = 0;

    DisableInterrupts();
    WDT_A->CTL = WDT_A_CTL_PW | WDT_A_CTL_HOLD;

    Clock_Init48MHz();
    LED2_Init();
    EUSCI_A0_UART_Init_Printf();
    BLE_UART_Init();
    Motor_Init();

    EnableInterrupts();

    Clock_Delay1ms(1000);
    BLE_UART_Reset();

    printf("BLE controller task active\n");
    BLE_UART_OutString("Controller Ready\r\n");

    while (1)
    {
        char incoming_byte = BLE_UART_InChar();

        if (packet_index == 0)
        {
            if (incoming_byte == '!')
            {
                rx_packet[packet_index] = incoming_byte;
                packet_index++;
            }
        }
        else
        {
            rx_packet[packet_index] = incoming_byte;
            packet_index++;

            if (packet_index >= CONTROLLER_PACKET_SIZE)
            {
                printf("Received packet: ");

                for (int i = 0; i < CONTROLLER_PACKET_SIZE; i++)
                {
                    printf("%c", rx_packet[i]);
                }

                printf("\n");

                Handle_Controller_Command(rx_packet);
                packet_index = 0;
            }
        }
    }
}
