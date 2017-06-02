/*----------------------------------------------------------------------------*-

  ttrd2-03a-t0401a-v001a_processor.c (Release 2017-02-22a)

  Configures Processor in one of the following Modes / States after a reset:

    FAIL_SAFE_S
    NORMAL_M

-*----------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*-

  This code is copyright (c) 2014-2017 SafeTTy Systems Ltd.

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

// Processor Header
#include "main.h"

// ------ Private variable definitions ---------------------------------------

// The current Processor Mode / State
static eProc_MoSt Processor_MoSt_g;

// ------ Private function declarations --------------------------------------

void PROCESSOR_Identify_Reqd_MoSt(void);
void PROCESSOR_Configure_Reqd_MoSt(void);

/*----------------------------------------------------------------------------*-

  PROCESSOR_Init()

  Wrapper for system startup functions.

  PARAMETERS:
     None.

  LONG-TERM DATA:
     None.

  MCU HARDWARE:
     None.

  PRE-CONDITION CHECKS:
     None.

  POST-CONDITION CHECKS:
     None.

  ERROR DETECTION / ERROR HANDLING:
     None.

  RETURN VALUE:
     None.

-*----------------------------------------------------------------------------*/
void PROCESSOR_Init(void)
{
    PROCESSOR_Identify_Reqd_MoSt();
    PROCESSOR_Configure_Reqd_MoSt();
}

/*----------------------------------------------------------------------------*-

  PROCESSOR_Identify_Reqd_MoSt()

  Identify the cause of the system reset.
  Set the system mode / state accordingly.

  PARAMETERS:
     None.

  LONG-TERM DATA:
     Processor_MoSt_g (W)

  MCU HARDWARE:
     Registers indicating cause of reset (R).

  PRE-CONDITION CHECKS:
     None.

  POST-CONDITION CHECKS:
     None.

  ERROR DETECTION / ERROR HANDLING:
     Check for reset caused by iWDT => 'fail safe'

  RETURN VALUE:
     None.

-*----------------------------------------------------------------------------*/
void PROCESSOR_Identify_Reqd_MoSt(void)
{
    // Check cause of reset
    if (reset_by_watchdog())
    {
        // Reset was caused by WDT => "Fail Safe" state
        Processor_MoSt_g = FAIL_SAFE_S;
    }
    else
    {
        // Here we treat all other forms of reset in the same way
        // => "Normal" mode
        Processor_MoSt_g = NORMAL_M;
    }
}

/*----------------------------------------------------------------------------*-

  PROCESSOR_Configure_Reqd_MoSt()

  Configure the system in the required mode.

  PARAMETERS:
     None.

  LONG-TERM DATA:
     System_node_G (R)

  MCU HARDWARE:
     None.

  PRE-CONDITION CHECKS:
     None.

  POST-CONDITION CHECKS:
     None.

  ERROR DETECTION / ERROR HANDLING:
     None.

  RETURN VALUE:
     None.

-*----------------------------------------------------------------------------*/
void PROCESSOR_Configure_Reqd_MoSt(void)
{
    switch (Processor_MoSt_g)
    {
        // Default to "Fail Safe" state
        default:
        case FAIL_SAFE_S:
        {
            // Configure UART1 (set baud rate)
            UART2_BUF_O_Init(115200);
            UART2_BUF_O_Write_String_To_Buffer("\nFail-Safe State\n");
            UART2_BUF_O_Send_All_Data();

            // Reset caused by iWDT
            // Trigger "fail safe" behaviour
            PROCESSOR_Perform_Safe_Shutdown();

            break;
        }

        case NORMAL_M:
        {
            mcu_init();

            // Set up the scheduler for 1 ms ticks
            SCH_Init_Milliseconds(1);

            tasks_init();

            // Feed the watchdog
            WATCHDOG_Update();

            break;
        }
    }
}

/*----------------------------------------------------------------------------*-

  PROCESSOR_Perform_Safe_Shutdown()

  Attempt to move the system into a safe ('silent') state.

  PARAMETERS:
     None.

  LONG-TERM DATA:
     None.

  MCU HARDWARE:
     Heartbeat LED (GPIO pin).

  PRE-CONDITION CHECKS:
     None.

  POST-CONDITION CHECKS:
     None.

  ERROR DETECTION / ERROR HANDLING:
     None.

  RETURN VALUE:
     None.

-*----------------------------------------------------------------------------*/
void PROCESSOR_Perform_Safe_Shutdown(void)
{
    uint32_t delay;

    // Here we simply "fail silent" with rudimentary fault reporting.
    // OTHER BEHAVIOUR IS LIKELY TO BE REQUIRED IN YOUR DESIGN

    // *************************************
    // NOTE: This function should NOT return
    // *************************************

    // Set up Heartbeat LED pin
    HEARTBEAT_SW_U_Init();

    while (1)
    {
        // Flicker Heartbeat LED to indicate fault
        for (delay = 0; delay < 10000; delay++)
        {
            ;
        }

        HEARTBEAT_SW_U_Update2();
    }
}

/*----------------------------------------------------------------------------*-
  ------------------------------ END OF FILE ---------------------------------
-*----------------------------------------------------------------------------*/

