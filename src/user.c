#include "main.h"

extern void tick_handler(void);

INTERRUPT(SCH_UPDATE, INTERRUPT_TIMER2)
{
    //clear flag
    TF2H = 0; // Have to manually clear this flag

    tick_handler();
}

void delay(void)
{
    int x;

    for (x = 0; x < 500; x)
        x++;
}

void system_clock_init(uint32_t freq)
{
    OSCICN |= 0x03;                     // Configure internal oscillator for
                                        // its maximum frequency and enable
                                        // missing clock detector

    CLKMUL  = 0x00;                     // Select internal oscillator as
                                        // input to clock multiplier

    CLKMUL |= 0x80;                     // Enable clock multiplier
    delay();                            // Delay for clock multiplier to begin
    CLKMUL |= 0xC0;                     // Initialize the clock multiplier
    delay();                            // Delay for clock multiplier to begin

    while (!(CLKMUL & 0x20));           // Wait for multiplier to lock

    switch (freq/1000000)
    {
    case 48:
        CLKSEL = 0x03; // Select system clock from internal High-Frequency Osc = 48MHz
        break;
    case 24:
        CLKSEL = 0x02; // Select system clock from internal High-Frequency Osc = 24MHz
        break;
    default:
        CLKSEL = 0x03; // Select system clock from internal High-Frequency Osc = 48MHz
        break;
    }
}

void timer_init(uint32_t tick)
{
    // Now set up Timer 2
    // [Based on SciLab example code]
    // Configure Timer2 to 16-bit auto-reload and generate an interrupt at
    // interval specified by <counts> using SYSCLK/48 as its time base.
    TMR2CN = 0x00;                // Stop Timer2; Clear TF2;
                                  // use SYSCLK/12 as timebase
    CKCON &= ~0x60;               // Timer2 clocked based on T2XCLK;

    // Init reload values
    TMR2RL = -(SYSCLK / 12 / 1000 / tick);
    TMR2   = 0xffff;                // Set to reload immediately

    // Not yet started timer
}

void timer_start(void)
{
    TF2H = 0;    // Clear Timer2 flag (just in case)
    TR2 = 1;
}

void timer_interrupt_enable(void)
{
    ET2 = 1;
}

void int_enable(void)
{
    EA = 1;
}

void int_disable(void)
{
    EA = 0;
}

void sleep(void)
{
    PCON |= 0x01;
    PCON  = PCON;
}

void mcu_init(void)
{
    //init system clock
    system_clock_init(SYSCLK);

    int_enable();
}

void tasks_init(void)
{
    // Set up WDT
    // Timeout is WDT count value: approx 32 per millisecond
    // => a count of 64 gives a timeout of approx 2 ms
    // NOTE: WDT driven by RC oscillator - timing varies with temperature
    WATCHDOG_Init(0xFF);

    // Prepare for switch task
    key_init();

    // Prepare for heartbeat-switch task
    HEARTBEAT_SW_U_Init();

    // Prepare for UART1 task (set baud rate)
    UART2_BUF_O_Init(BAUDRATE);

    // Report mode (via buffer)
    UART2_BUF_O_Write_String_To_Buffer("\nNormal mode\n");

    // Add tasks to schedule.
    // Parameters are:
    // A. Task name
    // B. Initial delay / offset (in Ticks)
    // C. Task period (in Ticks): Must be > 0
    //           A                       B  C
    SCH_Add_Task(WATCHDOG_Update,        0, 1);     // Feed iWDT
    SCH_Add_Task(key_update,             0, 1);     // Switch interface
    SCH_Add_Task(HEARTBEAT_SW_U_Update2, 0, 1000);  // Heartbeat LED
    SCH_Add_Task(UART2_BUF_O_Update,     0, 1);     // UART-USB reports
    SCH_Add_Task(protocol_update,        0, 10);    // protocol task
}

//SDCC for C8051F38x
__sfr __at(0xAA) _XPAGE; // EMI0CN for C8015F384

void _sdcc_external_startup(void)
{
    PCA0MD &= ~0x40;    //disable watchdog
}

