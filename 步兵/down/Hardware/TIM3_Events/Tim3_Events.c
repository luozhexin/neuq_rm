
/*****    ���ļ�ר��������TIM3��ʱ���ж������еĺ���   ******/
#include <string.h>
#include <stdio.h>

#include "Tim3_Events.h"
#include "key.h"
#include "led.h"
#include "bsp_debug_usart.h"
#include "bsp_uart7.h"
#include "fric.h"
#include "motor.h"
#include "kinematic.h"
#include "remote_code.h"
#include "angle_pid.h"
#include "speed_pid.h"
#include "stm32f4xx_tim.h"
#include "gimbal.h"
#include "json.h"
#include "kinematic.h"
//����Ҫ���͵�����


 //Pid_parameter Chassis,Gimbal,Trigger;
float kp=580,ki=0,kd=0.1;//kp=700����,ki=0.03,kd=0.1;
int pid_target_speed=0;
int pid_target_angle=4096;
//�����궨��   ��ǿ����ɶ���
#define pressed     0
#define unpressed   1

// ����: Robo_Control()
// ����: �������˶����������������pid��ң�����źŽ���
// ��������
// �������
void Robo_Move()
{
	if(stop_flag_1 && ap_pid_flag == ang_pid)			//�����ʱ�ٶ�Ϊ0��ֹͣ�������Զ�����ʱû��������λ�ñջ�   ��ô�Ƕȱջ�
	{	
		break_jugement();
		if(stop_flag_3 && 1)
		{
			stop_chassis_motor();
		}
		apid_PID_realize(0.2,0.05,0);			
	
	}
  if(/*(Control_Mode) ==*/ 1 /*0x03*/)//((Control_Mode & auto_control) == auto_control)
	{
		pid_target_speed =pid_pc();
		speed_control(Kinematics.target_velocities.linear_x, Kinematics.target_velocities.linear_y, Kinematics.target_velocities.angular_z);		
		gimbal_control(pid_target_speed/*Kinematics.target_angular.gimbal_angular.yaw_angular*/,Kinematics.target_angular.gimbal_angular.pitch_angular);
		//auto_fire();//��λ�����ͱ�־λ  ����
	}
 
	
   	vpid_PI_realize(2,0.05);			//�ٶȱջ�2  0.05
	  tvpid_PI_realize(2.5,0.05);      //�������ٶȱջ�  ����δȷ��   2.5  0.05
	  apid_GIMBAL_PI_realize(kp,ki,kd);     //
		set_chassis_current();		//�趨�������
	  set_trigger_current();
	  set_gimbal_current();

	  TIM_SetCompare1(TIM1,pwm_pulse1);
		TIM_SetCompare2(TIM1,pwm_pulse2);

	  
}
union {float fvalue;char data[4];}tmp1;
//������Ϣת����ʮ������������ȥ
void send_chassis_info_by_hex(void)//**************************δ�õ�
{
	char str[17];
	str[0] = 0x55;
	tmp1.fvalue = Kinematics.actual_velocities.linear_x;
	str[1] = tmp1.data[0];
	str[2] = tmp1.data[1];
	str[3] = tmp1.data[2];
	str[4] = tmp1.data[3];
	tmp1.fvalue = Kinematics.actual_velocities.linear_y;
	str[5] = tmp1.data[0];
	str[6] = tmp1.data[1];
	str[7] = tmp1.data[2];
	str[8] = tmp1.data[3];
	tmp1.fvalue = Kinematics.actual_velocities.angular_z;
	str[9] = tmp1.data[0];
	str[10] = tmp1.data[1];
	str[11] = tmp1.data[2];
	str[12] = tmp1.data[3];
	
	
//	memcpy(&str[1],&Kinematics.target_velocities.linear_x,4);
//	memcpy(&str[5],&Kinematics.target_velocities.linear_y,4);
//	memcpy(&str[9],&Kinematics.target_velocities.angular_z,4);
	str[13] = 0x56;
	str[14] = '\r';
	str[15] = '\n';
	for(char i=0;i<16;i++)
		Usart_SendByte( JSON_USART, str[i]);
	
	
};



// ����: Debug_Key()
// ����: �����õİ���������Ƭ���ϵİ�ɫ����
// ��������
// �������
void Debug_Key()
{
	static int key_flag = unpressed;		//���ڿ��������ж��ڰ��µĹ�����ֻ����һ��
	if( key_press() && key_flag == unpressed)		//�������������
	{
		LED4=!LED4;												//LED4��ת������״ָ̬ʾ
		key_flag = pressed;			          //����������
		// your codes below
		
		//NRF24L01_TxPacket(send_data);			//nrf�����ַ���
	}
	else if(!key_press())
		key_flag=unpressed;		//����δ������
}


// ����: Wireless_Tune_Para()
// ����: ���ߵ��Σ���������δд
// ��������
// �������
void Wireless_Tune_Para()
{
	LED3=!LED3;				//ÿ���յ�һ֡����LED3״̬��ת������״ָ̬ʾ����
	
	//�����յ�������receive_data
	
}

