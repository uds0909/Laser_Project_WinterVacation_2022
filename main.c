/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2021 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "string.h"
#include "usb_device.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

#include "UserFW_FPGA_CmdRegister.h"
#include "UserFW_FPGA_StatusRegister.h"
#include "UserFW_ScannerTest.h"

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
#if defined ( __ICCARM__ ) /*!< IAR Compiler */

#pragma location=0x30000000
ETH_DMADescTypeDef  DMARxDscrTab[ETH_RX_DESC_CNT]; /* Ethernet Rx DMA Descriptors */
#pragma location=0x30000060
ETH_DMADescTypeDef  DMATxDscrTab[ETH_TX_DESC_CNT]; /* Ethernet Tx DMA Descriptors */
#pragma location=0x300000c0
uint8_t Rx_Buff[ETH_RX_DESC_CNT][ETH_MAX_PACKET_SIZE]; /* Ethernet Receive Buffers */

#elif defined ( __CC_ARM )  /* MDK ARM Compiler */

__attribute__((at(0x30000000))) ETH_DMADescTypeDef  DMARxDscrTab[ETH_RX_DESC_CNT]; /* Ethernet Rx DMA Descriptors */
__attribute__((at(0x30000060))) ETH_DMADescTypeDef  DMATxDscrTab[ETH_TX_DESC_CNT]; /* Ethernet Tx DMA Descriptors */
__attribute__((at(0x300000c0))) uint8_t Rx_Buff[ETH_RX_DESC_CNT][ETH_MAX_PACKET_SIZE]; /* Ethernet Receive Buffer */

#elif defined ( __GNUC__ ) /* GNU Compiler */

ETH_DMADescTypeDef DMARxDscrTab[ETH_RX_DESC_CNT] __attribute__((section(".RxDecripSection"))); /* Ethernet Rx DMA Descriptors */
ETH_DMADescTypeDef DMATxDscrTab[ETH_TX_DESC_CNT] __attribute__((section(".TxDecripSection")));   /* Ethernet Tx DMA Descriptors */
uint8_t Rx_Buff[ETH_RX_DESC_CNT][ETH_MAX_PACKET_SIZE] __attribute__((section(".RxArraySection"))); /* Ethernet Receive Buffers */

#endif

ETH_TxPacketConfig TxConfig;

DAC_HandleTypeDef hdac1;

ETH_HandleTypeDef heth;

RTC_HandleTypeDef hrtc;

SPI_HandleTypeDef hspi1;

TIM_HandleTypeDef htim1;
TIM_HandleTypeDef htim3;

UART_HandleTypeDef huart4;

SRAM_HandleTypeDef hsram2;
SRAM_HandleTypeDef hsram3;

/* USER CODE BEGIN PV */

//char Test_Gcode[3][64] = {
//		{0x00,0x01,0x00,0x00,0x14,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xff},
//		{0x00,0x01,0x00,0x00,0x14,0x00,0x00,0x00,0x14,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xff},
//		{0x00,0x01,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xff}
//};

//double PI = 3.14159265358979;
float input_X = 0;
float input_Y = 0;
char *Test_Gcode[3][64] = {
		{"0x00","0x01","0x00","0x00","0x14","0x00","0x00","0x00","0x00","0x00""0x00","0x00","0x00","0x00","0x00","0x00","0x00","0x00","0x00","0x00","0x00","0x00","0x00","0x00","0x00","0x00","0x00","0x00","0x00","0x00","0x00","0x00","0x00","0x00","0x00","0x00","0x00","0x00","0x00","0x00","0x00","0x00","0x00","0x00","0x00","0x00","0x00","0x00","0x00","0x00","0x00","0x00","0x00","0x00","0x00","0x00","0x00","0x00","0x00","0x00","0x00","0x00","0x00","0xff"},
		{"0x00","0x01","0x00","0x00","0x14","0x00","0x00","0x00","0x14","0x00""0x00","0x00","0x00","0x00","0x00","0x00","0x00","0x00","0x00","0x00","0x00","0x00","0x00","0x00","0x00","0x00","0x00","0x00","0x00","0x00","0x00","0x00","0x00","0x00","0x00","0x00","0x00","0x00","0x00","0x00","0x00","0x00","0x00","0x00","0x00","0x00","0x00","0x00","0x00","0x00","0x00","0x00","0x00","0x00","0x00","0x00","0x00","0x00","0x00","0x00","0x00","0x00","0x00","0xff"},
		{"0x00","0x01","0x00","0x00","0x00","0x00","0x00","0x00","0x00","0x00""0x00","0x00","0x00","0x00","0x00","0x00","0x00","0x00","0x00","0x00","0x00","0x00","0x00","0x00","0x00","0x00","0x00","0x00","0x00","0x00","0x00","0x00","0x00","0x00","0x00","0x00","0x00","0x00","0x00","0x00","0x00","0x00","0x00","0x00","0x00","0x00","0x00","0x00","0x00","0x00","0x00","0x00","0x00","0x00","0x00","0x00","0x00","0x00","0x00","0x00","0x00","0x00","0x00","0xff"}
};
char *Change_Gcode;

int Gcode_X = 0;
int Gcode_Y = 0;
int Gcode_Z = 0;

char Reset_Stage = 0;
char Limit_Check = 0;
char Test_Triangle = 0;
int Test_Count = 0;

char Laser_Flag = 0;
char Check = 0;
char Center_Wobble_Flag;

char Stage_Ctrl_Flag_Cmd = 0;
char Stage_Ctrl_Flag_Status = 0;
char Stage_X_Flag = 0;
char Stage_Y_Flag = 0;

uint16_t Stage_X_Pulse_in = 0;
uint16_t Stage_Y_Pulse_in = 0;

uint32_t tm3_cnt = 0, old_tm3_cnt = 0;
uint16_t fpga_version;

int16_t Enc_X_cnt;
int16_t Enc_Y_cnt;
int8_t Enc_X_rev;
int8_t Enc_Y_rev;

