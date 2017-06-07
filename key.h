#ifndef __KEY_H__
#define __KEY_H__

#include "sys_type.h"

extern bit Flag_StallFwd;	//��ת��ת�������λ
extern bit Flag_StallRev;	//��ת��ת�������λ
#ifdef AUTO_STALL_STOP
extern bit Flag_StopNoLimit;
extern bit Flag_Stop;
#endif
extern UCHAR code KeyCodeMap[4][4];
extern UCHAR KeySta[4][4];

void KeyDriver(void);
void KeyScan(void);
void KeyAction(UCHAR keycode);

#endif