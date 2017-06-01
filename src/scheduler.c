/*----------------------------------------------------------------------------*-

  ttrd2-02a-t0401a-v001a_scheduler.c (Release 2017-02-22a)

  ----------------------------------------------------------------------------

  Time-Triggered Co-operative (TTC) task scheduler for STM32F401.

  See "ERES2" (Chapter 2) for further information about this scheduler.

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

// ------ Private variable definitions ---------------------------------------

// The array of tasks
// Check array size in scheduler header file
static sTask_t SCH_tasks_g[SCH_MAX_TASKS];

// Tick count
static uint32_t Tick_count_g = 0;

/*----------------------------------------------------------------------------*-

  SCH_Init_Milliseconds()

  Scheduler initialisation function.  Prepares scheduler data structures and 
   sets up timer interrupts every TICKms milliseconds.

  You must call this function before using the scheduler.

  [Required_SystemCoreClock frequency can be found in main.h.]

  PARAMETERS:
     TICKms is the required Tick Interval (milliseconds)

  LONG-TERM DATA:
     SCH_tasks_g (W)

  MCU HARDWARE:
     SysTick timer.

  PRE-CONDITION CHECKS:
     None.

  POST-CONDITION CHECKS:
     None.

  ERROR DETECTION / ERROR HANDLING:
     PROCESSOR_Perform_Safe_Shutdown() is called if the requested timing 
     requirements cannot be met.

  RETURN VALUE:
     None.

-*----------------------------------------------------------------------------*/
void SCH_Init_Milliseconds(const uint32_t TICKms)
{
    uint32_t Task_id;

    for (Task_id = 0; Task_id < SCH_MAX_TASKS; Task_id++)
    {
        // Set pTask to "null pointer"
        SCH_tasks_g[Task_id].pTask = SCH_NULL_PTR;
    }

#if 0    
    // Using CMSIS
      
    // SystemCoreClock gives the system operating frequency (in Hz)
    if (SystemCoreClock != REQUIRED_PROCESSOR_CORE_CLOCK)
    {
        // Fatal error
        PROCESSOR_Perform_Safe_Shutdown();
    }

   // Now to set up SysTick timer for "ticks" at interval TICKms
   if (SysTick_Config(TICKms * SystemCoreClock / 1000))
      {
      // Fatal error
      PROCESSOR_Perform_Safe_Shutdown();
      }

   // Timer is started by SysTick_Config():
   // we need to disable SysTick timer and SysTick interrupt until
   // all tasks have been added to the schedule.
   SysTick->CTRL &= 0xFFFFFFFC;
#else
//    system_clock_init(16000000);
   
    timer_init(1);
#endif   
}

/*----------------------------------------------------------------------------*-

  SCH_Start()

  Starts the scheduler, by enabling SysTick interrupt.

  NOTES: 
  * All tasks must be added before starting scheduler.
  * Any other interrupts MUST be synchronised to this tick.  

  PARAMETERS:
     None.

  LONG-TERM DATA:
     None.

  MCU HARDWARE:
     SysTick timer.

  PRE-CONDITION CHECKS:
     None.

  POST-CONDITION CHECKS:
     None.

  ERROR DETECTION / ERROR HANDLING:
     None.

  RETURN VALUE:
     None.

-*----------------------------------------------------------------------------*/
void SCH_Start(void) 
{
#if 0  
    // Enable SysTick timer
    SysTick->CTRL |= 0x01;

    // Enable SysTick interrupt
    SysTick->CTRL |= 0x02;
#else
    timer_interrupt_enable();
    timer_start(); 
    
//    int_enable();
#endif   
}

/*----------------------------------------------------------------------------*-

  tick_handler()

  [Function name determined by CMIS standard.]

  This is the scheduler ISR.  It is called at a rate 
  determined by the timer settings in the SCH_Init() function.

  PARAMETERS:
     None.

  LONG-TERM DATA:
     Tick_count_g (W)

  MCU HARDWARE:
     SysTick timer.

  PRE-CONDITION CHECKS:
     None.

  POST-CONDITION CHECKS:
     None.

  ERROR DETECTION / ERROR HANDLING:
     Checks Tick_count_g value => 'Fail Safe'
     [See ERES2, Ch2 for details.]

  RETURN VALUE:
     None.

-*----------------------------------------------------------------------------*/
void tick_handler(void)
{
    // Increment tick count and check against limit
    if (++Tick_count_g > SCH_TICK_COUNT_LIMIT)
    {
        // One or more tasks has taken too long to complete
        PROCESSOR_Perform_Safe_Shutdown();
    }
}

