/*----------------------------------------------------------------------------*-
  
   ttrd2-02a-t0401a-v001a_scheduler.h (Release 2017-02-22a)

  ----------------------------------------------------------------------------

   See ttrd2-02a-t0401a-v001a_scheduler.c for details.

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

#ifndef __SCHEDULER_H__
#define __SCHEDULER_H__

#include "main.h"

// ------ Public data type declarations ----------------------------

// User-defined type to store required data for each task
typedef struct 
{
    // Pointer to the task (must be a 'void (void)' function)
    void (*pTask) (void);  

    // Delay (ticks) until the task will (next) be run
    // - see SCH_Add_Task() for further details
    uint32_t Delay;       

    // Interval (ticks) between subsequent runs.
    // - see SCH_Add_Task() for further details
    uint32_t Period;
} sTask_t; 

// ------ Public function prototypes -----------------------------------------

void SCH_Init_Milliseconds(const uint32_t TICKms);    
void SCH_Start(void);
void SCH_Dispatch_Tasks(void);

void SCH_Add_Task(void (* pTask)(),
                  const uint32_t DELAY,   // Offset (Ticks)
                  const uint32_t PERIOD); // Period (Ticks)

// ------ Public constants -------------------------------------------------------------

// The maximum number of tasks required at any one time
// during the execution of the program
//
// MUST BE CHECKED FOR EACH PROJECT (*not* dynamic)
#define SCH_MAX_TASKS (20)

// Usually set to 1, unless 'Long Tasks' are employed
#define SCH_TICK_COUNT_LIMIT (1)

// Default value for pTask (no task at this location)
#define SCH_NULL_PTR ((void (*) (void)) 0)
 
#endif

/*----------------------------------------------------------------------------*-
  ------------------------------ END OF FILE ---------------------------------
-*----------------------------------------------------------------------------*/
