/*----------------------------------------------------------------------------*-

  ttrd2-03a-t0401a-v001a_uart2_buff_o_task.h (Release 2017-02-22a)

  ----------------------------------------------------------------------------

  See ttrd2-03a-t0401a-v001a_uart2_buff_o_task.c for details.

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

#ifndef __UART_TASK_H__
#define __UART_TASK_H__

// Processor Header
#include "main.h"

#define MAX_LENGTH  100

#define START_BYTE  0x55
#define END_BYTE    0x56

#define STATE_IDLE      0
#define STATE_START_OK  1
#define STATE_LEN_OK    2
#define STATE_CMD_OK    3
#define STATE_DAT_OK    4
#define STATE_CRC_OK    5
#define STATE_END_OK    6

#define WAIT_FOR_SOF1       0
#define WAIT_FOR_SOF2       1
#define WAIT_FOR_SOF3       2
#define WAIT_FOR_SOF4       3
#define WAIT_FOR_SOF5       4
#define WAIT_FOR_SOF6       5
#define WAIT_FOR_SOF7       6
#define WAIT_FOR_SOF8       7
#define WAIT_FOR_SOF9       8
#define WAIT_FOR_PGN        9
#define WAIT_FOR_SRC        10
#define WAIT_FOR_DST        11
#define WAIT_FOR_PRI        12
#define WAIT_FOR_DATA       13
#define EOF                 14

#define WAIT_FOR_DAT1       13
#define WAIT_FOR_DAT2       14
#define WAIT_FOR_DAT3       15
#define WAIT_FOR_DAT4       16
#define WAIT_FOR_DAT5       17
#define WAIT_FOR_DAT6       18
#define WAIT_FOR_DAT7       19
#define WAIT_FOR_DAT8       20
#define WAIT_FOR_DAT9       21

#define CMD_BTCAR       0
#define CMD_BTFRT       1
#define CMD_BTSEC       2
#define CMD_NONE        3

#define WAIT_FOR_SOF_AT_BTC  5
#define WAIT_FOR_SOF_AT_BTF  6
#define WAIT_FOR_SOF_AT_BTS  7
#define WAIT_FOR_SOF_AT_BTCA  8
#define WAIT_FOR_SOF_AT_BTFR  9
#define WAIT_FOR_SOF_AT_BTSE  10
#define WAIT_FOR_SOF_AT_BTCAR  11
#define WAIT_FOR_SOF_AT_BTFRT  12
#define WAIT_FOR_SOF_AT_BTSEC  13

#if 0
#define WAIT_FOR_PGN    1
#deifne WAIT_FOR_SRC    2   //source address
#deifne WAIT_FOR_DST    3   //destination address
#deifne WAIT_FOR_PRI    4   //priority
#deifne WAIT_FOR_IDT    5   //message identifier
#deifne WAIT_FOR_MSG    6   //messages
#define WAIT_FOR_EOF    7   //SOT #
#endif

// ------ Public function prototypes -----------------------------------------

void UART2_BUF_O_Init(uint32_t BAUD_RATE);
void UART2_BUF_O_Update(void);

void UART2_BUF_O_Send_All_Data(void);

void UART2_BUF_O_Write_String_To_Buffer(const char* const);
void UART2_BUF_O_Write_Char_To_Buffer(const char);

void UART2_BUF_O_Write_Number10_To_Buffer(const uint32_t DATA);
void UART2_BUF_O_Write_Number03_To_Buffer(const uint32_t DATA);
void UART2_BUF_O_Write_Number02_To_Buffer(const uint32_t DATA);

void protocol_update(void);

#endif

/*----------------------------------------------------------------------------*-
   ------------------------------ END OF FILE ---------------------------------
-*----------------------------------------------------------------------------*/
