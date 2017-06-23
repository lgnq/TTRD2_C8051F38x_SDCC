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
    Out_written_index_g = 0;
    Out_waiting_index_g = 0;
    In_read_index_g     = 0;
    In_waiting_index_g  = 0;

    SCON0 = 0x10;                       // SCON0: 8-bit variable bit rate
                                        //        level of STOP bit is ignored
                                        //        RX enabled
                                        //        ninth bits are zeros
                                        //        clear RI0 and TI0 bits
    if (SYSCLK/BAUDRATE/2/256 < 1)
    {
        TH1    = -(SYSCLK/BAUDRATE/2);
        CKCON &= ~0x0B;                  // T1M = 1; SCA1:0 = xx
        CKCON |=  0x08;
    }
    else if (SYSCLK/BAUDRATE/2/256 < 4)
    {
        TH1    = -(SYSCLK/BAUDRATE/2/4);
        CKCON &= ~0x0B;                  // T1M = 0; SCA1:0 = 01
        CKCON |=  0x01;
    }
    else if (SYSCLK/BAUDRATE/2/256 < 12)
    {
        TH1    = -(SYSCLK/BAUDRATE/2/12);
        CKCON &= ~0x0B;                  // T1M = 0; SCA1:0 = 00
    }
    else
    {
        TH1    = -(SYSCLK/BAUDRATE/2/48);
        CKCON &= ~0x0B;                  // T1M = 0; SCA1:0 = 10
        CKCON |=  0x02;
    }

    TL1   = TH1;                         // init Timer1
    TMOD &= ~0xf0;                       // TMOD: timer 1 in 8-bit autoreload
    TMOD |=  0x20;
    TR1   = 1;                           // START Timer1
    ES0   = 0;                           // Disable UART0 interrupts

    //init UART pins
    P0SKIP = 0xCF;                       // GPIO, GPIO, TX, RX, GPIO...
    XBR0   = 0x01;                       // .... ...(UART0)
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
    SBUF0 = CHARACTER;

    while (!TI0)
        ;

    TI0 = 0;
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

uint8_t state = STATE_IDLE;
#if 0
uint8_t length = 0;
uint8_t cmd = 0;
uint8_t crc = 0;

void protocol_update(void)
{
    uint8_t c;
    uint8_t i;

    c = uart_read_char_from_buffer();
    if (c != PC_LINK_NO_CHAR)
    {
        //protocol_processor(c);
        switch (state)
        {
        case STATE_IDLE:
            if (c == START_BYTE)
                state = STATE_START_OK;
            break;
        case STATE_START_OK:
            if (c > MAX_LENGTH)
                state = STATE_IDLE;
            else
                length = c;
                state = STATE_LEN_OK;
            break;
        case STATE_LEN_OK:
            cmd = c;
            state = STATE_CMD_OK;
            break;
        case STATE_CMD_OK:
            for (i = 0; i < length - 5; i++)
                UART2_BUF_O_Write_Char_To_Buffer(c);
            state = STATE_DAT_OK;
            break;
        case STATE_DAT_OK:
            crc = c;

            //if CRC OK
                state = STATE_CRC_OK;
            //else
            //    state = STATE_IDLE;
            break;
        case STATE_CRC_OK:
            if (c == END_BYTE)
                state = STATE_END_OK;
            else
                state = STATE_IDLE;
            break;
        case STATE_END_OK:
            UART2_BUF_O_Write_String_To_Buffer("package is ok\r\n");
            state = STATE_IDLE;
            break;
        default:
            break;
        }
    }
}
#else

uint8_t cmd = CMD_NONE;
uint8_t pgn[5] =
{
    0,
    0,
    0,
    0,
    0
};

uint8_t src[3] =
{
    0,
    0,
    0,
};

uint8_t dst[3] =
{
    0,
    0,
    0,
};

uint8_t timestamp[14];

uint8_t idx = 0;
uint8_t priority = 0;

uint8_t buff[50];

