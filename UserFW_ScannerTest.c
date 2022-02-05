/*
 * UserFW_ScannerTest.c
 *
 *  Created on: 2021. 4. 28.
 *      Author: Kim
 */
#include "UserFW_ScannerTest.h"
#include <math.h>
#include <stdlib.h>
#include "UserFW_FPGA_CmdRegister.h"
#include "UserFW_FPGA_StatusRegister.h"

int Correction_Count = 0;
int abcd = 0;


void Scanner_Homing(){
	fpga_cmd0.bit.STG_Homing_G = 1;
	*((volatile uint16_t *)FPGA_BASE + 32) = fpga_cmd0.all;
}

void Scanner_Init(_Scanner_Control* in)
{
	//Scanner_Homing();

	in->Scan_x_scale_mm = 897.739;
	in->Scan_y_scale_mm = 829.556;

	in->Re_Scan_x_scale_mm = (1 / in->Scan_x_scale_mm);
	in->Re_Scan_y_scale_mm = (1 / in->Scan_y_scale_mm);

	in->Scan_stepsize = 1;

	in->Flag = 0;

	in->height = 0;
	in->height_ratio = 0;

	in->Scan_x = 0;
	in->Scan_y = 0;
	in->Scan_z = 0;

	in->Scan_x_dist = 0;
	in->Scan_y_dist = 0;
	in->Scan_xy_dist = 0;

	in->Scan_x_dist_virtual = 0;
	in->Scan_y_dist_virtual = 0;

	in->Scan_x_temp = 0;
	in->Scan_y_temp = 0;
	in->Scan_z_temp = 0;

	in->Scan_x_trans = 0;
	in->Scan_y_trans = 0;
	in->Scan_z_trans = 0;

	in->Angle = 0;
	in->Radius = 1;  // 원래는 1
	in->Scan_x_wobble = 0;  // 원래는 0
	in->Scan_y_wobble = 0;
	in->Scan_z_wobble = 0;

	in->direction = 0; // 기본값으로 반시계방향 설정.

	//in->Scan_radius_dist = in->Scan_radius * in->Scan_x_scale_mm; // 반지름에 대한 보정이 들어가야 하기에 일단 x에 대한 보정으로 맞춤.
}

