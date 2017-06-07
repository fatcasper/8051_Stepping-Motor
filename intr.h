#ifndef __INTR_H__
#define __INTR_H__

#include "sys_type.h"

//#define AUTO_STALL_STOP

#ifdef AUTO_STALL_STOP
#define CNT_2MS_NOLIMIT	5500
#define CNT_2MS_LIMIT	1500

extern UINT Cnt_2MS;
#endif

void intr_init(void);

#endif