void protocol_update(void)
{
    uint8_t c;

    c = uart_read_char_from_buffer();

    if (c != PC_LINK_NO_CHAR)
    {
        UART2_BUF_O_Write_Char_To_Buffer(c);

        switch (state)
        {
        case WAIT_FOR_SOF1:
            if (c == 'A')
                state = WAIT_FOR_SOF2;
            break;
        case WAIT_FOR_SOF2:
            if (c == 'T')
                state = WAIT_FOR_SOF3;
            else
                state = WAIT_FOR_SOF1;
            break;
        case WAIT_FOR_SOF3:
            if (c == '+')
                state = WAIT_FOR_SOF4;
            else
                state = WAIT_FOR_SOF1;
            break;
        case WAIT_FOR_SOF4:
            if (c == 'B')
                state = WAIT_FOR_SOF5;
            else
                state = WAIT_FOR_SOF1;
            break;
        case WAIT_FOR_SOF5:
            if (c == 'T')
                state = WAIT_FOR_SOF6;
            else
                state = WAIT_FOR_SOF1;
            break;
        case WAIT_FOR_SOF6:
            if (c == 'C')
            {
                cmd = CMD_BTCAR;
                state = WAIT_FOR_SOF7;
            }
            else if (c == 'F')
            {
                cmd = CMD_BTFRT;
                state = WAIT_FOR_SOF7;
            }
            else if (c == 'S')
            {
                cmd = CMD_BTSEC;
                state = WAIT_FOR_SOF7;
            }
            else
            {
                cmd = CMD_NONE;
                state = WAIT_FOR_SOF1;
            }
            break;
        case WAIT_FOR_SOF7:
            if (cmd == CMD_BTCAR)
            {
                if (c == 'A')
                    state = WAIT_FOR_SOF8;
                else
                {
                    cmd = CMD_NONE;
                    state = WAIT_FOR_SOF1;
                }
            }
            else if(cmd == CMD_BTFRT)
            {
                if (c == 'R')
                    state = WAIT_FOR_SOF8;
                else
                {
                    cmd = CMD_NONE;
                    state = WAIT_FOR_SOF1;
                }
            }
            else if(cmd == CMD_BTSEC)
            {
                if (c == 'E')
                    state = WAIT_FOR_SOF8;
                else
                {
                    cmd = CMD_NONE;
                    state = WAIT_FOR_SOF1;
                }
            }
            break;
        case WAIT_FOR_SOF8:
            if (cmd == CMD_BTCAR)
            {
                if (c == 'R')
                    state = WAIT_FOR_SOF9;
                else
                {
                    cmd = CMD_NONE;
                    state = WAIT_FOR_SOF1;
                }
            }
            else if(cmd == CMD_BTFRT)
            {
                if (c == 'T')
                    state = WAIT_FOR_SOF9;
                else
                {
                    cmd = CMD_NONE;
                    state = WAIT_FOR_SOF1;
                }
            }
            else if(cmd == CMD_BTSEC)
            {
                if (c == 'C')
                    state = WAIT_FOR_SOF9;
                else
                {
                    cmd = CMD_NONE;
                    state = WAIT_FOR_SOF1;
                }
            }
            break;
        case WAIT_FOR_SOF9:
            if (c == '=')
            {
                idx = 0;
                state = WAIT_FOR_PGN;
            }
            else
                state = WAIT_FOR_SOF1;
            break;
        case WAIT_FOR_PGN:
            if (c != ',')
            {
                pgn[idx++] = c;
            }
            else
            {
                idx = 0;
                state = WAIT_FOR_SRC;
            }
            break;
        case WAIT_FOR_SRC:
            if (c != ',')
            {
                src[idx++] = c;
            }
            else
            {
                idx = 0;
                state = WAIT_FOR_DST;
            }
            break;
        case WAIT_FOR_DST:
            if (c != ',')
            {
                dst[idx++] = c;
            }
            else
            {
                state = WAIT_FOR_PRI;
            }
            break;
        case WAIT_FOR_PRI:
            idx = 0;
            priority = c;
            state = WAIT_FOR_DATA;
            break;
        case WAIT_FOR_DATA:
            if (c == '#')
            {
                state = WAIT_FOR_SOF1;

                UART2_BUF_O_Write_String_To_Buffer("Process data now!\n");

                //process data
            }
            else
            {
                buff[idx++] = c;
            }

            //if (timeout)
            //    state = WAIT_FOR_SOF1;
            break;
        default:
            break;
        }
    }
}
#endif

/*----------------------------------------------------------------------------*-
  ------------------------------ END OF FILE ---------------------------------
-*----------------------------------------------------------------------------*/

