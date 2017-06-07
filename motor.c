#include <reg52.h>
#include "sys_type.h"
#include "motor.h"
#include "key.h"
#include "intr.h"

bit Flag_StepCntFwd = 0;	//正转计数标记位
bit Flag_StepCntRev = 0;	//反转计数标记位
bit Flag_StepCntFwdNoLimit = 0;	//无限正转计数标记位
bit Flag_StepCntRevNoLimit = 0;	//无限反转计数标记位
LONG beats = 0;	//电机转动节拍总数

#ifdef TWO_TWO_STEPS
UCHAR code BeatCode[2][4] = {
#elif defined ONE_TWO_STEPS
UCHAR code BeatCode[2][8] = {
#endif
#ifdef TWO_TWO_STEPS
	/*********************************
		BJB B1856GW Board
	*********************************/
	{0x0C, 0x06, 0x03, 0x09},
	{0x09, 0x03, 0x06, 0x0C}
#elif defined ONE_TWO_STEPS
//	{0x08, 0x0C, 0x04, 0x06, 0x02, 0x03, 0x01, 0x09},
	{0x01, 0x03, 0x02, 0x06, 0x04, 0x0C, 0x08, 0x09},
/*	1000	1100	0100	0110	0010	0011	0001	1001 */
/*	A		AB		B		BC		C		CD		D		DA	 */
//	{0x08, 0x09, 0x01, 0x03, 0x02, 0x06, 0x04, 0x0C}
	{0x01, 0x09, 0x08, 0x0C, 0x04, 0x06, 0x02, 0x03}
/*	1000	1001	0001	0011	0010	0110	0100	1100 */
/*	A		AD		D		DC		C		CB		B		BA	 */
#endif
};

/* 步进电机启动函数，angle-需转过的角度 */
void StartMotorAng(LONG angle)
{
	EA = 0;
#ifdef KINGST_BOARD
	beats = (angle * 4076) / 360;	//实测为4076拍转动一圈
#endif
#ifdef TWO_TWO_STEPS
	if(angle == 0xFF){//使用0xFF设置步进电机无限循环
		beats = 0xFF;
	}else if(angle == (-0xFF)){//使用-0xFF设置步进电机无限循环
		beats = -0xFF;
	}else{
		beats = (angle * 2048) / 360;	//理论为2048拍转动一圈
	}
#elif defined ONE_TWO_STEPS
	if(angle == 0xFF){//使用0xFF设置步进电机无限循环
		beats = 0xFF;
	}else if(angle == (-0xFF)){//使用-0xFF设置步进电机无限循环
		beats = -0xFF;
	}else{
		beats = (angle * 4096) / 360;	//理论为4096拍转动一圈
	}
#endif
	EA = 1;
}

void StartMotorStepFwd(LONG steps)
{
	if(Flag_StallFwd == 0){
		EA = 0;
		beats = steps;	
		EA = 1;
	}else{
		Flag_StallFwd = 0;
		steps = 0;
		beats = 0;
//		Flag_StepCntFwdNoLimit = 1;
//		StartMotorAng(0xFF);
		Flag_StepCntRevNoLimit = 1;	//设定步数走完，无限反转，方便辨识
		StartMotorAng(-0xFF);	//设定步数走完，无限反转，方便辨识
	}
}

void StartMotorStepRev(LONG steps)
{
	if(Flag_StallRev == 0){
		EA = 0;
		beats = steps;	
		EA = 1;
	}else{
		Flag_StallRev = 0;
		steps = 0;
		beats = 0;
//		Flag_StepCntRevNoLimit = 1;
//		StartMotorAng(-0xFF);
		Flag_StepCntFwdNoLimit = 1;
		StartMotorAng(0xFF);
	}
}

/* 步进电机停止函数 */
void StopMotor(void)
{
	EA = 0;
	beats = 0;
	EA = 1;
}

/* 电机转动控制函数 */
void TurnMotor()
{
	UCHAR tmp;	//临时变量
 	static UCHAR index = 0;	//节拍输出索引
    
	if(beats){//节拍数不为0则产生一个驱动节拍
		if(beats > 0){//节拍数大于0时正转
			index++;	//正转时节拍输出索引递增
#ifdef ONE_TWO_STEPS
			index = index & 0x07; //用&操作实现到8归零
#elif defined TWO_TWO_STEPS
			index = index & 0x0B; //用&操作实现到4归零
#endif
			if(Flag_StepCntFwdNoLimit == 1 && beats == 0xFF){
				;
			}else{
				beats--;	//正转时节拍计数递减
			}
		}else{//节拍数小于0时反转
			index++;	//反转时节拍输出索引递增
#ifdef ONE_TWO_STEPS
			index = index & 0x07;	//用&操作同样可以实现到-1时归7
#elif defined TWO_TWO_STEPS
			index = index & 0x0B;	//用&操作实现到4归零
#endif
			if(Flag_StepCntRevNoLimit == 1 && beats == (-0xFF)){
				;
			}else{
				beats++;	//反转时节拍计数递减
			}
		}
		tmp = P1;	//用tmp把P1口当前值暂存
		tmp = tmp & 0xF0;	//用&操作清零低4位
		if(beats > 0){
			tmp = tmp | BeatCode[0][index]; //用|操作把节拍代码写到低4位
		}else{
			tmp = tmp | BeatCode[1][index]; //用|操作把节拍代码写到低4位
		}
		P1  = tmp;	//把低4位的节拍代码和高4位的原值送回P1
	}else{//节拍数为0则关闭电机所有的相
#ifdef KINGST_BOARD
	/*********************************
		金沙滩51 Developer Board
	*********************************/
		P1 = P1 | 0x0F;
#else
		P1 = P1 & 0x00;
#endif
		index = 0;
		Flag_StepCntFwdNoLimit = 0;
		Flag_StepCntRevNoLimit = 0;
		Flag_StepCntFwd = 0;
		Flag_StepCntRev = 0;
	}
}