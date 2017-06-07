#include <reg52.h>
#include "intr.h"
#include "key.h"
#include "mcr_bit.h"
#include "motor.h"

//#define ITR_2MS	//2秒计时开始使能
#define TM0CNTER 33 //2ms计时

static UCHAR TM0Cnt = 0;
#ifdef AUTO_STALL_STOP
UINT Cnt_2MS = 0;
#endif

void intr_init(void)
{
	EA = 0;
	TMOD = 0x01;
	TH0  = 0xFC;
	TL0  = 0x67;
	ET0  = 1;
 	TR0  = 1; 
	EA = 1;
}

void InterruptTimer0() interrupt 1
{
	static bit div = 0;

	TM0Cnt++;
	/* 2ms */
	TH0 = 0xFC;
  	TL0 = 0x67;
 	KeyScan();
#ifdef ITR_2MS
	div = ~div;
	
  	if(div == 1){
		TurnMotor();
  	}
#endif
	if(Flag_StepCntFwdNoLimit || Flag_StepCntRevNoLimit){
		div = ~div;
	
		if(div == 1){
#ifdef AUTO_STALL_STOP
			if(!Flag_StopNoLimit){
				Cnt_2MS++;
			}
			if(Cnt_2MS >= CNT_2MS_NOLIMIT){
				Cnt_2MS = 0;
				beats = 0;
				Flag_StopNoLimit = 1;
			}
#endif
			TurnMotor();
		}
	}else{
#ifndef ITR_2MS
		if(TM0Cnt >= TM0CNTER){
			TM0Cnt = 0;
			TurnMotor();
		}
	}
#endif
}
