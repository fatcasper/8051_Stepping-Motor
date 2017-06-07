/********************************************************************************
* Name: main.c
* Description:stepping motor estimate
* Version: v1.0.0
* Author:Wei Birong
* Attribute: Modify
********************************************************************************/

#include <reg52.h>
#include "mcr_bit.h"
#include "sys_type.h"
#include "key.h"
#include "intr.h"

void main(void)
{
	intr_init();	//��ʼ���ж�
	
	while(1){
		KeyDriver();   //���ð�����������
	}
}