void Scanner_Correction(_Scanner_Control* in)
{
	/*   기존꺼    */
//	in->height = abs((in->Scan_y_temp - 32767)/in->Scan_y_scale_mm);
//	in->width = abs((in->Scan_x_temp - 32767)/in->Scan_x_scale_mm);

	/*  수정본  */
	in->height = abs((in->Scan_y_temp - 32767) * in->Re_Scan_y_scale_mm);
	in->width = abs((in->Scan_x_temp - 32767) * in->Re_Scan_x_scale_mm);


	/* 입력된 mm 단위 * 변환 단위를 곱하여 스캐너가 이동할 목표값을 생성 */
	in->Scan_x_dist = in->Scan_x * in->Scan_x_scale_mm;
	in->Scan_y_dist = in->Scan_y * in->Scan_y_scale_mm;
	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/*	if(in->height <= 5)
	{
		in->height_ratio = in->height / 5;
		in->Scan_x_dist_virtual = in->Scan_x_dist * 0.9499 * (1-in->height_ratio) + in->Scan_x_dist * 0.9515 * in->height_ratio;
		HAL_GPIO_WritePin(GPIOF, GPIO_PIN_10, GPIO_PIN_SET);
	}
	else if(in->height <= 10)
	{
		in->height_ratio = (in->height - 5) / 5;
		in->Scan_x_dist_virtual = in->Scan_x_dist * 0.9515 * (1-in->height_ratio) + in->Scan_x_dist * 0.9532 * in->height_ratio;
	}
	else if(in->height <= 15)
	{
		in->height_ratio = (in->height - 10) / 5;
		in->Scan_x_dist_virtual = in->Scan_x_dist * 0.9532 * (1-in->height_ratio) + in->Scan_x_dist * 0.9581 * in->height_ratio;
	}
	else if(in->height <= 20)
	{
		in->height_ratio = (in->height - 15) / 5;
		in->Scan_x_dist_virtual = in->Scan_x_dist * 0.9581 * (1-in->height_ratio) + in->Scan_x_dist * 0.9632 * in->height_ratio;
	}
	else if(in->height <= 25)
	{
		in->height_ratio = (in->height - 20) / 5;
		in->Scan_x_dist_virtual = in->Scan_x_dist * 0.9632 * (1-in->height_ratio) + in->Scan_x_dist * 0.9734 * in->height_ratio;
	}
	else if(in->height <= 30)
	{
		in->height_ratio = (in->height - 25) / 5;
		in->Scan_x_dist_virtual = in->Scan_x_dist * 0.9734 * (1-in->height_ratio) + in->Scan_x_dist * 0.9857 * in->height_ratio;
	}
	else if(in->height <= 35)
	{
		in->height_ratio = (in->height - 30) / 5;
		in->Scan_x_dist_virtual = in->Scan_x_dist * 0.9857 * (1-in->height_ratio) + in->Scan_x_dist * 1 * in->height_ratio;
		HAL_GPIO_WritePin(GPIOF, GPIO_PIN_10, GPIO_PIN_RESET);
	}
	else // in->height > 35
	{
		in->Scan_x_dist_virtual = in->Scan_x_dist;
	}  */

	/////////////////////////// 수정중인부분 ///////////////////////////////
	switch(Correction_Count){
	case 0 :
		if(!(in->height <= 5)){
			Correction_Count = 1;
		}
		in->height_ratio = in->height / 5;
		in->Scan_x_dist_virtual = in->Scan_x_dist * 0.9499 * (1-in->height_ratio) + in->Scan_x_dist * 0.9515 * in->height_ratio;
		HAL_GPIO_WritePin(GPIOF, GPIO_PIN_10, GPIO_PIN_SET);
		break;
	case 1 :
		if(!(in->height <= 10)){
			Correction_Count = 2;
		}
		in->height_ratio = (in->height - 5) / 5;
		in->Scan_x_dist_virtual = in->Scan_x_dist * 0.9515 * (1-in->height_ratio) + in->Scan_x_dist * 0.9532 * in->height_ratio;
		break;
	case 2 :
		if(!(in->height <= 15)){
			Correction_Count = 3;
		}
		in->height_ratio = (in->height - 10) / 5;
		in->Scan_x_dist_virtual = in->Scan_x_dist * 0.9532 * (1-in->height_ratio) + in->Scan_x_dist * 0.9581 * in->height_ratio;
		break;
	case 3 :
		if(!(in->height <= 20)){
			Correction_Count = 4;
		}
		in->height_ratio = (in->height - 15) / 5;
		in->Scan_x_dist_virtual = in->Scan_x_dist * 0.9581 * (1-in->height_ratio) + in->Scan_x_dist * 0.9632 * in->height_ratio;
		break;
	case 4 :
		if(!(in->height <= 25)){
			Correction_Count = 5;
		}
		in->height_ratio = (in->height - 20) / 5;
		in->Scan_x_dist_virtual = in->Scan_x_dist * 0.9632 * (1-in->height_ratio) + in->Scan_x_dist * 0.9734 * in->height_ratio;
		break;
	case 5 :
		if(!(in->height <= 30)){
			Correction_Count = 6;
		}
		in->height_ratio = (in->height - 25) / 5;
		in->Scan_x_dist_virtual = in->Scan_x_dist * 0.9734 * (1-in->height_ratio) + in->Scan_x_dist * 0.9857 * in->height_ratio;
		break;
	case 6 :
		if(!(in->height <= 35)){
			Correction_Count = 7;
		}
		in->height_ratio = (in->height - 30) / 5;
		in->Scan_x_dist_virtual = in->Scan_x_dist * 0.9857 * (1-in->height_ratio) + in->Scan_x_dist * 1 * in->height_ratio;
		HAL_GPIO_WritePin(GPIOF, GPIO_PIN_10, GPIO_PIN_RESET);
		break;
	case 7 :
		in->Scan_x_dist_virtual = in->Scan_x_dist;
		break;
	default :
		break;
	}

	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	if(in->width <= 5)
	{
		in->width_ratio = in->width / 5;
		in->Scan_y_dist_virtual = in->Scan_y_dist;
	}
	else if(in->width <= 10)
	{
		in->width_ratio = (in->width - 5) / 5;
		in->Scan_y_dist_virtual = in->Scan_y_dist * 1 * (1-in->width_ratio) + in->Scan_y_dist * 0.998507503 * in->width_ratio;
	}
	else if(in->width <= 15)
	{
		in->width_ratio = (in->width - 10) / 5;
		in->Scan_y_dist_virtual = in->Scan_y_dist * 0.998507503 * (1-in->width_ratio) + in->Scan_y_dist * 0.997023054 * in->width_ratio;
	}
	else if(in->width <= 20)
	{
		in->width_ratio = (in->width - 15) / 5;
		in->Scan_y_dist_virtual = in->Scan_y_dist * 0.997023054 * (1-in->width_ratio) + in->Scan_y_dist * 0.995535836 * in->width_ratio;
	}
	else if(in->width <= 25)
	{
		in->width_ratio = (in->width - 20) / 5;
		in->Scan_y_dist_virtual = in->Scan_y_dist * 0.995535836 * (1-in->width_ratio) + in->Scan_y_dist * 0.994799255 * in->width_ratio;
	}
	else if(in->width <= 30)
	{
		in->width_ratio = (in->width - 25) / 5;
		in->Scan_y_dist_virtual = in->Scan_y_dist * 0.994799255 * (1-in->width_ratio) + in->Scan_y_dist * 0.992585354 * in->width_ratio;
	}
	else if(in->width <= 35)
	{
		in->width_ratio = (in->width - 30) / 5;
		in->Scan_y_dist_virtual = in->Scan_y_dist * 0.992585354 * (1-in->width_ratio) + in->Scan_y_dist * 0.991111351 * in->width_ratio;
	}
	else // in->width < 35
	{
		in->Scan_y_dist_virtual = in->Scan_y_dist * 0.991111351;
	}
}

