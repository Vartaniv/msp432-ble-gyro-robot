/* @author Vedi Vartani
 *
 * Tilt / Gesture Controlled Robot
 *
 * This program uses the Bluefruit Connect app Controller mode.
 * Instead of arrow buttons, it reads quaternion orientation packets
 * from the phone and drives the robot based on phone tilt.
 */

#include <stdint.h>
#include <math.h>
#include <stdio.h>
#include <string.h>
#include "msp.h"

#include "inc/Clock.h"
#include "inc/CortexM.h"
#include "inc/GPIO.h"
#include "inc/EUSCI_A0_UART.h"
#include "inc/Motor.h"
#include "inc/BLE_UART.h"

/*
 * Bluefruit Quaternion Packet Format:
 *
 * Byte 0: '!'
 * Byte 1: 'Q'
 * Byte 2-5:   qx float
 * Byte 6-9:   qy float
 * Byte 10-13: qz float
 * Byte 14-17: qw float
 * Byte 18: checksum
 */
#define QUATERNION_PACKET_SIZE 19

#define PWM_NOMINAL 4500
#define PWM_SLOW    3000

#define PWM_MIN     2500
#define PWM_MAX     14000

#define TILT_MIN    0.10f
#define TILT_MAX    0.65f

/*
 * Uncomment this if you want speed to change based on phone tilt.
 * Leave commented if you want fixed speed like the original example.
 */
#define USE_PROPORTIONAL_SPEED


uint16_t Limit_PWM(int pwm)
{
    if (pwm < PWM_MIN)
    {
        pwm = PWM_MIN;
    }

    if (pwm > PWM_MAX)
    {
        pwm = PWM_MAX;
    }

    return (uint16_t)pwm;
}


/*
 * This function calculates motor speed from phone tilt.
 *
 * More tilt = larger quaternion value = larger PWM duty cycle.
 */
uint16_t Calculate_PWM(float q_value)
{
    int pwm;

    pwm = (int)(fabsf(q_value) * 10000.0f);

    return Limit_PWM(pwm);
}


/*
 * This function waits until a full quaternion packet is received.
 *
 * It searches for the start byte '!' first.
 * Then it reads the remaining 18 bytes.
 */
void Read_Quaternion_Packet(uint8_t packet[])
{
    uint8_t incoming_byte;
    int index = 0;

    while (index < QUATERNION_PACKET_SIZE)
    {
        incoming_byte = BLE_UART_InChar();

        if (index == 0)
        {
            if (incoming_byte == '!')
            {
                packet[index] = incoming_byte;
                index++;
            }
        }
        else
        {
            packet[index] = incoming_byte;
            index++;
        }
    }
}


/*
 * Converts 4 raw bytes from the packet into a float.
 *
 * Bluefruit sends qx, qy, qz, and qw as 4-byte floating point values.
 */
float Get_Float_From_Packet(uint8_t packet[], int start_index)
{
    float value;

    memcpy(&value, &packet[start_index], 4);

    return value;
}


/*
 * This function decodes the quaternion packet and drives the robot.
 *
 * qx controls forward/backward tilt in vertical phone mode.
 * qy controls left/right tilt in vertical phone mode.
 *
 * The robot can move:
 * - Forward
 * - Backward
 * - Left
 * - Right
 * - Forward-right
 * - Forward-left
 * - Backward-right
 * - Backward-left
 * - Stop
 */
