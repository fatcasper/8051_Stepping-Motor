#include "sys_type.h"
#include "mcr_bit.h"
#include "key.h"
#include "motor.h"

#ifdef TWO_TWO_STEPS
#define KEY_PRESS_CNT 70		//������ת�������ת����
#elif defined ONE_TWO_STEPS
//#define KEY_PRESS_CNT (((360 - MOTOR_UNREACHABLE_OFFSET) / MOTOR_ONE_STEP_ANG) * 64)	//������ת�������ת����(124��)
#define KEY_PRESS_CNT	115
#endif

bit Flag_StallFwd = 0;	//��ת��ת�������λ
bit Flag_StallRev = 0;	//��ת��ת�������λ
#ifdef AUTO_STALL_STOP
bit Flag_StopNoLimit = 0;
bit Flag_Stop = 0;
#endif
static UCHAR keyCntFwd = 0;	//���򰴼���������
static UCHAR keyCntRev = 0;	//���򰴼���������

UCHAR code KeyCodeMap[4][4] = { //���󰴼���ŵ���׼���̼����ӳ���
	{ 0x31, 0x32, 0x33, 0x26 },	//���ּ�1�����ּ�2�����ּ�3�����ϼ�
	{ 0x34, 0x35, 0x36, 0x25 }, //���ּ�4�����ּ�5�����ּ�6�������
	{ 0x37, 0x38, 0x39, 0x28 }, //���ּ�7�����ּ�8�����ּ�9�����¼�
	{ 0x30, 0x1B, 0x0D, 0x27 }  //���ּ�0��ESC����  �س����� ���Ҽ�
};

UCHAR KeySta[4][4] = {//ȫ�����󰴼��ĵ�ǰ״̬
	{1, 1, 1, 1},
	{1, 1, 1, 1},
	{1, 1, 1, 1},
	{1, 1, 1, 1}
};

/* �����������������ݼ���ִ����Ӧ�Ĳ�����keycode-�������� */
void KeyAction(UCHAR keycode)
{	
	if((keycode>=0x30) && (keycode<=0x39)){  //���Ƶ��ת��1-9Ȧ
		;
	}else if(keycode == 0x26){//���ϼ�������ת������Ϊ��ת
		Flag_StepCntFwdNoLimit = 1;
		StartMotorAng(0xFF);
	}else if(keycode == 0x28){//���¼�������ת������Ϊ��ת
		Flag_StepCntRevNoLimit = 1;
		StartMotorAng(-0xFF);
	}else if(keycode == 0x25){//��������̶���ת30��
		Flag_StepCntFwd = 1;
		keyCntFwd++;
		if(keyCntFwd >= KEY_PRESS_CNT){
			keyCntFwd = 0;
			keyCntRev = 0;
			Flag_StallFwd = 1;
		}
		StartMotorStepFwd(30);	//������ת64�������ٱ�1/64��Լ11.25�㣬32������
	}else if(keycode == 0x27){//���Ҽ����̶���ת30��
		Flag_StepCntRev = 1;
		keyCntRev++;
		if(keyCntRev >= KEY_PRESS_CNT){
			keyCntRev = 0;
			keyCntFwd = 0;
			Flag_StallRev = 1;
		}
		StartMotorStepRev(-30);	//������ת64�������ٱ�1/64��Լ11.25�㣬32������
	}else if(keycode == 0x1B){//Esc����ֹͣת��
		StopMotor();
	}
}

/* ����������������ⰴ��������������Ӧ����������������ѭ���е��� */
void KeyDriver(void)
{
	UCHAR i, j;

	static UCHAR backup[4][4] = {//����ֵ���ݣ�����ǰһ�ε�ֵ
		{1, 1, 1, 1},  
		{1, 1, 1, 1},  
		{1, 1, 1, 1}, 
		{1, 1, 1, 1}
	};
	
	for(i=0; i<4; i++){//ѭ�����4*4�ľ��󰴼�
		for(j=0; j<4; j++){
			if(backup[i][j] != KeySta[i][j]){//��ⰴ������
				if (backup[i][j] != 0){//��������ʱִ�ж���
					KeyAction(KeyCodeMap[i][j]);	//���ð�����������
				}
			backup[i][j] = KeySta[i][j];	//ˢ��ǰһ�εı���ֵ
			}
		}
	}
}

/* ����ɨ�躯�������ڶ�ʱ�ж��е��ã��Ƽ����ü��1ms */
void KeyScan(void)
{
    UCHAR i;
	
	
	static UCHAR keyout = 0;   //���󰴼�ɨ���������
	static UCHAR keybuf[4][4] = {//���󰴼�ɨ�軺����
		   {0xFF, 0xFF, 0xFF, 0xFF},  
		   {0xFF, 0xFF, 0xFF, 0xFF},
		   {0xFF, 0xFF, 0xFF, 0xFF}, 
		   {0xFF, 0xFF, 0xFF, 0xFF}
	};

    //��һ�е�4������ֵ���뻺����
    keybuf[keyout][0] = (keybuf[keyout][0] << 1) | KEY_IN_1;
    keybuf[keyout][1] = (keybuf[keyout][1] << 1) | KEY_IN_2;
    keybuf[keyout][2] = (keybuf[keyout][2] << 1) | KEY_IN_3;
    keybuf[keyout][3] = (keybuf[keyout][3] << 1) | KEY_IN_4;
    //��������°���״̬
    for (i=0; i<4; i++){//ÿ��4������������ѭ��4��
			if((keybuf[keyout][i] & 0x0F) == 0x00){//����4��ɨ��ֵΪ0����4*4ms�ڶ��ǰ���״̬ʱ������Ϊ�������ȶ��İ���
				KeySta[keyout][i] = 0;
			}else if ((keybuf[keyout][i] & 0x0F) == 0x0F){//����4��ɨ��ֵΪ1����4*4ms�ڶ��ǵ���״̬ʱ������Ϊ�������ȶ��ĵ���
				KeySta[keyout][i] = 1;
			}
    }
    //ִ����һ�ε�ɨ�����
    keyout++;	//�����������
    keyout = keyout & 0x03;  //����ֵ�ӵ�4������
    switch (keyout){         //�����������ͷŵ�ǰ������ţ������´ε��������
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
