#ifndef __MOTOR_H__
#define __MOTOR_H__

//#define TWO_TWO_STEPS	//��ת:AB��BC��CD��DA��ʽ
						//��ת:AB��DA��CD��BC��ʽ
#define ONE_TWO_STEPS	//1-2����Ʒ�ʽ
//#define KINGST_BOARD	//�������·����

#define MOTOR_ONE_STEP_ANG			5.625	//������������Ƕ�
#define MOTOR_UNREACHABLE_OFFSET	32.5	//���䷧�޷������λ�ýǶ�

extern bit Flag_StepCntFwd;			//��ת�������λ
extern bit Flag_StepCntRev;			//��ת�������λ
extern bit Flag_StepCntFwdNoLimit;	//������ת�������λ
extern bit Flag_StepCntRevNoLimit;	//���޷�ת�������λ 
extern LONG beats;  				//���ת����������

void StartMotorAng(LONG angle);
void StartMotorStepFwd(LONG steps);
void StartMotorStepRev(LONG steps);
void StopMotor(void);
void TurnMotor(void);

#endif