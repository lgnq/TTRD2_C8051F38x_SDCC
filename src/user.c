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
    CLKSEL  = 0x03;                     // Select system clock from internal High-Frequency Osc = 48MHz
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
    TMR2RL = -(SYSCLK / 12 / 1000);
    TMR2 = 0xffff;                // Set to reload immediately

    // Not yet started timer
}

void timer_start(void)
{
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

}

void mcu_init(void)
{
#if 0
    P0MDOUT = 0x03;                     // LED-R, LED-G
    P1MDOUT = 0x00;                     //
    P2MDOUT = 0x00;                     // LED1, 2
    P3MDOUT = 0x00;                     //
    P4MDOUT = 0x00;

    P0SKIP = 0xCF;                      // GPIO, GPIO, TX, RX, GPIO...
    P1SKIP = 0xFF;                      // GPIOs
    P2SKIP = 0xFF;                      // GPIOs
    P3SKIP = 0xFF;                      // GPIOs

    XBR0 = 0x01;                        // .... ...(UART0)
    XBR1 = 0xC0;                        // disable weak pull-up, XBAR enable, important always!!!
    XBR2 = 0x00;                        //

    EX0 = 0;        // INT0 disable
    EX1 = 0;        // INT1 disable
    IT0 = 1;        // Edge trigger
    IT1 = 1;        // Edge trigger
    IT01CF = 0x76;                      // P0.6(MPO0) as INT0, P0.7(MPO1) as INT1

    LED_R = 1;
    LED_G = 0;

    P0 = 0xFF;
#endif // 0

    //init system clock
    system_clock_init(16000000);

    int_enable();
}

void tasks_init(void)
{
    // Set up WDT
    // Timeout is WDT count value: approx 32 per millisecond
    // => a count of 64 gives a timeout of approx 2 ms
    // NOTE: WDT driven by RC oscillator - timing varies with temperature
    WATCHDOG_Init(60);

    // Prepare for switch task
    SWITCH_BUTTON1_Init();

    // Prepare for heartbeat-switch task
    HEARTBEAT_SW_U_Init();

    // Prepare for UART1 task (set baud rate)
    UART2_BUF_O_Init(115200);

    // Report mode (via buffer)
    UART2_BUF_O_Write_String_To_Buffer("\nNormal mode\n");

    // Add tasks to schedule.
    // Parameters are:
    // A. Task name
    // B. Initial delay / offset (in Ticks)
    // C. Task period (in Ticks): Must be > 0
    //           A                       B  C
    SCH_Add_Task(WATCHDOG_Update,        0, 1);     // Feed iWDT
    SCH_Add_Task(SWITCH_BUTTON1_Update,  0, 1);     // Switch interface
    SCH_Add_Task(HEARTBEAT_SW_U_Update1, 0, 1000);  // Heartbeat LED
    SCH_Add_Task(UART2_BUF_O_Update,     0, 1);     // UART-USB reports
    SCH_Add_Task(protocol_update,        0, 10);    // protocol task
}
