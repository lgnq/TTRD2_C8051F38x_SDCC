/*----------------------------------------------------------------------------*-

  ttrd2-02a-t0401a-v001a_iwdt_task.c (Release 2017-02-22a)

  ----------------------------------------------------------------------------

  'Watchdog' timer library (internal) for STM32F401RC.

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

/*----------------------------------------------------------------------------*-

  WATCHDOG_Init()

  Set up independent watchdog timer.

  The watchdog timer is driven by the Internal LSI (RC) Oscillator:
  the timing varies (significantly) with temperature.

  Datasheet values:
  RC oscillator frequency (Minimum) : 17 kHz
  RC oscillator frequency (Typical) : 32 kHz
  RC oscillator frequency (Maximum) : 47 kHz

  It is suggested that you use twice the required timeout value (approx).

  PARAMETERS:
     WDT_COUNT : Will be multiplied by ~100¦Ìs to determine the timeout.

  LONG-TERM DATA:
     None.

  MCU HARDWARE:
     iWDT hardware.

  PRE-CONDITION CHECKS:
     None.

  POST-CONDITION CHECKS:
     None.

  ERROR DETECTION / ERROR HANDLING:
     None.

  RETURN VALUE:
     None.

-*----------------------------------------------------------------------------*/
void WATCHDOG_Init(const uint32_t WDT_COUNT)
{
    /*
    To configure the WDT, perform the following tasks:
    1. Disable the WDT by writing a 0 to the WDTE bit.
    2. Select the desired PCA clock source (with the CPS2–CPS0 bits). default is SYSCLK/12
    3. Load PCA0CPL4 with the desired WDT update offset value.
    4. Configure the PCA Idle mode (set CIDL if the WDT should be suspended while the CPU is in Idle mode).
    5. Enable the WDT by setting the WDTE bit to 1.
    6. Reset the WDT timer by writing to PCA0CPH4.
    */

    PCA0CPL4 = 0xFF;

    PCA0MD |= 0x40;						// Enable Watchdog timer, WDTE = 1

    // Feed watchdog
    WATCHDOG_Update();
}

/*----------------------------------------------------------------------------*-

  WATCHDOG_Update()

  Feed the watchdog timer.

  See Watchdog_Init() for further information.

  PARAMETERS:
     None.

  LONG-TERM DATA:
     None.

  MCU HARDWARE:
     iWDT hardware.

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
void WATCHDOG_Update(void)
{
    // Feed the watchdog (reload IWDG counter)
    PCA0CPH4 = 0x0;
}

uint8_t reset_by_watchdog(void)
{
    if (RSTSRC == 0x08)
    {
        return 1;
    }
    else
    {
        return 0;
    }
}

/*----------------------------------------------------------------------------*-
  ------------------------------ END OF FILE ---------------------------------
-*----------------------------------------------------------------------------*/
