/*----------------------------------------------------------------------------*-

  ttrd2-03a-t0401a-v001a_switch_task.h (Release 2017-02-22a)

  ----------------------------------------------------------------------------

  - See ttrd2-03a-t0401a-v001a_switch_task.c for details.

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

#ifndef __KEY_TASK_H__
#define __KEY_TASK_H__

// Processor Header
#include "main.h"

// ------ Public constants ---------------------------------------------------

SBIT(MODE_KEY, SFR_P3, 0);

//#define KEY_INPUT   (MODE_KEY)
#define KEY_INPUT   (~P3 & 0x3F)

#define KEY_MODE	    0x01
#define KEY_ENTER	    0x02
#define KEY_UP		    0x04
#define KEY_DOWN	    0x08
#define KEY_LEFT	    0x10
#define KEY_RIGHT	    0x20

#define KEY_L_MODE	    (KEY_MODE  | 0x80)
#define KEY_L_ENTER	    (KEY_ENTER | 0x80)
#define KEY_L_UP		(KEY_UP    | 0x80)
#define KEY_L_DOWN	    (KEY_DOWN  | 0x80)
#define KEY_L_LEFT	    (KEY_LEFT  | 0x80)
#define KEY_L_RIGHT	    (KEY_RIGHT | 0x80)

#define N_KEY       0
#define S_KEY       1
#define D_KEY       2
#define L_KEY       3

#define KEY_STATE_0 0
#define KEY_STATE_1 1
#define KEY_STATE_2 2
#define KEY_STATE_3 3

void key_init(void);
void key_update(void);

#endif

/*----------------------------------------------------------------------------*-
   ------------------------------ END OF FILE ---------------------------------
-*----------------------------------------------------------------------------*/
