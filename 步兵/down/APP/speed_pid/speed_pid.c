#include "speed_pid.h"
#include "motor.h"
#include "Kinematic.h"

#define IntegralUpperLimit    5000    //���̿����ֱ���ֵ
#define Integralgimbal        500     //��̨���ֱ���ֵ
#define IntegralSeparation    300     //���̻��ַ���
#define vPID_OUT_MAX          8000		//��������
#define gPID_OUT_MAX          30000   //��̨pid����޷�
#define gimbal_angel_upperlimit  500  //��̨�����ֱ���ֵ
#define gimbal_angel_downlimit   20   //��̨���ַ���
#define tvPID_OUT_MAX         9000    //������

int pid_flag_start=1;
int pid_flag_end=0;

int vpid_out_max=vPID_OUT_MAX;
int find_max(void);

// ����: VPID_Init()
// ����: ���ת��pid������ʼ��
// �������ٶȲ����ṹ��
// �������
// �ڲ��������û��������
void VPID_Init(VPID_t *vpid)
{
	vpid->target_speed=0;
	vpid->actual_speed=0;
	vpid->err=0;
	vpid->last_err=0;
	vpid->err_integration=0;
	vpid->P_OUT=0;
	vpid->I_OUT=0;
	vpid->PID_OUT=0;
}

// ����: VPID_Init_All()
// ����: ��..������ٶȳ�ʼ��
// ��������
// �������
void VPID_Init_All()	
{
	VPID_Init(&motor1.vpid);
	VPID_Init(&motor2.vpid);
	VPID_Init(&motor3.vpid);
	VPID_Init(&motor4.vpid);
	VPID_Init(&motor5.vpid);
}


// ����: vpid_realize()
// ����: �ٶ�pid����
// �������ٶ�pid�����ṹ�壬�ٶ�pid��p��i��΢�����Ҫ
// �������
// �ڲ��������û��������
void vpid_realize(VPID_t *vpid,float kp,float ki)
{
		vpid->err = vpid->target_speed - vpid->actual_speed;
		
	if(vpid->err==0)
	{
   vpid->err_integration=0;
	}
	if(abs(vpid->err) <= IntegralSeparation)		//���ַ���
			vpid->err_integration += vpid->err;
	if(vpid->err_integration > IntegralUpperLimit)		//�����ֱ���
		vpid->err_integration = IntegralUpperLimit;
	else if(vpid->err_integration < -IntegralUpperLimit)
		vpid->err_integration = -Integralgimbal;
	
	vpid->P_OUT = kp * vpid->err;								//P��
	vpid->I_OUT = ki * vpid->err_integration;		//I��
	
	//����޷�
	if((vpid->P_OUT + vpid->I_OUT )> vPID_OUT_MAX) 
		vpid->PID_OUT = vPID_OUT_MAX;
	else if((vpid->P_OUT + vpid->I_OUT ) < -vPID_OUT_MAX) 
		vpid->PID_OUT = -vPID_OUT_MAX;
	else
		vpid->PID_OUT = vpid->P_OUT + vpid->I_OUT;
}


// ����: vpid_PI_realize()
// ����: ���ת��pidʵ��
// ���������ת��pid��p��i��΢�����Ҫ
// �������
void vpid_PI_realize(float kp,float ki)
{
	//��ȡ�����ǰת��
	motor1.vpid.actual_speed = motor1.actual_speed;
	motor2.vpid.actual_speed = motor2.actual_speed;
	motor3.vpid.actual_speed = motor3.actual_speed;
	motor4.vpid.actual_speed = motor4.actual_speed;
	

	//�������ֵ
	vpid_realize(&motor1.vpid,kp,ki);
	vpid_realize(&motor2.vpid,kp,ki);
	vpid_realize(&motor3.vpid,kp,ki);
	vpid_realize(&motor4.vpid,kp,ki);
	
	/******************���ʿ��Ʒ���*************************/

/*	power_limitation_jugement();
	power_limitation_coefficient();*/
	
}

void tvpid_realize(VPID_t *vpid,float kp,float ki)
{
	vpid->err = vpid->target_speed - vpid->actual_speed;
	
	if(abs(vpid->err) <= IntegralSeparation)		//���ַ���
		vpid->err_integration += vpid->err;
	if(vpid->err_integration > IntegralUpperLimit)		//�����ֱ���
		vpid->err_integration = IntegralUpperLimit;
	else if(vpid->err_integration < -IntegralUpperLimit)
		vpid->err_integration = -IntegralUpperLimit;
	
	vpid->P_OUT = kp * vpid->err;								//P��
	vpid->I_OUT = ki * vpid->err_integration;		//I��
	
	//����޷�
	if((vpid->P_OUT + vpid->I_OUT) > tvPID_OUT_MAX) 
		vpid->PID_OUT = tvPID_OUT_MAX;
	else if((vpid->P_OUT + vpid->I_OUT) < -tvPID_OUT_MAX) 
		vpid->PID_OUT = -tvPID_OUT_MAX;
	else
		vpid->PID_OUT = vpid->P_OUT + vpid->I_OUT;
}
void tvpid_PI_realize(float kp,float ki)
{
	//��ȡ�����ǰת��
	
	motor5.vpid.actual_speed = motor5.actual_speed;
	

	//�������ֵ
	vpid_realize(&motor5.vpid,kp,ki);

	
	
	
}