#define APP_RX_DATA_SIZE	64
#define APP_TX_DATA_SIZE	64
#define BUF_SIZE	1024	// 3640	// 3664	// 3600
int BufIndex = 0;
uint8_t buffer[4*BUF_SIZE];
uint8_t *bufferA=&buffer[0];
uint8_t oldRxBuf[64];

char USB_flag = 0;

extern uint8_t UserRxBufferFS[APP_RX_DATA_SIZE];
extern uint8_t UserTxBufferFS[APP_TX_DATA_SIZE];

int processtest, indextest;
_Scanner_Control Scanner_Control;

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_ETH_Init(void);
static void MX_FMC_Init(void);
static void MX_DAC1_Init(void);
static void MX_RTC_Init(void);
static void MX_SPI1_Init(void);
static void MX_TIM1_Init(void);
static void MX_TIM3_Init(void);
static void MX_UART4_Init(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */



/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{
  /* USER CODE BEGIN 1 */

  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_ETH_Init();
  MX_FMC_Init();
  MX_DAC1_Init();
  MX_RTC_Init();
  MX_SPI1_Init();
  MX_TIM1_Init();
  MX_TIM3_Init();
  MX_UART4_Init();
  MX_USB_DEVICE_Init();
  /* USER CODE BEGIN 2 */

 //test_sram_memory();
  HAL_DAC_Start(&hdac1, DAC_CHANNEL_1);
  HAL_DAC_Start(&hdac1, DAC_CHANNEL_2);


  do {		// 2020.11.19 doohee : Wait until FPGA ready !!
  	  fpga_version =  *((volatile uint16_t *)FPGA_BASE);
    } while((fpga_version < 0x1013) || (fpga_version > 0x1110 ));

    fpga_cmd0.bit.P_LED1 = 1;	// P_LED4 Off
    fpga_cmd0.bit.P_LED2 = 1;	// P_LED3 Off
    fpga_cmd0.bit.P_LED3 = 1;	// P_LED2 Off

    HAL_TIM_Base_Start_IT(&htim3);
    HAL_NVIC_EnableIRQ(SysTick_IRQn);
    __enable_irq();
    Scanner_Init(&Scanner_Control);

    // 초기 ?��치�?? 중앙?���???????? ?��치시?���???????? ?��?��
    Scanner_Control.Scan_x_temp = 32767;
    Scanner_Control.Scan_y_temp = 32767;


  /*  for(int i =0;i<3;i++){
    	strcat(Change_Gcode,(char *)Test_Gcode[i][4]);
    	strcat(Change_Gcode," ");
    	strcat(Change_Gcode,(char *)Test_Gcode[i][8]);
    	strcat(Change_Gcode," ");
    	strcat(Change_Gcode,(char *)Test_Gcode[i][12]);
    	strcat(Change_Gcode," ");
    }  /*


  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */

	  /*Pulse Test */
	  fpga_status2.all = *((volatile uint16_t *)FPGA_BASE + 2);
	  Internal_Cnt.all = *((volatile uint16_t *)FPGA_BASE + 6);
	  Pulse_From_FPGA.all = *((volatile uint16_t *)FPGA_BASE + 7);


	  // 나중에 값 바뀌었을 때만 넣어주도록 수정해
	  *((volatile uint16_t *)FPGA_BASE + 49) = ENC_Clk_Speed.all;


		/* Homing Check */
		if(fpga_status2.bit.STG_Homing == 1)
		{
			fpga_cmd0.bit.STG_Homing_G = 0;
			*((volatile uint16_t *)FPGA_BASE + 32) = fpga_cmd0.all;
			fpga_status2.bit.STG_Homing = 0;
			*((volatile uint16_t *)FPGA_BASE + 2) = fpga_status2.all;
		}

	  if(Reset_Stage == 1){
		  Scanner_Homing();
		  Reset_Stage = 0;
	  }


	  if(Stage_Ctrl_Flag_Cmd == 1)
	  {
	  	fpga_cmd0.bit.X_dir = Stage_X_Flag;
	  	fpga_cmd0.bit.Y_dir = Stage_Y_Flag;
	  	fpga_cmd0.bit.Stage_Ctrl = Stage_Ctrl_Flag_Cmd;
	  	*((volatile uint16_t *)FPGA_BASE + 32) = fpga_cmd0.all;

	  	Stage_X_Pulse.all = Stage_X_Pulse_in;
	  	Stage_Y_Pulse.all = Stage_Y_Pulse_in;
	  	*((volatile uint16_t *)FPGA_BASE + 47) = Stage_X_Pulse.all;
	  	*((volatile uint16_t *)FPGA_BASE + 48) = Stage_Y_Pulse.all;

	  	fpga_status2.all = *((volatile uint16_t *)FPGA_BASE + 2);
	  	Stage_Ctrl_Flag_Status = fpga_status2.bit.Stage_Ctrl;

	  	if(Stage_Ctrl_Flag_Status == 1)
	  	{
	  		Limit_Check = 0;
	  		Check = 1;
	  		Stage_Ctrl_Flag_Cmd = 0;
	  		fpga_cmd0.bit.Stage_Ctrl = Stage_Ctrl_Flag_Cmd;
	  		*((volatile uint16_t *)FPGA_BASE + 32) = fpga_cmd0.all;
	  	}

	  	 /* cmd�?? 1?�� ?��?��?��?�� Limit?�� ?��?��?��?�� 경우*/
	  	if(Limit_Check == 0){
	  	if((fpga_status2.bit.LMT_XP == 1) || (fpga_status2.bit.LMT_XN == 1) || (fpga_status2.bit.LMT_YP == 1) || (fpga_status2.bit.LMT_YN == 1)){
	  		Limit_Check = 1;
	  		Stage_Ctrl_Flag_Cmd = 0;
	  		fpga_cmd0.bit.Stage_Ctrl = Stage_Ctrl_Flag_Cmd;
	  		 *((volatile uint16_t *)FPGA_BASE + 32) = fpga_cmd0.all;

	  		if((fpga_status2.bit.LMT_XP == 1) || (fpga_status2.bit.LMT_XN == 1)){
	  			Stage_X_Pulse_in = 0;
	  			Stage_X_Pulse.all = Stage_X_Pulse_in;
	  			*((volatile uint16_t *)FPGA_BASE + 47) = Stage_X_Pulse.all;
	  		}
	  		else if((fpga_status2.bit.LMT_YP == 1) || (fpga_status2.bit.LMT_YN == 1)){
	  			Stage_Y_Pulse_in = 0;
	  			Stage_Y_Pulse.all = Stage_Y_Pulse_in;
	  			*((volatile uint16_t *)FPGA_BASE + 48) = Stage_Y_Pulse.all;
	  		}
	  	}
	  }


	  }

	  /* Encoder Test */

	  Enc_X_Max_Cnt.all = 14400;
	  Enc_Y_Max_Cnt.all = 14400;
	  *((volatile uint16_t *)FPGA_BASE + 45) = Enc_X_Max_Cnt.all;
	  *((volatile uint16_t *)FPGA_BASE + 46) = Enc_Y_Max_Cnt.all;

	  int *pRxBuffer = (int*)UserRxBufferFS;
	  int *pBufOld = (int*)oldRxBuf;

	  // UserRxBufferFS ?�� ?��로운 명령?�� ?��?��?�� �????????
	  if(*(pBufOld+0) != *(pRxBuffer+0))
	  {
	  //if(oldRxBuf[0] != UserRxBufferFS[0]) {
		memcpy(oldRxBuf, UserRxBufferFS, 64);
		memcpy(UserTxBufferFS, UserRxBufferFS, 64);
	  	memset(UserRxBufferFS, 0, APP_RX_DATA_SIZE);
	  	char cmd = *((char*)(oldRxBuf)+1);
	  	if (*((char*)oldRxBuf) == 'S')
	  	{
	  		switch(cmd)
	  		 {
	  			case 0x01:
	  				fpga_cmd0.bit.P_LED1 = 0;
	  				break;

	  			case 0x02:
	  				fpga_cmd0.bit.P_LED2 = 0;
	  				break;

	  			case 0x03:
	  				fpga_cmd0.bit.P_LED1 = 1;
	  				break;

	  			case 0x04:
	  				fpga_cmd0.bit.P_LED2 = 1;
	  				break;

	  			case 0x05:
	  				break;

	  			default:
	  				break;
	  		  }
	  		CDC_Transmit_HS(UserTxBufferFS,64);
	  	}
	  	else  // when an error occurs (Start Byte error)
	  	{

	  	}
	  }


		if(old_tm3_cnt != tm3_cnt) // every 8KHz -> every 125us
		{
			old_tm3_cnt = tm3_cnt;

			/* Test Triangle */
			if(Test_Triangle == 1 && Scanner_Control.Flag == 0){
					  if(Test_Count == 0){
						  // ?��기에?���? 만든 Gcode �?분에?�� x�?�?, y�?�? �??��?���? ???��?��
						  /////////////////////////////////////////////////////////////
						/*  Change_Gcode = (char *)Test_Gcode[0][4];
						  Gcode_X = strtol(Change_Gcode, NULL,16);
						  Change_Gcode = (char *)Test_Gcode[0][8];
						  Gcode_Y = strtol(Change_Gcode, NULL,16);
						  Change_Gcode = (char *)Test_Gcode[0][12];
						  Gcode_Z = strtol(Change_Gcode, NULL,16);  */
						  Gcode_X = strtol(Test_Gcode[0][4], NULL,16);
						  Gcode_Y = strtol(Test_Gcode[0][8], NULL,16);
						  Gcode_Z = strtol(Test_Gcode[0][12], NULL,16);

						  Scanner_Control.Scan_x = Gcode_X; // 20
						  Scanner_Control.Scan_y = Gcode_Y; // 0
						  Scanner_Control.Scan_z = Gcode_Z;
						  /////////////////////////////////////////////////////////////
						  Scanner_Control.Flag = 1;
						  Test_Count = 1;
					  }else if(Test_Count == 1){
						  // ?��기에?���? 만든 Gcode �?분에?�� x�?�?, y�?�? �??��?���? ???��?��
						  /////////////////////////////////////////////////////////////
						  Gcode_X = strtol(Test_Gcode[1][4], NULL,16);
						  Gcode_Y = strtol(Test_Gcode[1][8], NULL,16);
						  Gcode_Z = strtol(Test_Gcode[1][12], NULL,16);

						  Scanner_Control.Scan_x = Gcode_X; // 20
						  Scanner_Control.Scan_y = Gcode_Y; // 20
						  Scanner_Control.Scan_z = Gcode_Z;
						  /////////////////////////////////////////////////////////////

						  Scanner_Control.Flag = 1;
						  Test_Count = 2;
					  }else if(Test_Count == 2){
						  // ?��기에?���? 만든 Gcode �?분에?�� x�?�?, y�?�? �??��?���? ???��?��
						  /////////////////////////////////////////////////////////////
						  Gcode_X = strtol(Test_Gcode[2][4], NULL,16);
						  Gcode_Y = strtol(Test_Gcode[2][8], NULL,16);
						  Gcode_Z = strtol(Test_Gcode[2][12], NULL,16);

						  Scanner_Control.Scan_x = Gcode_X; // 0
						  Scanner_Control.Scan_y = Gcode_Y; // 0
						  Scanner_Control.Scan_z = Gcode_Z;
						  /////////////////////////////////////////////////////////////
						  Scanner_Control.Flag = 1;
						  Test_Count = 0;
						  Test_Triangle = 0;
					  }
			}


			/* Laser Output Control */
			if(Laser_Flag == 1)
			{
				fpga_cmd0.bit.Laser_Ctrl = 1;
				*((volatile uint16_t *)FPGA_BASE + 32) = fpga_cmd0.all;
			}
			else // Laser_Flag == 0
			{
				fpga_cmd0.bit.Laser_Ctrl = 0;
				*((volatile uint16_t *)FPGA_BASE + 32) = fpga_cmd0.all;  // ?�� �?�? ?��중에 ?��?��?��
			}

			/* Scanner_ED bit Check */
			fpga_status2.all = *((volatile uint16_t *)FPGA_BASE + 2);
			if (fpga_status2.bit.Scanner_ED == 1) // when scanner error occurs
			{
				Scanner_X_0.bit.Scanner_X_0 = 0x0000;   // example
				Scanner_Y_0.bit.Scanner_Y_0 = 0x0000;
				Scanner_Z_0.bit.Scanner_Z_0 = 0x0000;
				*((volatile uint16_t *)FPGA_BASE + 33) = Scanner_X_0.all;
				*((volatile uint16_t *)FPGA_BASE + 34) = Scanner_Y_0.all;
				*((volatile uint16_t *)FPGA_BASE + 35) = Scanner_Z_0.all;
			}

			if (fpga_status2.bit.Scanner_ED == 0) // when scanner is operating normally
			{
				static int pos_index = 0, pos_processed, skip_cnt = 0;
				int buffer_ok;
				fpga_status2.all = *((volatile uint16_t *)FPGA_BASE + 2);
				pos_processed = fpga_status2.bit.Pos_processed;
				processtest = pos_processed; // test
				buffer_ok = (pos_index + 5 - pos_processed) & 0x3;

				if(buffer_ok)
				{
					Scanner_X_0.all = Scanner_Control.Scan_x_wobble; // Scanner_X command
					Scanner_Y_0.all = Scanner_Control.Scan_y_wobble; // Scanner Y command
					Scanner_Z_0.all = Scanner_Control.Scan_z_wobble; // Scanner Z command

					if(pos_index == 0)
					{
						*((volatile uint16_t *)FPGA_BASE + 33) = Scanner_X_0.all;
						*((volatile uint16_t *)FPGA_BASE + 34) = Scanner_Y_0.all;
						*((volatile uint16_t *)FPGA_BASE + 35) = Scanner_Z_0.all;
						pos_index = 1;
						indextest = 1; // test
					}
					else if(pos_index == 1)
					{
						*((volatile uint16_t *)FPGA_BASE + 36) = Scanner_X_0.all;
						*((volatile uint16_t *)FPGA_BASE + 37) = Scanner_Y_0.all;
						*((volatile uint16_t *)FPGA_BASE + 38) = Scanner_Z_0.all;
						pos_index = 2;
						indextest = 2; // test
					}
					else if(pos_index == 2)
					{
						*((volatile uint16_t *)FPGA_BASE + 39) = Scanner_X_0.all;
						*((volatile uint16_t *)FPGA_BASE + 40) = Scanner_Y_0.all;
						*((volatile uint16_t *)FPGA_BASE + 41) = Scanner_Z_0.all;
						pos_index = 3;
						indextest = 3; // test
					}
					else // if(pos_index == 3)
					{
						*((volatile uint16_t *)FPGA_BASE + 42) = Scanner_X_0.all;
						*((volatile uint16_t *)FPGA_BASE + 43) = Scanner_Y_0.all;
						*((volatile uint16_t *)FPGA_BASE + 44) = Scanner_Z_0.all;
						pos_index = 0;
						indextest = 0; // test
					}
					fpga_cmd0.bit.Pos_Index = pos_index;
					*((volatile uint16_t *)FPGA_BASE + 32) = fpga_cmd0.all;

					// check buffer operation
					Scanner_X_1.all = *((volatile uint16_t *)FPGA_BASE + 36);
					Scanner_Y_1.all = *((volatile uint16_t *)FPGA_BASE + 37);
					Scanner_Z_1.all = *((volatile uint16_t *)FPGA_BASE + 38);
					Scanner_X_2.all = *((volatile uint16_t *)FPGA_BASE + 39);
					Scanner_Y_2.all = *((volatile uint16_t *)FPGA_BASE + 40);
					Scanner_Z_2.all = *((volatile uint16_t *)FPGA_BASE + 41);
					Scanner_X_3.all = *((volatile uint16_t *)FPGA_BASE + 42);
					Scanner_Y_3.all = *((volatile uint16_t *)FPGA_BASE + 43);
					Scanner_Z_3.all = *((volatile uint16_t *)FPGA_BASE + 44);
				}

				else
				{
					skip_cnt++;
				}
			}

			if(Scanner_Control.Flag == 1)
			{
				Scanner_Correction(&Scanner_Control);
				//Scanner_Line_Draw(&Scanner_Control);
				Scanner_Line_Draw_Re(&Scanner_Control);
				//Scanner_Line_Draw_RERE(&Scanner_Control);
				Scanner_Wobble_Circle(&Scanner_Control);
			}

		/*	if(Center_Wobble_Flag == 1)
			{
				Scanner_Correction(&Scanner_Control);
				Scanner_Line_Draw(&Scanner_Control);
				Scanner_Wobble_Circle(&Scanner_Control);
			}   */

			if(Scanner_Control.Figure_Flag == 1 && Scanner_Control.Flag == 0){
				if(Test_Count == 1){
					Laser_Flag = 1;
				}

				if(Scanner_Control.direction == 0){
					if(Scanner_Control.Figure_Angle >= 7){
						Scanner_Control.Angle = Test_Count * 2 * Scanner_Control.PI / 720;

						input_X = cos(Scanner_Control.Angle) * Scanner_Control.Scan_radius;
						input_Y = sin(Scanner_Control.Angle) * Scanner_Control.Scan_radius;
					}else{
						Scanner_Control.Angle = Test_Count * 2 * Scanner_Control.PI / Scanner_Control.Figure_Angle;

						//Scanner_Control.Scan_radius_dist_x = Scanner_Control.Scan_radius;// * Scanner_Control.Scan_x_scale_mm;
						//Scanner_Control.Scan_radius_dist_y = Scanner_Control.Scan_radius;// * Scanner_Control.Scan_y_scale_mm;

						input_X = cos(Scanner_Control.Angle) * Scanner_Control.Scan_radius;
						input_Y = sin(Scanner_Control.Angle) * Scanner_Control.Scan_radius;
						//aaaaaa+=120;
					}
				}
				else{
					if(Scanner_Control.Figure_Angle >= 7){
						Scanner_Control.Angle = (720 - Test_Count) * 2 * Scanner_Control.PI / 720;

						input_X = cos(Scanner_Control.Angle) * Scanner_Control.Scan_radius;
						input_Y = sin(Scanner_Control.Angle) * Scanner_Control.Scan_radius;
					}else{
						Scanner_Control.Angle = (Scanner_Control.Figure_Angle - Test_Count) * 2 * Scanner_Control.PI / Scanner_Control.Figure_Angle;

						Scanner_Control.Scan_radius_dist_x = Scanner_Control.Scan_radius; //* Scanner_Control.Scan_x_scale_mm;
						Scanner_Control.Scan_radius_dist_y = Scanner_Control.Scan_radius; //* Scanner_Control.Scan_y_scale_mm;

						//input_X = cos(Scanner_Control.Angle) * Scanner_Control.Scan_radius_dist_x;
						//input_Y = sin(Scanner_Control.Angle) * Scanner_Control.Scan_radius_dist_y;
					}
				}

//				input_X = cos(Scanner_Control.Angle) * Scanner_Control.Scan_radius;
//				input_Y = sin(Scanner_Control.Angle) * Scanner_Control.Scan_radius;


				Scanner_Control.Scan_x = input_X;
				Scanner_Control.Scan_y = input_Y;
				Scanner_Control.Flag = 1;

				Test_Count++;
				if(Scanner_Control.Figure_Angle >= 7){
					if(Test_Count > 720){
							Test_Count = 0;
							Laser_Flag = 0;
							Scanner_Control.Figure_Flag = 0;
					}
				}else{
					if(Test_Count > (Scanner_Control.Figure_Angle + 1)){    // Test_Count > (Scanner_Control.Figure_Angle + 1)
							Test_Count = 0;
							Laser_Flag = 0;
							Scanner_Control.Figure_Flag = 0;
					}
				}


				//Scanner_Control.Circle_Flag = 0;

				//Scanner_Wobble_Circle(&Scanner_Control);
			}

			// Encoder check
			Enc_X_cnt = *((volatile uint16_t *)FPGA_BASE + 3);
			Enc_Y_cnt = *((volatile uint16_t *)FPGA_BASE + 4);

			Enc_X_rev = *((volatile uint16_t *)FPGA_BASE + 5) & 0x00FF;
			Enc_Y_rev = (*((volatile uint16_t *)FPGA_BASE + 5) & 0xFF00) >> 8;

			HAL_DAC_SetValue(&hdac1, DAC_CHANNEL_1, DAC_ALIGN_12B_R, tm3_cnt & 0xFFF);
			HAL_DAC_SetValue(&hdac1, DAC_CHANNEL_2, DAC_ALIGN_12B_R, (0x1000-tm3_cnt) & 0xFFF);
		  }

		if(USB_flag)
		{
			CDC_Transmit_HS(bufferA,BUF_SIZE);
			USB_flag = 0;
		}

  }
  /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Supply configuration update enable
  */
  HAL_PWREx_ConfigSupply(PWR_LDO_SUPPLY);
  /** Configure the main internal regulator output voltage
  */
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE0);

  while(!__HAL_PWR_GET_FLAG(PWR_FLAG_VOSRDY)) {}
  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI48|RCC_OSCILLATORTYPE_LSI
                              |RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.LSIState = RCC_LSI_ON;
  RCC_OscInitStruct.HSI48State = RCC_HSI48_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLM = 1;
  RCC_OscInitStruct.PLL.PLLN = 30;
  RCC_OscInitStruct.PLL.PLLP = 1;
  RCC_OscInitStruct.PLL.PLLQ = 2;
  RCC_OscInitStruct.PLL.PLLR = 2;
  RCC_OscInitStruct.PLL.PLLRGE = RCC_PLL1VCIRANGE_3;
  RCC_OscInitStruct.PLL.PLLVCOSEL = RCC_PLL1VCOWIDE;
  RCC_OscInitStruct.PLL.PLLFRACN = 0;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }
  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2
                              |RCC_CLOCKTYPE_D3PCLK1|RCC_CLOCKTYPE_D1PCLK1;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.SYSCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_HCLK_DIV2;
  RCC_ClkInitStruct.APB3CLKDivider = RCC_APB3_DIV2;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_APB1_DIV2;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_APB2_DIV2;
  RCC_ClkInitStruct.APB4CLKDivider = RCC_APB4_DIV2;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_3) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
  * @brief DAC1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_DAC1_Init(void)
{

  /* USER CODE BEGIN DAC1_Init 0 */

  /* USER CODE END DAC1_Init 0 */

  DAC_ChannelConfTypeDef sConfig = {0};

  /* USER CODE BEGIN DAC1_Init 1 */

  /* USER CODE END DAC1_Init 1 */
  /** DAC Initialization
  */
  hdac1.Instance = DAC1;
  if (HAL_DAC_Init(&hdac1) != HAL_OK)
  {
    Error_Handler();
  }
  /** DAC channel OUT1 config
  */
  sConfig.DAC_SampleAndHold = DAC_SAMPLEANDHOLD_DISABLE;
  sConfig.DAC_Trigger = DAC_TRIGGER_NONE;
  sConfig.DAC_OutputBuffer = DAC_OUTPUTBUFFER_ENABLE;
  sConfig.DAC_ConnectOnChipPeripheral = DAC_CHIPCONNECT_DISABLE;
  sConfig.DAC_UserTrimming = DAC_TRIMMING_FACTORY;
  if (HAL_DAC_ConfigChannel(&hdac1, &sConfig, DAC_CHANNEL_1) != HAL_OK)
  {
    Error_Handler();
  }
  /** DAC channel OUT2 config
  */
  if (HAL_DAC_ConfigChannel(&hdac1, &sConfig, DAC_CHANNEL_2) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN DAC1_Init 2 */

  /* USER CODE END DAC1_Init 2 */

}

/**
  * @brief ETH Initialization Function
  * @param None
  * @retval None
  */
static void MX_ETH_Init(void)
{

  /* USER CODE BEGIN ETH_Init 0 */

  /* USER CODE END ETH_Init 0 */

   static uint8_t MACAddr[6];

  /* USER CODE BEGIN ETH_Init 1 */

  /* USER CODE END ETH_Init 1 */
  heth.Instance = ETH;
  MACAddr[0] = 0x00;
  MACAddr[1] = 0x80;
  MACAddr[2] = 0xE1;
  MACAddr[3] = 0x00;
  MACAddr[4] = 0x00;
  MACAddr[5] = 0x00;
  heth.Init.MACAddr = &MACAddr[0];
  heth.Init.MediaInterface = HAL_ETH_RMII_MODE;
  heth.Init.TxDesc = DMATxDscrTab;
  heth.Init.RxDesc = DMARxDscrTab;
  heth.Init.RxBuffLen = 1524;

  /* USER CODE BEGIN MACADDRESS */

  /* USER CODE END MACADDRESS */

  if (HAL_ETH_Init(&heth) != HAL_OK)
  {
    Error_Handler();
  }

  memset(&TxConfig, 0 , sizeof(ETH_TxPacketConfig));
  TxConfig.Attributes = ETH_TX_PACKETS_FEATURES_CSUM | ETH_TX_PACKETS_FEATURES_CRCPAD;
  TxConfig.ChecksumCtrl = ETH_CHECKSUM_IPHDR_PAYLOAD_INSERT_PHDR_CALC;
  TxConfig.CRCPadCtrl = ETH_CRC_PAD_INSERT;
  /* USER CODE BEGIN ETH_Init 2 */

  /* USER CODE END ETH_Init 2 */

}

/**
  * @brief RTC Initialization Function
  * @param None
  * @retval None
  */
static void MX_RTC_Init(void)
{

  /* USER CODE BEGIN RTC_Init 0 */

  /* USER CODE END RTC_Init 0 */

  /* USER CODE BEGIN RTC_Init 1 */

  /* USER CODE END RTC_Init 1 */
  /** Initialize RTC Only
  */
  hrtc.Instance = RTC;
  hrtc.Init.HourFormat = RTC_HOURFORMAT_24;
  hrtc.Init.AsynchPrediv = 127;
  hrtc.Init.SynchPrediv = 255;
  hrtc.Init.OutPut = RTC_OUTPUT_DISABLE;
  hrtc.Init.OutPutPolarity = RTC_OUTPUT_POLARITY_HIGH;
  hrtc.Init.OutPutType = RTC_OUTPUT_TYPE_OPENDRAIN;
  hrtc.Init.OutPutRemap = RTC_OUTPUT_REMAP_NONE;
  if (HAL_RTC_Init(&hrtc) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN RTC_Init 2 */

  /* USER CODE END RTC_Init 2 */

}

/**
  * @brief SPI1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_SPI1_Init(void)
{

  /* USER CODE BEGIN SPI1_Init 0 */

  /* USER CODE END SPI1_Init 0 */

  /* USER CODE BEGIN SPI1_Init 1 */

  /* USER CODE END SPI1_Init 1 */
  /* SPI1 parameter configuration*/
  hspi1.Instance = SPI1;
  hspi1.Init.Mode = SPI_MODE_MASTER;
  hspi1.Init.Direction = SPI_DIRECTION_2LINES;
  hspi1.Init.DataSize = SPI_DATASIZE_4BIT;
  hspi1.Init.CLKPolarity = SPI_POLARITY_LOW;
  hspi1.Init.CLKPhase = SPI_PHASE_1EDGE;
  hspi1.Init.NSS = SPI_NSS_HARD_OUTPUT;
  hspi1.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_2;
  hspi1.Init.FirstBit = SPI_FIRSTBIT_MSB;
  hspi1.Init.TIMode = SPI_TIMODE_DISABLE;
  hspi1.Init.CRCCalculation = SPI_CRCCALCULATION_DISABLE;
  hspi1.Init.CRCPolynomial = 0x0;
  hspi1.Init.NSSPMode = SPI_NSS_PULSE_ENABLE;
  hspi1.Init.NSSPolarity = SPI_NSS_POLARITY_LOW;
  hspi1.Init.FifoThreshold = SPI_FIFO_THRESHOLD_01DATA;
  hspi1.Init.TxCRCInitializationPattern = SPI_CRC_INITIALIZATION_ALL_ZERO_PATTERN;
  hspi1.Init.RxCRCInitializationPattern = SPI_CRC_INITIALIZATION_ALL_ZERO_PATTERN;
  hspi1.Init.MasterSSIdleness = SPI_MASTER_SS_IDLENESS_00CYCLE;
  hspi1.Init.MasterInterDataIdleness = SPI_MASTER_INTERDATA_IDLENESS_00CYCLE;
  hspi1.Init.MasterReceiverAutoSusp = SPI_MASTER_RX_AUTOSUSP_DISABLE;
  hspi1.Init.MasterKeepIOState = SPI_MASTER_KEEP_IO_STATE_DISABLE;
  hspi1.Init.IOSwap = SPI_IO_SWAP_DISABLE;
  if (HAL_SPI_Init(&hspi1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN SPI1_Init 2 */

  /* USER CODE END SPI1_Init 2 */

}

/**
  * @brief TIM1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_TIM1_Init(void)
{

  /* USER CODE BEGIN TIM1_Init 0 */

  /* USER CODE END TIM1_Init 0 */

  TIM_ClockConfigTypeDef sClockSourceConfig = {0};
  TIM_SlaveConfigTypeDef sSlaveConfig = {0};
  TIM_MasterConfigTypeDef sMasterConfig = {0};

  /* USER CODE BEGIN TIM1_Init 1 */

  /* USER CODE END TIM1_Init 1 */
  htim1.Instance = TIM1;
  htim1.Init.Prescaler = 0;
  htim1.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim1.Init.Period = 65535;
  htim1.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim1.Init.RepetitionCounter = 0;
  htim1.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  if (HAL_TIM_Base_Init(&htim1) != HAL_OK)
  {
    Error_Handler();
  }
  sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
  if (HAL_TIM_ConfigClockSource(&htim1, &sClockSourceConfig) != HAL_OK)
  {
    Error_Handler();
  }
  sSlaveConfig.SlaveMode = TIM_SLAVEMODE_RESET;
  sSlaveConfig.InputTrigger = TIM_TS_ITR3;
  if (HAL_TIM_SlaveConfigSynchro(&htim1, &sSlaveConfig) != HAL_OK)
  {
    Error_Handler();
  }
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterOutputTrigger2 = TIM_TRGO2_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim1, &sMasterConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN TIM1_Init 2 */

  /* USER CODE END TIM1_Init 2 */

}

/**
  * @brief TIM3 Initialization Function
  * @param None
  * @retval None
  */
static void MX_TIM3_Init(void)
{

  /* USER CODE BEGIN TIM3_Init 0 */

  /* USER CODE END TIM3_Init 0 */

  TIM_ClockConfigTypeDef sClockSourceConfig = {0};
  TIM_MasterConfigTypeDef sMasterConfig = {0};

  /* USER CODE BEGIN TIM3_Init 1 */

  /* USER CODE END TIM3_Init 1 */
  htim3.Instance = TIM3;
  htim3.Init.Prescaler = 79;
  htim3.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim3.Init.Period = 199;
  htim3.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim3.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_ENABLE;
  if (HAL_TIM_Base_Init(&htim3) != HAL_OK)
  {
    Error_Handler();
  }
  sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
  if (HAL_TIM_ConfigClockSource(&htim3, &sClockSourceConfig) != HAL_OK)
  {
    Error_Handler();
  }
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim3, &sMasterConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN TIM3_Init 2 */

  /* USER CODE END TIM3_Init 2 */

}

/**
  * @brief UART4 Initialization Function
  * @param None
  * @retval None
  */
static void MX_UART4_Init(void)
{

  /* USER CODE BEGIN UART4_Init 0 */

  /* USER CODE END UART4_Init 0 */

  /* USER CODE BEGIN UART4_Init 1 */

  /* USER CODE END UART4_Init 1 */
  huart4.Instance = UART4;
  huart4.Init.BaudRate = 115200;
  huart4.Init.WordLength = UART_WORDLENGTH_8B;
  huart4.Init.StopBits = UART_STOPBITS_1;
  huart4.Init.Parity = UART_PARITY_NONE;
  huart4.Init.Mode = UART_MODE_TX_RX;
  huart4.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart4.Init.OverSampling = UART_OVERSAMPLING_16;
  huart4.Init.OneBitSampling = UART_ONE_BIT_SAMPLE_DISABLE;
  huart4.Init.ClockPrescaler = UART_PRESCALER_DIV1;
  huart4.AdvancedInit.AdvFeatureInit = UART_ADVFEATURE_NO_INIT;
  if (HAL_UART_Init(&huart4) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_UARTEx_SetTxFifoThreshold(&huart4, UART_TXFIFO_THRESHOLD_1_8) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_UARTEx_SetRxFifoThreshold(&huart4, UART_RXFIFO_THRESHOLD_1_8) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_UARTEx_DisableFifoMode(&huart4) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN UART4_Init 2 */

  /* USER CODE END UART4_Init 2 */

}

/* FMC initialization function */
static void MX_FMC_Init(void)
{

  /* USER CODE BEGIN FMC_Init 0 */

  /* USER CODE END FMC_Init 0 */

  FMC_NORSRAM_TimingTypeDef Timing = {0};

  /* USER CODE BEGIN FMC_Init 1 */

  /* USER CODE END FMC_Init 1 */

  /** Perform the SRAM2 memory initialization sequence
  */
  hsram2.Instance = FMC_NORSRAM_DEVICE;
  hsram2.Extended = FMC_NORSRAM_EXTENDED_DEVICE;
  /* hsram2.Init */
  hsram2.Init.NSBank = FMC_NORSRAM_BANK2;
  hsram2.Init.DataAddressMux = FMC_DATA_ADDRESS_MUX_DISABLE;
  hsram2.Init.MemoryType = FMC_MEMORY_TYPE_PSRAM;
  hsram2.Init.MemoryDataWidth = FMC_NORSRAM_MEM_BUS_WIDTH_16;
  hsram2.Init.BurstAccessMode = FMC_BURST_ACCESS_MODE_DISABLE;
  hsram2.Init.WaitSignalPolarity = FMC_WAIT_SIGNAL_POLARITY_LOW;
  hsram2.Init.WaitSignalActive = FMC_WAIT_TIMING_BEFORE_WS;
  hsram2.Init.WriteOperation = FMC_WRITE_OPERATION_ENABLE;
  hsram2.Init.WaitSignal = FMC_WAIT_SIGNAL_DISABLE;
  hsram2.Init.ExtendedMode = FMC_EXTENDED_MODE_DISABLE;
  hsram2.Init.AsynchronousWait = FMC_ASYNCHRONOUS_WAIT_DISABLE;
  hsram2.Init.WriteBurst = FMC_WRITE_BURST_DISABLE;
  hsram2.Init.ContinuousClock = FMC_CONTINUOUS_CLOCK_SYNC_ONLY;
  hsram2.Init.WriteFifo = FMC_WRITE_FIFO_DISABLE;
  hsram2.Init.PageSize = FMC_PAGE_SIZE_NONE;
  /* Timing */
  Timing.AddressSetupTime = 10;
  Timing.AddressHoldTime = 15;
  Timing.DataSetupTime = 10;
  Timing.BusTurnAroundDuration = 1;
  Timing.CLKDivision = 16;
  Timing.DataLatency = 17;
  Timing.AccessMode = FMC_ACCESS_MODE_A;
  /* ExtTiming */

  if (HAL_SRAM_Init(&hsram2, &Timing, NULL) != HAL_OK)
  {
    Error_Handler( );
  }

  HAL_SetFMCMemorySwappingConfig(FMC_SWAPBMAP_SDRAM_SRAM);

  /** Perform the SRAM3 memory initialization sequence
  */
  hsram3.Instance = FMC_NORSRAM_DEVICE;
  hsram3.Extended = FMC_NORSRAM_EXTENDED_DEVICE;
  /* hsram3.Init */
  hsram3.Init.NSBank = FMC_NORSRAM_BANK3;
  hsram3.Init.DataAddressMux = FMC_DATA_ADDRESS_MUX_DISABLE;
  hsram3.Init.MemoryType = FMC_MEMORY_TYPE_PSRAM;
  hsram3.Init.MemoryDataWidth = FMC_NORSRAM_MEM_BUS_WIDTH_16;
  hsram3.Init.BurstAccessMode = FMC_BURST_ACCESS_MODE_DISABLE;
  hsram3.Init.WaitSignalPolarity = FMC_WAIT_SIGNAL_POLARITY_LOW;
  hsram3.Init.WaitSignalActive = FMC_WAIT_TIMING_BEFORE_WS;
  hsram3.Init.WriteOperation = FMC_WRITE_OPERATION_ENABLE;
  hsram3.Init.WaitSignal = FMC_WAIT_SIGNAL_DISABLE;
  hsram3.Init.ExtendedMode = FMC_EXTENDED_MODE_DISABLE;
  hsram3.Init.AsynchronousWait = FMC_ASYNCHRONOUS_WAIT_DISABLE;
  hsram3.Init.WriteBurst = FMC_WRITE_BURST_DISABLE;
  hsram3.Init.ContinuousClock = FMC_CONTINUOUS_CLOCK_SYNC_ONLY;
  hsram3.Init.WriteFifo = FMC_WRITE_FIFO_DISABLE;
  hsram3.Init.PageSize = FMC_PAGE_SIZE_NONE;
  /* Timing */
  Timing.AddressSetupTime = 15;
  Timing.AddressHoldTime = 15;
  Timing.DataSetupTime = 255;
  Timing.BusTurnAroundDuration = 15;
  Timing.CLKDivision = 16;
  Timing.DataLatency = 17;
  Timing.AccessMode = FMC_ACCESS_MODE_A;
  /* ExtTiming */

  if (HAL_SRAM_Init(&hsram3, &Timing, NULL) != HAL_OK)
  {
    Error_Handler( );
  }

  /* USER CODE BEGIN FMC_Init 2 */

  /* USER CODE END FMC_Init 2 */
}

/**
  * @brief GPIO Initialization Function
  * @param None
  * @retval None
  */
static void MX_GPIO_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOE_CLK_ENABLE();
  __HAL_RCC_GPIOC_CLK_ENABLE();
  __HAL_RCC_GPIOF_CLK_ENABLE();
  __HAL_RCC_GPIOH_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOG_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();
  __HAL_RCC_GPIOD_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOE, GPIO_PIN_6, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOC, GPIO_PIN_13|GPIO_PIN_14, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(PULLUP_USB_DP_GPIO_Port, PULLUP_USB_DP_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOD, GPIO_PIN_6, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOG, GPIO_PIN_14, GPIO_PIN_RESET);

  /*Configure GPIO pin : PE6 */
  GPIO_InitStruct.Pin = GPIO_PIN_6;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOE, &GPIO_InitStruct);

  /*Configure GPIO pins : PC13 PC14 */
  GPIO_InitStruct.Pin = GPIO_PIN_13|GPIO_PIN_14;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

  /*Configure GPIO pins : PC15 PC6 PC7 PC8
                           PC9 */
  GPIO_InitStruct.Pin = GPIO_PIN_15|GPIO_PIN_6|GPIO_PIN_7|GPIO_PIN_8
                          |GPIO_PIN_9;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

  /*Configure GPIO pins : PF6 PF7 PF8 PF9 */
  GPIO_InitStruct.Pin = GPIO_PIN_6|GPIO_PIN_7|GPIO_PIN_8|GPIO_PIN_9;
  GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  GPIO_InitStruct.Alternate = GPIO_AF5_SPI5;
  HAL_GPIO_Init(GPIOF, &GPIO_InitStruct);

  /*Configure GPIO pin : DETECT_VBUS_Pin */
  GPIO_InitStruct.Pin = DETECT_VBUS_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(DETECT_VBUS_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pin : PULLUP_USB_DP_Pin */
  GPIO_InitStruct.Pin = PULLUP_USB_DP_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(PULLUP_USB_DP_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pin : PG7 */
  GPIO_InitStruct.Pin = GPIO_PIN_7;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(GPIOG, &GPIO_InitStruct);

  /*Configure GPIO pin : PD6 */
  GPIO_InitStruct.Pin = GPIO_PIN_6;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOD, &GPIO_InitStruct);

  /*Configure GPIO pin : PG14 */
  GPIO_InitStruct.Pin = GPIO_PIN_14;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOG, &GPIO_InitStruct);

  /*Configure GPIO pin : PB4 */
  GPIO_InitStruct.Pin = GPIO_PIN_4;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  /* EXTI interrupt init*/
  HAL_NVIC_SetPriority(EXTI9_5_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(EXTI9_5_IRQn);

}

/* USER CODE BEGIN 4 */
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
	if(htim->Instance == TIM3)
	{
		tm3_cnt++;
	}
}



/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */


//  __disable_irq();
//  while (1)
//  {
//  }


  /* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */

