#ifndef __MOTOR_H__
#define __MOTOR_H__

//#define TWO_TWO_STEPS	//正转:AB→BC→CD→DA方式
						//反转:AB→DA→CD→BC方式
#define ONE_TWO_STEPS	//1-2相控制方式
//#define KINGST_BOARD	//开发板电路控制

#define MOTOR_ONE_STEP_ANG			5.625	//步进电机单步角度
#define MOTOR_UNREACHABLE_OFFSET	32.5	//分配阀无法到达的位置角度

extern bit Flag_StepCntFwd;			//正转计数标记位
extern bit Flag_StepCntRev;			//反转计数标记位
extern bit Flag_StepCntFwdNoLimit;	//无限正转计数标记位
extern bit Flag_StepCntRevNoLimit;	//无限反转计数标记位 
extern LONG beats;  				//电机转动节拍总数

void StartMotorAng(LONG angle);
void StartMotorStepFwd(LONG steps);
void StartMotorStepRev(LONG steps);
void StopMotor(void);
void TurnMotor(void);

#endif