// ����: set_motor_speed()
// ����: ����Ŀ���ٶ�
// ������4�������Ŀ���ٶ�
// �������
void set_chassis_motor_speed(int motor1_speed,int motor2_speed,int motor3_speed,int motor4_speed)
{
	motor1.vpid.target_speed = motor1_speed;		//��Ϊ��������෴  ���ԼӸ���
	motor2.vpid.target_speed = motor2_speed;
	motor3.vpid.target_speed = motor3_speed;
	motor4.vpid.target_speed = motor4_speed;
	
	
	
	motor1.target_speed = motor1_speed;		//��Ϊ��������෴  ���ԼӸ���
	motor2.target_speed = motor2_speed;
	motor3.target_speed = motor3_speed;
	motor4.target_speed = motor4_speed;
	
}

void set_GIMBAL_angle(int gimbal1_speed,int gimbal2_speed)
{
	gimbal1.apid.target_speed = gimbal1_speed;  //angel���ٶ�
	//gimbal1.apid.target_angle = gimbal1_angle;
}

void set_trigger_motor_speed(int motor5_speed)
{
	motor5.vpid.target_speed = motor5_speed;
	
	motor5.target_speed = motor5_speed;	
	

}
void set_gimbal1_motor_speed(int gimbal1_speed)
  {
	gimbal1.vpid.target_speed = gimbal1_speed;
	gimbal1.target_speed = gimbal1_speed;		
	
	}


/*********************************************��̨pid����*******************************************************/
void apid_GIMBAL_realize(APID_t *vpid,float kpa,float kia,float kda)
{
	vpid->err = vpid->target_speed - vpid->actual_speed;	
	/*if(vpid->err==0)
	{
   vpid->err_integration=0;
	}*/
	//if(abs(vpid->err) <= gimbal_angel_downlimit)		//���ַ���
		//	vpid->err_integration += vpid->err;
//	if(vpid->err_integration > gimbal_angel_upperlimit)		//�����ֱ���
	//	vpid->err_integration = Integralgimbal;
	//else if(vpid->err_integration < -gimbal_angel_upperlimit)
		//vpid->err_integration = -Integralgimbal;
	
	vpid->P_OUT = kpa * vpid->err;								//P��
	vpid->I_OUT = kia * vpid->err_integration;		//I��
	vpid->D_OUT = kda * (vpid->err-vpid->last_err);//D��
	vpid->last_err=vpid->err;
	//����޷�
	if(abs(vpid->err) <= 2)
	vpid->PID_OUT=0;
	else
	{	
	if((vpid->P_OUT + vpid->I_OUT + vpid->D_OUT)> gPID_OUT_MAX) 
		vpid->PID_OUT = gPID_OUT_MAX;
	else if((vpid->P_OUT + vpid->I_OUT + vpid->D_OUT) < -gPID_OUT_MAX) 
		vpid->PID_OUT = -gPID_OUT_MAX;
	else
		vpid->PID_OUT = vpid->P_OUT + vpid->I_OUT + vpid->D_OUT;
  }
  
}

void apid_GIMBAL_PI_realize(float kpa,float kia,float kda)
{
	//��ȡ�����ǰת��
	gimbal1.apid.actual_speed = gimbal1.actual_speed;
	gimbal1.apid.actual_angle = gimbal1.actual_angle;
	//�������ֵ
	apid_GIMBAL_realize(&gimbal1.apid,kpa,kia,kda);
}

/*************************************************��̨pid����******************************************************/

// ����: abs()
// ����: �Զ���������ֵ��������Ϊmath.h��Ĳ�����
// ������input
// �����|input|
int abs(int input)
{
	if(input<0)
		input = -input;
	return input;
}

int pid_auto(void)
{
	int a=0;
   if(gimbal1.actual_angle>=2048&&gimbal1.actual_angle<=6144&&pid_flag_start)
	 {
	   a=60;
	 }
	 if(gimbal1.actual_angle>=2048&&gimbal1.actual_angle<=6144&&pid_flag_end)
	 {
		 a=-60;
	 }
   if(gimbal1.actual_angle<2048&&gimbal1.actual_angle>2008)
	 {
	 a=0;
	 }
	 if(gimbal1.actual_angle<6184&&gimbal1.actual_angle>6144)
	 {
	 a=0;
	 }

	 if(gimbal1.actual_angle>=6184)
	 {
	   a=0;
		 pid_flag_start=0;
		 pid_flag_end=1;
	 }
	 if(gimbal1.actual_angle<=2008)
	 {
	  a=0;
		pid_flag_start=1;
		pid_flag_end=0;
	 } 
	 return a;
}

int pid_pc(void)
{
	int a = 0;
/*if(gimbal1.actual_angle>=2730&&gimbal1.actual_angle<=5462&&pid_flag_start)
{
	a=5462-gimbal1.actual_angle;
}
if(gimbal1.actual_angle<=5462&&gimbal1.actual_angle>=2730&&pid_flag_end)
{
	a=2730-gimbal1.actual_angle;
}
if(gimbal1.actual_angle>5470)
{
	a=0;
	pid_flag_start=0;
  pid_flag_end=1;
}
if(gimbal1.actual_angle<2720)
{
	a=0;
	pid_flag_start=1;
  pid_flag_start=0;
}
else
{
a=0;
}*/
a=4096-gimbal1.actual_angle;
a=a*0.0347624*3;
  return a;
}
