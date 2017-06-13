/*----------------------------------------------------------------------------*-

  ttrd2-03a-t0401a-v001a_heartbeat_sw_u_task.c (Release 2017-02-22a)

  ----------------------------------------------------------------------------

  Simple 'Heartbeat-switch-uart' Task Module for STM32F401RC.

  Targets Nucleo F401RC board.

-*----------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*-

  This code is copyright (c) 2014-2016 SafeTTy Systems Ltd.

  This code forms part of a Time-Triggered Reference Design (TTRD) that is
  documented in the following book:

   Pont, M.J. (2016)
   "The Engineering of Reliable Embedded Systems (Second Edition)",
   Published by SafeTTy Systems Ltd. ISBN: 978-0-9930355-3-1.

  Both the TTRDs and the above book ("ERES2") describe patented
  technology and are subject to copyright and other restrictions.

  This code may be used without charge: [i] by universities and colleges on
  courses for which a degree up to and including 'MSc' level (or equivalent)
  is awarded; [ii] for non-commercial projects carried out by individuals
  and hobbyists.

  Any and all other use of this code and / or the patented technology
  described in ERES2 requires purchase of a ReliabiliTTy Technology Licence:
  http://www.safetty.net/technology/reliabilitty-technology-licences

  Please contact SafeTTy Systems Ltd if you require clarification of these
  licensing arrangements: http://www.safetty.net/contact

  CorrelaTTor, DecomposiTTor, DuplicaTTor, MoniTTor, PredicTTor, ReliabiliTTy,
  SafeTTy, SafeTTy Systems, TriplicaTTor and WarranTTor are registered
  trademarks or trademarks of SafeTTy Systems Ltd in the UK & other countries.

-*----------------------------------------------------------------------------*/

#include "main.h"

uint8_t reset_heartbeat_flag = 0;

/*----------------------------------------------------------------------------*-

  HEARTBEAT_SW_U_Init()

  Prepare for HEARTBEAT_SW_Update() function - see below.

  PARAMETERS:
     None.

  LONG-TERM DATA:
     None.

  MCU HARDWARE:
     GPIO pin (Heartbeat pin).

  PRE-CONDITION CHECKS:
     None.

  POST-CONDITION CHECKS:
     None.

  ERROR DETECTION / ERROR HANDLING:
     None.

  RETURN VALUE:
     None.

-*----------------------------------------------------------------------------*/
void HEARTBEAT_SW_U_Init(void)
{
    P0MDOUT = 0x03;     // Enable LED as a push-pull output
    P0SKIP  = 0xCF;		// GPIO, GPIO, TX, RX, GPIO...

    XBR1 = 0xC0;        // disable weak pull-up, enable crossbar

    LED_G = 0;
    LED_R = 0;
}

/*----------------------------------------------------------------------------*-

  HEARTBEAT_SW_U_Update1()

  Changes LED state.

  For 'standard' heartbeat, must release once per second (soft deadline).

  For demo purposes, this version incorporates a switch interface
  plus some basic fault injection capabilities.

  PARAMETERS:
     None.

  LONG-TERM DATA:
     Heartbeat_state_s (W)
     Countdown_s (W)

  MCU HARDWARE:
     GPIO pin (Heartbeat pin).
     UART2.

  PRE-CONDITION CHECKS:
     None.

  POST-CONDITION CHECKS:
     None.

  ERROR DETECTION / ERROR HANDLING:
     None.

  WCET:
     Not yet determined.

  BCET:
     Not yet determined.

  RETURN VALUE:
     None.

-*----------------------------------------------------------------------------*/
void HEARTBEAT_SW_U_Update1(void)
{
    static uint32_t Heartbeat_state_s = 0;
    static uint32_t Countdown_s = 10;

    UART2_BUF_O_Write_String_To_Buffer("\nCountdown : ");
    UART2_BUF_O_Write_Number03_To_Buffer(Countdown_s);
    UART2_BUF_O_Write_String_To_Buffer("\n");

    if (Countdown_s-- == 0)
    {
        // Trigger task overrun (for demo purposes) ...
        while (1)
            ;
    }

    // Only flash the LED if the switch is *not* pressed
    //if (SWITCH_BUTTON1_Get_State() == BUTTON1_NOT_PRESSED)
    if (reset_heartbeat_flag == 0)
    {
        // Change the LED from OFF to ON (or vice versa)
        if (Heartbeat_state_s == 1)
        {
            Heartbeat_state_s = 0;
            //todo : turn LED on
            LED_G = 1;

            UART2_BUF_O_Write_String_To_Buffer("LED On ...\n");
        }
        else
        {
            Heartbeat_state_s = 1;
            //todo : turn LED off
            LED_G = 0;

            UART2_BUF_O_Write_String_To_Buffer("LED Off ...\n");
        }
    }
    else
    {
        UART2_BUF_O_Write_String_To_Buffer("SWITCH PRESSED ...\n");
        Countdown_s = 10;
        reset_heartbeat_flag = 0;
    }
}

/*----------------------------------------------------------------------------*-

  HEARTBEAT_SW_U_Update2()

  Changes LED state.

  For 'standard' heartbeat, must release once per second (soft deadline).

  Basic 'heartbeat' code: shares pin with 'Update1';

  PARAMETERS:
     None.

  LONG-TERM DATA:
     Heartbeat_state_s (W)

  MCU HARDWARE:
     GPIO pin (Heartbeat pin).

  PRE-CONDITION CHECKS:
     None.

  POST-CONDITION CHECKS:
     None.

  ERROR DETECTION / ERROR HANDLING:
     None.

  WCET:
     Not yet determined.

  BCET:
     Not yet determined.

  RETURN VALUE:
     None.

-*----------------------------------------------------------------------------*/
void HEARTBEAT_SW_U_Update2(void)
{
    static uint32_t Heartbeat_state_s = 0;

    // Change the LED from OFF to ON (or vice versa)
    if (Heartbeat_state_s == 1)
    {
        Heartbeat_state_s = 0;

        //todo : turn LED on
        LED_R = 1;
    }
    else
    {
        Heartbeat_state_s = 1;

        //todo : turn LED off
        LED_R = 0;
    }
}

/*----------------------------------------------------------------------------*-
  ------------------------------ END OF FILE ---------------------------------
-*----------------------------------------------------------------------------*/