void Process_Quaternion_Controller(uint8_t packet[])
{
    float qx;
    float qy;
    float qz;
    float qw;

    uint16_t left_pwm;
    uint16_t right_pwm;

    /*
     * Check that this is a valid quaternion packet.
     * Quaternion packets begin with !Q.
     */
    if ((packet[0] != '!') || (packet[1] != 'Q'))
    {
        printf("Invalid quaternion packet\n");
        Motor_Stop();
        return;
    }

    /*
     * Extract quaternion float values from the packet.
     */
    qx = Get_Float_From_Packet(packet, 2);
    qy = Get_Float_From_Packet(packet, 6);
    qz = Get_Float_From_Packet(packet, 10);
    qw = Get_Float_From_Packet(packet, 14);

    printf("qx: %.2f  qy: %.2f  qz: %.2f  qw: %.2f\n", qx, qy, qz, qw);

#ifdef USE_PROPORTIONAL_SPEED

    /*
     * Proportional speed mode:
     * More tilt creates larger PWM values.
     */
    left_pwm = Calculate_PWM(qx);
    right_pwm = Calculate_PWM(qx);

#else

    /*
     * Fixed speed mode:
     * Same speed every time.
     */
    left_pwm = PWM_NOMINAL;
    right_pwm = PWM_NOMINAL;

#endif

    /*
     * Forward + Right
     *
     * Left motor faster than right motor.
     */
    if ((qx > TILT_MIN) && (qx < TILT_MAX) &&
        (qy > TILT_MIN) && (qy < TILT_MAX))
    {
#ifdef USE_PROPORTIONAL_SPEED
        left_pwm = Limit_PWM(Calculate_PWM(qx) + Calculate_PWM(qy));
        right_pwm = Limit_PWM(Calculate_PWM(qx));
#else
        left_pwm = PWM_NOMINAL;
        right_pwm = PWM_SLOW;
#endif

        printf("Forward Right\n");
        LED2_Output(RGB_LED_WHITE);
        Motor_Forward(left_pwm, right_pwm);
    }

    /*
     * Forward + Left
     *
     * Right motor faster than left motor.
     */
    else if ((qx > TILT_MIN) && (qx < TILT_MAX) &&
             (qy < -TILT_MIN) && (qy > -TILT_MAX))
    {
#ifdef USE_PROPORTIONAL_SPEED
        left_pwm = Limit_PWM(Calculate_PWM(qx));
        right_pwm = Limit_PWM(Calculate_PWM(qx) + Calculate_PWM(qy));
#else
        left_pwm = PWM_SLOW;
        right_pwm = PWM_NOMINAL;
#endif

        printf("Forward Left\n");
        LED2_Output(RGB_LED_WHITE);
        Motor_Forward(left_pwm, right_pwm);
    }

    /*
     * Backward + Right
     */
    else if ((qx < -TILT_MIN) && (qx > -TILT_MAX) &&
             (qy > TILT_MIN) && (qy < TILT_MAX))
    {
#ifdef USE_PROPORTIONAL_SPEED
        left_pwm = Limit_PWM(Calculate_PWM(qx) + Calculate_PWM(qy));
        right_pwm = Limit_PWM(Calculate_PWM(qx));
#else
        left_pwm = PWM_NOMINAL;
        right_pwm = PWM_SLOW;
#endif

        printf("Backward Right\n");
        LED2_Output(RGB_LED_SKY_BLUE);
        Motor_Backward(left_pwm, right_pwm);
    }

    /*
     * Backward + Left
     */
    else if ((qx < -TILT_MIN) && (qx > -TILT_MAX) &&
             (qy < -TILT_MIN) && (qy > -TILT_MAX))
    {
#ifdef USE_PROPORTIONAL_SPEED
        left_pwm = Limit_PWM(Calculate_PWM(qx));
        right_pwm = Limit_PWM(Calculate_PWM(qx) + Calculate_PWM(qy));
#else
        left_pwm = PWM_SLOW;
        right_pwm = PWM_NOMINAL;
#endif

        printf("Backward Left\n");
        LED2_Output(RGB_LED_SKY_BLUE);
        Motor_Backward(left_pwm, right_pwm);
    }

    /*
     * Forward only
     */
    else if ((qx > TILT_MIN) && (qx < TILT_MAX))
    {
#ifdef USE_PROPORTIONAL_SPEED
        left_pwm = Calculate_PWM(qx);
        right_pwm = Calculate_PWM(qx);
#endif

        printf("Forward\n");
        LED2_Output(RGB_LED_GREEN);
        Motor_Forward(left_pwm, right_pwm);
    }

    /*
     * Backward only
     */
    else if ((qx < -TILT_MIN) && (qx > -TILT_MAX))
    {
#ifdef USE_PROPORTIONAL_SPEED
        left_pwm = Calculate_PWM(qx);
        right_pwm = Calculate_PWM(qx);
#endif

        printf("Backward\n");
        LED2_Output(RGB_LED_BLUE);
        Motor_Backward(left_pwm, right_pwm);
    }

    /*
     * Rotate right in place
     */
    else if ((qy > TILT_MIN) && (qy < TILT_MAX))
    {
#ifdef USE_PROPORTIONAL_SPEED
        left_pwm = Calculate_PWM(qy);
        right_pwm = Calculate_PWM(qy);
#else
        left_pwm = 2300;
        right_pwm = 2300;
#endif

        printf("Right\n");
        LED2_Output(RGB_LED_PINK);
        Motor_Right(left_pwm, right_pwm);
    }

    /*
     * Rotate left in place
     */
    else if ((qy < -TILT_MIN) && (qy > -TILT_MAX))
    {
#ifdef USE_PROPORTIONAL_SPEED
        left_pwm = Calculate_PWM(qy);
        right_pwm = Calculate_PWM(qy);
#else
        left_pwm = 2300;
        right_pwm = 2300;
#endif

        printf("Left\n");
        LED2_Output(RGB_LED_YELLOW);
        Motor_Left(left_pwm, right_pwm);
    }

    /*
     * Neutral phone position.
     * Stop the robot.
     */
    else
    {
        printf("Stop\n");
        LED2_Output(RGB_LED_RED);
        Motor_Stop();
    }
}


int main(void)
{
    uint8_t quaternion_packet[QUATERNION_PACKET_SIZE];

    DisableInterrupts();

    WDT_A->CTL = WDT_A_CTL_PW | WDT_A_CTL_HOLD;

    Clock_Init48MHz();

    LED2_Init();

    EUSCI_A0_UART_Init_Printf();

    BLE_UART_Init();

    Motor_Init();
    Motor_Stop();

    EnableInterrupts();

    Clock_Delay1ms(1000);

    BLE_UART_Reset();

    printf("BLE quaternion tilt controller active\n");
    BLE_UART_OutString("Quaternion Controller Ready\r\n");

    while (1)
    {
        /*
         * Read one full !Q quaternion packet from the phone.
         */
        Read_Quaternion_Packet(quaternion_packet);

        /*
         * Decode the packet and move the robot.
         */
        Process_Quaternion_Controller(quaternion_packet);

        /*
         * Small delay so the robot does not react too aggressively.
         */
        Clock_Delay1ms(50);
    }
}
