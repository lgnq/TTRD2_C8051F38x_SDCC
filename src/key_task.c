/*----------------------------------------------------------------------------*-

  ttrd2-02a-t0401a-v001a_switch_task.c (Release 2017-02-22a)

  ----------------------------------------------------------------------------
   
  Simple switch interface library for Nucleo F401 board.

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

// Processor Header
#include "main.h"

// ------ Private constants --------------------------------------------------

// Allows NO or NC switch to be used (or other wiring variations)
#define SW_PRESSED (0)

// SW_THRES must be > 1 for correct debounce behaviour
#define SW_THRES (3)

// ------ Private variable definitions ----------------------------------------

// The current switch state (see Init function)
static uint32_t Switch_button1_pressed_g = BUTTON1_NOT_PRESSED;

/*----------------------------------------------------------------------------*-

  SWITCH_BUTTON1_Init()

  Initialisation function for simple switch-interface module.

  Works with Button 1 on Nucleo board.

  PARAMETERS:
     None.

  LONG-TERM DATA:
     None.
   
  MCU HARDWARE:
     GPIO pin (Switch pin)

  PRE-CONDITION CHECKS:
     None.

  POST-CONDITION CHECKS:
     None.

  ERROR DETECTION / ERROR HANDLING:
     None.

  RETURN VALUE:
     None.

-*----------------------------------------------------------------------------*/
void SWITCH_BUTTON1_Init(void)
{
    //todo : init the pins of key

    // Set the initial state  
    Switch_button1_pressed_g = BUTTON1_NOT_PRESSED;
}

/*----------------------------------------------------------------------------*-

  SWITCH_BUTTON1_Update()

  Switch-reading task. 
   
  PARAMETERS:
     None.

  LONG-TERM DATA:
     Duration_s (W)
   
  MCU HARDWARE:
     GPIO pin (Switch pin)

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
void SWITCH_BUTTON1_Update(void)
{
    // Duration of switch press
    static uint32_t Duration_s = 0;

    uint32_t Button1_input = 1;
        
    // Read the pin state
    ///Button1_input = GPIO_ReadInputDataBit(BUTTON1_PORT, BUTTON1_PIN);
   
    if (Button1_input == SW_PRESSED)
    {
        Duration_s += 1;

        if (Duration_s > SW_THRES)
        {
            Duration_s = SW_THRES;

            Switch_button1_pressed_g = BUTTON1_PRESSED;
        }
        else
        {
            // Switch pressed, but not yet for long enough
            Switch_button1_pressed_g = BUTTON1_NOT_PRESSED;
        }
    }
    else
    {
        // Switch not pressed - reset the count
        Duration_s = 0;

        // Update status
        Switch_button1_pressed_g = BUTTON1_NOT_PRESSED;
    }
}

/*----------------------------------------------------------------------------*-

  SWITCH_BUTTON1_Get_State()

  Reurns switch state.
   
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

  WCET:
     Not yet determined.

  BCET:
     Not yet determined.

  RETURN VALUE:
     Returns BUTTON1_PRESSED or BUTTON1_NOT_PRESSED.

-*----------------------------------------------------------------------------*/
uint32_t SWITCH_BUTTON1_Get_State(void)
{
    return Switch_button1_pressed_g;
}

/*----------------------------------------------------------------------------*-
  ------------------------------ END OF FILE ---------------------------------
-*----------------------------------------------------------------------------*/
