/*----------------------------------------------------------------------------*-

  ttrd2-03a-t0401a-v001a_uart2_buff_o_task.c (Release 2017-02-22a)

  ----------------------------------------------------------------------------

  Simple UART2 'buffered output' library for STM32F4 (Nucleo-F401RE board).

  Also offers unbuffered ('flush whole buffer') option: use this with care!

  Allows use of USB port on board as UART2 interface.

  See 'ERES2' (Chapter 3) for further information.

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

#include "main.h"

// ------ Private constants --------------------------------------------------

// Value returned by PC_LINK_Get_Char if no character is
// available in buffer
#define PC_LINK_NO_CHAR 127
#define XON 0x11
#define XOFF 0x13

// The transmit buffer length
#define Tx_buffer_g_SIZE_BYTES 200
// The receive buffer length
#define Rx_buffer_g_SIZE_BYTES 200

// ------ Private variables --------------------------------------------------

static char Tx_buffer_g[Tx_buffer_g_SIZE_BYTES];
static char Rx_buffer_g[Rx_buffer_g_SIZE_BYTES];

static uint32_t Out_written_index_g;  // Data in buffer that has been written
static uint32_t Out_waiting_index_g;  // Data in buffer not yet written
static uint32_t In_read_index_g;      // Data in buffer that has been received
static uint32_t In_waiting_index_g;   // Data in buffer not yet received

// ------ Private function prototypes ----------------------------------------

void UART2_BUF_O_Send_Char(const char);

/*----------------------------------------------------------------------------*-

  UART2_BUF_O_Init()

  Set up UART1.

  PARAMETER:
     Required baud rate.

  LONG-TERM DATA:
     None.

  MCU HARDWARE:
     UART1.

  PRE-CONDITION CHECKS:
     None.

  POST-CONDITION CHECKS:
     None.

  ERROR DETECTION / ERROR HANDLING:
     None.

  RETURN VALUE:
     None.

-*----------------------------------------------------------------------------*/
void UART2_BUF_O_Init(uint32_t BAUD_RATE)
{
    //todo : init the uart
}

/*----------------------------------------------------------------------------*-

  UART2_BUF_O_Update()

  Sends next character from the software transmit buffer

  NOTE: Output-only library (Cannot receive chars)

  Uses on-chip UART hardware.

  PARAMETERS:
     None.

  LONG-TERM DATA:
     None.

  MCU HARDWARE:
     UART1.

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
void UART2_BUF_O_Update(void)
{
    // Deal with transmit bytes here

    // Is there any data ready to send?
    if (Out_written_index_g < Out_waiting_index_g)
    {
        UART2_BUF_O_Send_Char(Tx_buffer_g[Out_written_index_g]);

        Out_written_index_g++;
    }
    else
    {
        // No data to send - just reset the buffer index
        Out_waiting_index_g = 0;
        Out_written_index_g = 0;
    }

    if (RI0 == 1)
    {
        // Flag only set when a valid stop bit is received,
        // -> data ready to be read into the received buffer
        // Want to read into index 0, if old data have been read
        // (simple ~circular buffer)
        if (In_waiting_index_g == In_read_index_g)
        {
            In_waiting_index_g = 0;
            In_read_index_g = 0;
        }

        // Read the data from USART buffer
        Rx_buffer_g[In_waiting_index_g] = SBUF0;

        if (In_waiting_index_g < Rx_buffer_g_SIZE_BYTES)
        {
            In_waiting_index_g++;
        }

        //todo : clear the RX flag
        RI0 = 0;
    }
}

/*----------------------------------------------------------------------------*-

  UART2_BUF_O_Send_All_Data()

  Sends all data from the software transmit buffer.

  NOTES:
  * May have very long execution time!
  * Intended for use when the scheduler is NOT running.

  PARAMETERS:
     None.

  LONG-TERM DATA:
     None.

  MCU HARDWARE:
     UART1.

  PRE-CONDITION CHECKS:
     None.

  POST-CONDITION CHECKS:
     None.

  ERROR DETECTION / ERROR HANDLING:
     None.

  RETURN VALUE:
     None.

-*----------------------------------------------------------------------------*/
void UART2_BUF_O_Send_All_Data(void)
{
    while (Out_written_index_g < Out_waiting_index_g)
    {
        UART2_BUF_O_Send_Char(Tx_buffer_g[Out_written_index_g]);

        Out_written_index_g++;
    }

    // Reset the buffer indices
    Out_waiting_index_g = 0;
    Out_written_index_g = 0;
}