void Scanner_Line_Draw(_Scanner_Control* in)
{
	if(in->Scan_x_dist_virtual != in->Scan_x_temp - 32767)
	{
		if(in->Scan_x_dist_virtual > in->Scan_x_temp - 32767)
		{
			in->Scan_x_trans += (int)in->Scan_stepsize;
			in->Scan_x_temp = in->Scan_x_trans + 32767;
		}
		else
		{
			in->Scan_x_trans -= (int)in->Scan_stepsize;
			in->Scan_x_temp = in->Scan_x_trans + 32767;
		}
	}

	if(in->Scan_y_dist_virtual != in->Scan_y_temp - 32767)
	{
		if(in->Scan_y_dist_virtual > in->Scan_y_temp - 32767)
		{
			in->Scan_y_trans += (int)in->Scan_stepsize;
			in->Scan_y_temp = in->Scan_y_trans + 32767;
		}
		else
		{
			in->Scan_y_trans -= (int)in->Scan_stepsize;
			in->Scan_y_temp = in->Scan_y_trans + 32767;
		}
	}


	if((((in->Scan_x_dist_virtual + 32767 - in->Scan_stepsize) <= in->Scan_x_temp) &&
			((in->Scan_x_dist_virtual + 32767 + in->Scan_stepsize) >= in->Scan_x_temp)) &&
			(((in->Scan_y_dist_virtual + 32767 - in->Scan_stepsize) <= in->Scan_y_temp) &&
					((in->Scan_y_dist_virtual + 32767 + in->Scan_stepsize) >= in->Scan_y_temp)))
		in->Flag = 0;
}

void Scanner_Wobble_Circle(_Scanner_Control* in)
{
	in->Scan_x_wobble = in->Scan_x_temp + in->Radius * in->Scan_x_scale_mm * cos(in->Angle);
	in->Scan_y_wobble = in->Scan_y_temp + in->Radius * in->Scan_y_scale_mm * sin(in->Angle);
	in->Angle = in->Angle + 0.2;
	if(in->Angle >= 360)
		in->Angle = 0;
}


/*
void Scanner_Circle_Draw(_Scanner_Control* in){
	for(int angle = 0;angle<= 360;angle++){
		int input_X = cos(in->Angle) * in->Scan_radius_dist;
		int input_Y = sin(in->Angle) * in->Scan_radius_dist;

		int Set_Flag = 1;
		/////////////////////////////////////////////////////////////////////////////
		while(Set_Flag != 0){
		if(input_X != in->Scan_x_temp - 32767)
			{
				if(input_X > in->Scan_x_temp - 32767)
				{
					in->Scan_x_trans += (int)in->Scan_stepsize;
					in->Scan_x_temp = in->Scan_x_trans + 32767;
				}
				else
				{
					in->Scan_x_trans -= (int)in->Scan_stepsize;
					in->Scan_x_temp = in->Scan_x_trans + 32767;
				}
			}

			if(input_Y != in->Scan_y_temp - 32767)
			{
				if(input_Y > in->Scan_y_temp - 32767)
				{
					in->Scan_y_trans += (int)in->Scan_stepsize;
					in->Scan_y_temp = in->Scan_y_trans + 32767;
				}
				else
				{
					in->Scan_y_trans -= (int)in->Scan_stepsize;
					in->Scan_y_temp = in->Scan_y_trans + 32767;
				}
			}


			if((((input_X + 32767 - in->Scan_stepsize) <= in->Scan_x_temp) &&
					((input_X + 32767 + in->Scan_stepsize) >= in->Scan_x_temp)) &&
					(((input_Y + 32767 - in->Scan_stepsize) <= in->Scan_y_temp) &&
							((input_Y + 32767 + in->Scan_stepsize) >= in->Scan_y_temp))){
				Set_Flag = 0;
				abcd++;
			}
		}
	}

}    */

// 테스트용 함수
void Center_Wobble(_Scanner_Control* in)
{
	if(in->Scan_x_dist_virtual != in->Scan_x_temp - 32767)
	{
		if(in->Scan_x_dist_virtual > in->Scan_x_temp - 32767)
		{
			in->Scan_x_trans += (int)in->Scan_stepsize;
			in->Scan_x_temp = in->Scan_x_trans + 32767;
		}
		else
		{
			in->Scan_x_trans -= (int)in->Scan_stepsize;
			in->Scan_x_temp = in->Scan_x_trans + 32767;
		}
	}

	if(in->Scan_y_dist_virtual != in->Scan_y_temp - 32767)
	{
		if(in->Scan_y_dist_virtual > in->Scan_y_temp - 32767)
		{
			in->Scan_y_trans += (int)in->Scan_stepsize;
			in->Scan_y_temp = in->Scan_y_trans + 32767;
		}
		else
		{
			in->Scan_y_trans -= (int)in->Scan_stepsize;
			in->Scan_y_temp = in->Scan_y_trans + 32767;
		}
	}
}
/*
	if(scan_x_dist != scan_x_temp)
	{
		if(scan_x_dist > scan_x_temp)
			//scan_x_temp += (scan_stepsize * scan_x_dist / scan_xy_dist * 0.829556);
			scan_x_temp += (int)scan_stepsize;
		else
			//scan_x_temp -= (scan_stepsize * scan_x_dist / scan_xy_dist * 0.829556);
			scan_x_temp -= (int)scan_stepsize;
	}

	if(scan_y_dist != scan_y_temp)
	{
		if(scan_y_dist > scan_y_temp)
			//scan_y_temp += (scan_stepsize * scan_y_dist / scan_xy_dist * 0.897739);
			scan_y_temp += (int)scan_stepsize;
		else
			//scan_y_temp -= (scan_stepsize * scan_y_dist / scan_xy_dist * 0.897739);
			scan_y_temp -= (int)scan_stepsize;
	}

	scan_x_dist = scan_x * scan_x_mm;
	scan_y_dist = scan_y * scan_y_mm;
	scan_xy_dist = (sqrt(scan_x_dist*scan_x_dist + scan_y_dist*scan_y_dist));
	scan_x_dist = scan_x * scan_x_mm;
	scan_y_dist = 32767 + scan_y * scan_y_mm;
 */
