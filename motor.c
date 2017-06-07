#include <reg52.h>
#include "sys_type.h"
#include "motor.h"
#include "key.h"
#include "intr.h"

bit Flag_StepCntFwd = 0;	//��ת�������λ
bit Flag_StepCntRev = 0;	//��ת�������λ
bit Flag_StepCntFwdNoLimit = 0;	//������ת�������λ
bit Flag_StepCntRevNoLimit = 0;	//���޷�ת�������λ
LONG beats = 0;	//���ת����������

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

/* �����������������angle-��ת���ĽǶ� */
void StartMotorAng(LONG angle)
{
	EA = 0;
#ifdef KINGST_BOARD
	beats = (angle * 4076) / 360;	//ʵ��Ϊ4076��ת��һȦ
#endif
#ifdef TWO_TWO_STEPS
	if(angle == 0xFF){//ʹ��0xFF���ò����������ѭ��
		beats = 0xFF;
	}else if(angle == (-0xFF)){//ʹ��-0xFF���ò����������ѭ��
		beats = -0xFF;
	}else{
		beats = (angle * 2048) / 360;	//����Ϊ2048��ת��һȦ
	}
#elif defined ONE_TWO_STEPS
	if(angle == 0xFF){//ʹ��0xFF���ò����������ѭ��
		beats = 0xFF;
	}else if(angle == (-0xFF)){//ʹ��-0xFF���ò����������ѭ��
		beats = -0xFF;
	}else{
		beats = (angle * 4096) / 360;	//����Ϊ4096��ת��һȦ
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
		Flag_StepCntRevNoLimit = 1;	//�趨�������꣬���޷�ת�������ʶ
		StartMotorAng(-0xFF);	//�趨�������꣬���޷�ת�������ʶ
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

/* �������ֹͣ���� */
void StopMotor(void)
{
	EA = 0;
	beats = 0;
	EA = 1;
}

/* ���ת�����ƺ��� */
void TurnMotor()
{
	UCHAR tmp;	//��ʱ����
 	static UCHAR index = 0;	//�����������
    
	if(beats){//��������Ϊ0�����һ����������
		if(beats > 0){//����������0ʱ��ת
			index++;	//��תʱ���������������
#ifdef ONE_TWO_STEPS
			index = index & 0x07; //��&����ʵ�ֵ�8����
#elif defined TWO_TWO_STEPS
			index = index & 0x0B; //��&����ʵ�ֵ�4����
#endif
			if(Flag_StepCntFwdNoLimit == 1 && beats == 0xFF){
				;
			}else{
				beats--;	//��תʱ���ļ����ݼ�
			}
		}else{//������С��0ʱ��ת
			index++;	//��תʱ���������������
#ifdef ONE_TWO_STEPS
			index = index & 0x07;	//��&����ͬ������ʵ�ֵ�-1ʱ��7
#elif defined TWO_TWO_STEPS
			index = index & 0x0B;	//��&����ʵ�ֵ�4����
#endif
			if(Flag_StepCntRevNoLimit == 1 && beats == (-0xFF)){
				;
			}else{
				beats++;	//��תʱ���ļ����ݼ�
			}
		}
		tmp = P1;	//��tmp��P1�ڵ�ǰֵ�ݴ�
		tmp = tmp & 0xF0;	//��&���������4λ
		if(beats > 0){
			tmp = tmp | BeatCode[0][index]; //��|�����ѽ��Ĵ���д����4λ
		}else{
			tmp = tmp | BeatCode[1][index]; //��|�����ѽ��Ĵ���д����4λ
		}
		P1  = tmp;	//�ѵ�4λ�Ľ��Ĵ���͸�4λ��ԭֵ�ͻ�P1
	}else{//������Ϊ0��رյ�����е���
#ifdef KINGST_BOARD
	/*********************************
		��ɳ̲51 Developer Board
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