/*----------------------------------------------------------------------------*-

  SCH_Dispatch_Tasks()

  This is the 'dispatcher' function.  When a task (function)
  is due to run, SCH_Dispatch_Tasks() will run it.
  This function must be called (repeatedly) from the main loop.

  Triggers move to "idle" mode when all tasks have been released.

  PARAMETERS:
     None.

  LONG-TERM DATA:
     SCH_tasks_g (W)
     Tick_count_g (W)

  MCU HARDWARE:
     Triggers move to idle mode.

  PRE-CONDITION CHECKS:
     None.

  POST-CONDITION CHECKS:
     None.

  ERROR DETECTION / ERROR HANDLING:
     None.

  RETURN VALUE:
     None.

-*----------------------------------------------------------------------------*/
void SCH_Dispatch_Tasks(void) 
{
    uint32_t Task_id;
    uint32_t Update_required;

    int_disable();  
    Update_required = (Tick_count_g > 0);  // Check tick count
    int_enable();

    while (Update_required)
    {
        // Go through the task array
        for (Task_id = 0; Task_id < SCH_MAX_TASKS; Task_id++)
        {
            // Check if there is a task at this location
            if (SCH_tasks_g[Task_id].pTask != SCH_NULL_PTR)
            {
                if (--SCH_tasks_g[Task_id].Delay == 0)
                {
                    (*SCH_tasks_g[Task_id].pTask)();  // Run the task

                    // All tasks are periodic: schedule task to run again
                    SCH_tasks_g[Task_id].Delay = SCH_tasks_g[Task_id].Period;
                }
            }         
        } 

        int_disable();
        Tick_count_g--;                       // Decrement the count
        Update_required = (Tick_count_g > 0); // Check again
        int_enable();
    }

    // The scheduler enters idle mode at this point 
    sleep();
}

/*----------------------------------------------------------------------------*-

  SCH_Add_Task()

  Adds a task (function) to the schedule.
  The task will be released periodically by the scheduler.

  PARAMETERS:
     pTask  : The name of the task (function) to be scheduled.
              NOTE: All scheduled functions must be 'void, void' -
              that is, they must take no parameters, and have 
              a void return type (in this design).
                   
     DELAY  : The interval (ticks) before the task is first executed.

     PERIOD : Task period (in ticks).  Must be > 0.

  LONG-TERM DATA:
     SCH_tasks_g (W)
  
  MCU HARDWARE:
     None.

  PRE-CONDITION CHECKS:
     1. There is space in the task array.
     2. The task is periodic ('one-shot' tasks are not supported. 

  POST-CONDITION CHECKS:
     None.

  ERROR DETECTION / ERROR HANDLING:
     PROCESSOR_Perform_Safe_Shutdown() is called:
     - if the task cannot be added to the schedule (array too small)
     - if an attempt is made to schedule a "one shot" task

  RETURN VALUE:  
     None.
 
-*----------------------------------------------------------------------------*/
void SCH_Add_Task(void (* pTask)(), const uint32_t DELAY, const uint32_t PERIOD)
{
    uint32_t Task_id = 0;
   
    // First find a gap in the array (if there is one)
    while ((SCH_tasks_g[Task_id].pTask != SCH_NULL_PTR) && (Task_id < SCH_MAX_TASKS))
    {
        Task_id++;
    } 
   
    // Have we reached the end of the list?   
    if (Task_id == SCH_MAX_TASKS)
    {
        // Task list is full - fatal error
        PROCESSOR_Perform_Safe_Shutdown();
    }
      
    // Check for "one shot" tasks
    if (PERIOD == 0)
    {
        // We do not allow "one shot" tasks (all tasks must be periodic)
        PROCESSOR_Perform_Safe_Shutdown();
    }

    // If we're here, there is a space in the task array
    // and the task to be added is periodic
    SCH_tasks_g[Task_id].pTask  = pTask;

    SCH_tasks_g[Task_id].Delay  = DELAY + 1;
    SCH_tasks_g[Task_id].Period = PERIOD;
}

/*----------------------------------------------------------------------------*-
  ------------------------------ END OF FILE ---------------------------------
-*----------------------------------------------------------------------------*/
