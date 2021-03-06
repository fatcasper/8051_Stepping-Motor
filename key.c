#include "sys_type.h"
#include "mcr_bit.h"
#include "key.h"
#include "motor.h"

#ifdef TWO_TWO_STEPS
#define KEY_PRESS_CNT 70		//按键运转结束后堵转清零
#elif defined ONE_TWO_STEPS
//#define KEY_PRESS_CNT (((360 - MOTOR_UNREACHABLE_OFFSET) / MOTOR_ONE_STEP_ANG) * 64)	//按键运转结束后堵转清零(124步)
#define KEY_PRESS_CNT	115
#endif

bit Flag_StallFwd = 0;	//堵转正转计数标记位
bit Flag_StallRev = 0;	//堵转反转计数标记位
#ifdef AUTO_STALL_STOP
bit Flag_StopNoLimit = 0;
bit Flag_Stop = 0;
#endif
static UCHAR keyCntFwd = 0;	//正向按键次数计数
static UCHAR keyCntRev = 0;	//反向按键次数计数

UCHAR code KeyCodeMap[4][4] = { //矩阵按键编号到标准键盘键码的映射表
	{ 0x31, 0x32, 0x33, 0x26 },	//数字键1、数字键2、数字键3、向上键
	{ 0x34, 0x35, 0x36, 0x25 }, //数字键4、数字键5、数字键6、向左键
	{ 0x37, 0x38, 0x39, 0x28 }, //数字键7、数字键8、数字键9、向下键
	{ 0x30, 0x1B, 0x0D, 0x27 }  //数字键0、ESC键、  回车键、 向右键
};

UCHAR KeySta[4][4] = {//全部矩阵按键的当前状态
	{1, 1, 1, 1},
	{1, 1, 1, 1},
	{1, 1, 1, 1},
	{1, 1, 1, 1}
};

/* 按键动作函数，根据键码执行相应的操作，keycode-按键键码 */
void KeyAction(UCHAR keycode)
{	
	if((keycode>=0x30) && (keycode<=0x39)){  //控制电机转动1-9圈
		;
	}else if(keycode == 0x26){//向上键，控制转动方向为正转
		Flag_StepCntFwdNoLimit = 1;
		StartMotorAng(0xFF);
	}else if(keycode == 0x28){//向下键，控制转动方向为反转
		Flag_StepCntRevNoLimit = 1;
		StartMotorAng(-0xFF);
	}else if(keycode == 0x25){//向左键，固定正转30步
		Flag_StepCntFwd = 1;
		keyCntFwd++;
		if(keyCntFwd >= KEY_PRESS_CNT){
			keyCntFwd = 0;
			keyCntRev = 0;
			Flag_StallFwd = 1;
		}
		StartMotorStepFwd(30);	//正向运转64步，减速比1/64，约11.25°，32次走完
	}else if(keycode == 0x27){//向右键，固定反转30步
		Flag_StepCntRev = 1;
		keyCntRev++;
		if(keyCntRev >= KEY_PRESS_CNT){
			keyCntRev = 0;
			keyCntFwd = 0;
			Flag_StallRev = 1;
		}
		StartMotorStepRev(-30);	//反向运转64步，减速比1/64，约11.25°，32次走完
	}else if(keycode == 0x1B){//Esc键，停止转动
		StopMotor();
	}
}

/* 按键驱动函数，检测按键动作，调度相应动作函数，需在主循环中调用 */
void KeyDriver(void)
{
	UCHAR i, j;

	static UCHAR backup[4][4] = {//按键值备份，保存前一次的值
		{1, 1, 1, 1},  
		{1, 1, 1, 1},  
		{1, 1, 1, 1}, 
		{1, 1, 1, 1}
	};
	
	for(i=0; i<4; i++){//循环检测4*4的矩阵按键
		for(j=0; j<4; j++){
			if(backup[i][j] != KeySta[i][j]){//检测按键动作
				if (backup[i][j] != 0){//按键按下时执行动作
					KeyAction(KeyCodeMap[i][j]);	//调用按键动作函数
				}
			backup[i][j] = KeySta[i][j];	//刷新前一次的备份值
			}
		}
	}
}

/* 按键扫描函数，需在定时中断中调用，推荐调用间隔1ms */
void KeyScan(void)
{
    UCHAR i;
	
	
	static UCHAR keyout = 0;   //矩阵按键扫描输出索引
	static UCHAR keybuf[4][4] = {//矩阵按键扫描缓冲区
		   {0xFF, 0xFF, 0xFF, 0xFF},  
		   {0xFF, 0xFF, 0xFF, 0xFF},
		   {0xFF, 0xFF, 0xFF, 0xFF}, 
		   {0xFF, 0xFF, 0xFF, 0xFF}
	};

    //将一行的4个按键值移入缓冲区
    keybuf[keyout][0] = (keybuf[keyout][0] << 1) | KEY_IN_1;
    keybuf[keyout][1] = (keybuf[keyout][1] << 1) | KEY_IN_2;
    keybuf[keyout][2] = (keybuf[keyout][2] << 1) | KEY_IN_3;
    keybuf[keyout][3] = (keybuf[keyout][3] << 1) | KEY_IN_4;
    //消抖后更新按键状态
    for (i=0; i<4; i++){//每行4个按键，所以循环4次
			if((keybuf[keyout][i] & 0x0F) == 0x00){//连续4次扫描值为0，即4*4ms内都是按下状态时，可认为按键已稳定的按下
				KeySta[keyout][i] = 0;
			}else if ((keybuf[keyout][i] & 0x0F) == 0x0F){//连续4次扫描值为1，即4*4ms内都是弹起状态时，可认为按键已稳定的弹起
				KeySta[keyout][i] = 1;
			}
    }
    //执行下一次的扫描输出
    keyout++;	//输出索引递增
    keyout = keyout & 0x03;  //索引值加到4即归零
    switch (keyout){         //根据索引，释放当前输出引脚，拉低下次的输出引脚
			case 0: 
				KEY_OUT_4 = 1; 
				KEY_OUT_1 = 0;
				break;
			case 1: 
				KEY_OUT_1 = 1; 
				KEY_OUT_2 = 0; 
				break;
			case 2: 
				KEY_OUT_2 = 1; 
				KEY_OUT_3 = 0; 
				break;
			case 3: 
				KEY_OUT_3 = 1; 
				KEY_OUT_4 = 0; 
				break;
			default:
				break;
    }
}