/*----------------------------------------------------------------------------*-

  UART2_BUF_O_Write_String_To_Buffer()

  Copies a (null terminated) string to the module Tx buffer.

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
void UART2_BUF_O_Write_String_To_Buffer(const char* const STR_PTR)
{
    uint32_t i = 0;

    while (STR_PTR[i] != '\0')
    {
        UART2_BUF_O_Write_Char_To_Buffer(STR_PTR[i]);
        i++;
    }
}

/*----------------------------------------------------------------------------*-

  UART2_BUF_O_Write_Char_To_Buffer()

  Stores a single character in the Tx buffer.

  PARAMETERS:
     CHARACTER is character to be stored.

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
void UART2_BUF_O_Write_Char_To_Buffer(const char CHARACTER)
{
    // Write to the buffer *only* if there is space
    if (Out_waiting_index_g < Tx_buffer_g_SIZE_BYTES)
    {
        Tx_buffer_g[Out_waiting_index_g] = CHARACTER;
        Out_waiting_index_g++;
    }
    else
    {
        // Write buffer is full
        // No error handling / reporting here (characters may be lost)
        // Adapt as required to meet the needs of your application
    }
}

uint8_t uart_read_char_from_buffer(void)
{
    uint8_t c = PC_LINK_NO_CHAR;

    // If there is new data in the buffer
    if (In_read_index_g < In_waiting_index_g)
    {
        c = Rx_buffer_g[In_read_index_g];
        if (In_read_index_g < Rx_buffer_g_SIZE_BYTES)
        {
            In_read_index_g++;
        }
    }

    return c;
}

/*----------------------------------------------------------------------------*-

  UART2_BUF_O_Write_Number10_To_Buffer()

  Writes 10-digit (decimal, unsigned) number to Tx buffer as a string.

  Supported values: 0 - 9,999,999,999.

  (Can be used with 32-bit unsigned integer values.)

  PARAMETERS:
     DATA : The number to be stored.

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
void UART2_BUF_O_Write_Number10_To_Buffer(const uint32_t DATA)
{
    char Digit[11];

    int_disable();
    Digit[10] = '\0';  // Null terminator
    Digit[9]  = 48 + (DATA % 10);
    Digit[8]  = 48 + ((DATA/10) % 10);
    Digit[7]  = 48 + ((DATA/100) % 10);
    Digit[6]  = 48 + ((DATA/1000) % 10);
    Digit[5]  = 48 + ((DATA/10000) % 10);
    Digit[4]  = 48 + ((DATA/100000) % 10);
    Digit[3]  = 48 + ((DATA/1000000) % 10);
    Digit[2]  = 48 + ((DATA/10000000) % 10);
    Digit[1]  = 48 + ((DATA/100000000) % 10);
    Digit[0]  = 48 + ((DATA/1000000000) % 10);
    int_enable();

    UART2_BUF_O_Write_String_To_Buffer(Digit);
}

/*----------------------------------------------------------------------------*-

  UART2_BUF_O_Write_Number03_To_Buffer()

  Writes 3-digit (decimal, unsigned) number to Tx buffer as a string.

  Supported values: 0 - 999.

  PARAMETERS:
     DATA : The number to be stored.

  LONG-TERM DATA:
     None.

  MCU HARDWARE:
     None.

  PRE-CONDITION CHECKS:
     A simple range check is performed.

  POST-CONDITION CHECKS:
     None.

  ERROR DETECTION / ERROR HANDLING:
     The function simply returns if the data are out of range.

  RETURN VALUE:
     None.

-*----------------------------------------------------------------------------*/
void UART2_BUF_O_Write_Number03_To_Buffer(const uint32_t DATA)
{
    char Digit[4];

    if (DATA <= 999)
    {
        int_disable();
        Digit[3] = '\0';  // Null terminator
        Digit[2] = 48 + (DATA % 10);
        Digit[1] = 48 + ((DATA/10) % 10);
        Digit[0] = 48 + ((DATA/100) % 10);
        int_enable();

        UART2_BUF_O_Write_String_To_Buffer(Digit);
    }
}

/*----------------------------------------------------------------------------*-

  UART2_BUF_O_Write_Number02_To_Buffer()

  Writes 2-digit (decimal, unsigned) number to Tx buffer as a string.

  Supported values: 0 - 99.

  PARAMETERS:
     DATA : The number to be stored.

  LONG-TERM DATA:
     None.

  MCU HARDWARE:
     None.

  PRE-CONDITION CHECKS:
     A simple range check is performed.

  POST-CONDITION CHECKS:
     None.

  ERROR DETECTION / ERROR HANDLING:
     The function simply returns if the data are out of range.

  RETURN VALUE:
     None.

-*----------------------------------------------------------------------------*/
void UART2_BUF_O_Write_Number02_To_Buffer(const uint32_t DATA)
{
    char Digit[3];

    if (DATA <= 99)
    {
        int_disable();
        Digit[2] = '\0';  // Null terminator
        Digit[1] = 48 + (DATA % 10);
        Digit[0] = 48 + ((DATA/10) % 10);
        int_enable();
    }

    UART2_BUF_O_Write_String_To_Buffer(Digit);
}

/*----------------------------------------------------------------------------*-

  UART2_BUF_O_Send_Char()

  Uses on-chip UART0 hardware to send a single character.

  PARAMETERS:
     CHARACTER : The data to be sent.

  LONG-TERM DATA:
     None.

  MCU HARDWARE:
     UART1.

  PRE-CONDITION CHECKS:
     None.

  POST-CONDITION CHECKS:
     None.

  ERROR DETECTION / ERROR HANDLING:
     None.

  RETURN VALUE:
     None.

-*----------------------------------------------------------------------------*/
void UART2_BUF_O_Send_Char(const char CHARACTER)
{
#if 0
    // Wait for USART to be free
    // Requires a 'peripheral timeout' mechanism (see ERES2, Chapter 5)
    while (USART_GetFlagStatus(USART2, USART_FLAG_TXE) == 0);

    USART_SendData(USART2, CHARACTER);
#else
#endif
}

void protocol_processor(uint8_t c)
{
    // Perform the task
    switch (c)
    {
        case 'a':
        case 'A':
        {
            UART2_BUF_O_Write_String_To_Buffer("A pressed\r\n");
            break;
        }
        case 'b':
        case 'B':
        {
            UART2_BUF_O_Write_String_To_Buffer("B pressed\r\n");
            break;
        }
        case 'c':
        case 'C':
        {
            UART2_BUF_O_Write_String_To_Buffer("C pressed\r\n");
            break;
        }
        default:
            UART2_BUF_O_Write_String_To_Buffer("X pressed\r\n");
    }
}

void protocol_update(void)
{
    uint8_t c;

    c = uart_read_char_from_buffer();
    if (c != PC_LINK_NO_CHAR)
    {
        protocol_processor(c);
    }
}

/*----------------------------------------------------------------------------*-
  ------------------------------ END OF FILE ---------------------------------
-*----------------------------------------------------------------------------*/

