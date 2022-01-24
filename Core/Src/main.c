/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2021 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under BSD 3-Clause license,
  * the "License"; You may not use this file except in compliance with the
  * License. You may obtain a copy of the License at:
  *                        opensource.org/licenses/BSD-3-Clause
  *
  ******************************************************************************
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "dma.h"
#include "fatfs.h"
#include "sdio.h"
#include "usart.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

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

/* USER CODE BEGIN PV */
typedef enum {FAILED = 0, PASSED = !FAILED} TestStatus;
#define BLOCK_SIZE 512 // SD卡块大小
#define NUMBER_OF_BLOCKS 8 // 测试块数量 (小于 15) 
#define WRITE_READ_ADDRESS 0x00002000 // 测试读写地址
__align(4) uint32_t Buffer_Block_Tx[BLOCK_SIZE*NUMBER_OF_BLOCKS]; // 写数据缓存
__align(4) uint32_t Buffer_Block_Rx[BLOCK_SIZE*NUMBER_OF_BLOCKS]; // 读数据缓存
HAL_StatusTypeDef sd_status; // HAL库函数操作 SD卡函数返回值：操作结果
TestStatus test_status; // 数据测试结果

FATFS fs; // Work area (file system object) for logical drive 
FIL fil; // file objects 
uint32_t byteswritten; /* File write counts */ 
uint32_t bytesread; /* File read counts */ 
uint8_t wtext[] = "\
This is STM32 working with FatFs,This is STM32 working with FatFs,This is STM32 working with FatFs,This is STM32 working with FatFs,This is STM32 working with FatFs,This is STM32 working with FatFs,This is STM32 working with FatFs  \
This is STM32 working with FatFs,This is STM32 working with FatFs,This is STM32 working with FatFs,This is STM32 working with FatFs,This is STM32 working with FatFs,This is STM32 working with FatFs,This is STM32 working with FatFs  \
This is STM32 working with FatFs,This is STM32 working with FatFs,This is STM32 working with FatFs,This is STM32 working with FatFs,This is STM32 working with FatFs,This is STM32 working with FatFs,This is STM32 working with FatFs  \
This is STM32 working with FatFs,This is STM32 working with FatFs,This is STM32 working with FatFs,This is STM32 working with FatFs,This is STM32 working with FatFs,This is STM32 working with FatFs,This is STM32 working with FatFs  \
This is STM32 working with FatFs,This is STM32 working with FatFs,This is STM32 working with FatFs,This is STM32 working with FatFs,This is STM32 working with FatFs,This is STM32 working with FatFs,This is STM32 working with FatFs  \
This is STM32 working with FatFs,This is STM32 working with FatFs,This is STM32 working with FatFs,This is STM32 working with FatFs,This is STM32 working with FatFs,This is STM32 working with FatFs,This is STM32 working with FatFs  \
This is STM32 working with FatFs,This is STM32 working with FatFs,This is STM32 working with FatFs,This is STM32 working with FatFs,This is STM32 working with FatFs,This is STM32 working with FatFs,This is STM32 working with FatFs  \
This is STM32 working with FatFs,This is STM32 working with FatFs,This is STM32 working with FatFs,This is STM32 working with FatFs,This is STM32 working with FatFs,This is STM32 working with FatFs,This is STM32 working with FatFs  \
This is STM32 working with FatFs,This is STM32 working with FatFs,This is STM32 working with FatFs,This is STM32 working with FatFs,This is STM32 working with FatFs,This is STM32 working with FatFs,This is STM32 working with FatFs  \
This is STM32 working with FatFs,This is STM32 working with FatFs,This is STM32 working with FatFs,This is STM32 working with FatFs,This is STM32 working with FatFs,This is STM32 working with FatFs,This is STM32 working with FatFs  \
This is STM32 working with FatFs,This is STM32 working with FatFs,This is STM32 working with FatFs,This is STM32 working with FatFs,This is STM32 working with FatFs,This is STM32 working with FatFs,This is STM32 working with FatFs  \
This is STM32 working with FatFs,This is STM32 working with FatFs,This is STM32 working with FatFs,This is STM32 working with FatFs,This is STM32 working with FatFs,This is STM32 working with FatFs,This is STM32 working with FatFs  \
This is STM32 working with FatFs,This is STM32 working with FatFs,This is STM32 working with FatFs,This is STM32 working with FatFs,This is STM32 working with FatFs,This is STM32 working with FatFs,This is STM32 working with FatFs  \
This is STM32 working with FatFs,This is STM32 working with FatFs,This is STM32 working with FatFs,This is STM32 working with FatFs,This is STM32 working with FatFs,This is STM32 working with FatFs,This is STM32 working with FatFs  \
This is STM32 working with FatFs,This is STM32 working with FatFs,This is STM32 working with FatFs,This is STM32 working with FatFs,This is STM32 working with FatFs,This is STM32 working with FatFs,This is STM32 working with FatFs  \
This is STM32 working with FatFs,This is STM32 working with FatFs,This is STM32 working with FatFs,This is STM32 working with FatFs,This is STM32 working with FatFs,This is STM32 working with FatFs,This is STM32 working with FatFs  \
This is STM32 working with FatFs,This is STM32 working with FatFs,This is STM32 working with FatFs,This is STM32 working with FatFs,This is STM32 working with FatFs,This is STM32 working with FatFs,This is STM32 working with FatFs  \
This is STM32 working with FatFs,This is STM32 working with FatFs,This is STM32 working with FatFs,This is STM32 working with FatFs,This is STM32 working with FatFs,This is STM32 working with FatFs,This is STM32 working with FatFs  \
This is STM32 working with FatFs,This is STM32 working with FatFs,This is STM32 working with FatFs,This is STM32 working with FatFs,This is STM32 working with FatFs,This is STM32 working with FatFs,This is STM32 working with FatFs  \
This is STM32 working with FatFs,This is STM32 working with FatFs,This is STM32 working with FatFs,This is STM32 working with FatFs,This is STM32 working with FatFs,This is STM32 working with FatFs,This is STM32 working with FatFs  \
This is STM32 working with FatFs,This is STM32 working with FatFs,This is STM32 working with FatFs,This is STM32 working with FatFs,This is STM32 working with FatFs,This is STM32 working with FatFs,This is STM32 working with FatFs  \
This is STM32 working with FatFs,This is STM32 working with FatFs,This is STM32 working with FatFs,This is STM32 working with FatFs,This is STM32 working with FatFs,This is STM32 working with FatFs,This is STM32 working with FatFs  \
This is STM32 working with FatFs,This is STM32 working with FatFs,This is STM32 working with FatFs,This is STM32 working with FatFs,This is STM32 working with FatFs,This is STM32 working with FatFs,This is STM32 working with FatFs  \
This is STM32 working with FatFs,This is STM32 working with FatFs,This is STM32 working with FatFs,This is STM32 working with FatFs,This is STM32 working with FatFs,This is STM32 working with FatFs,This is STM32 working with FatFs  \
This is STM32 working with FatFs,This is STM32 working with FatFs,This is STM32 working with FatFs,This is STM32 working with FatFs,This is STM32 working with FatFs,This is STM32 working with FatFs,This is STM32 working with FatFs  \
This is STM32 working with FatFs,This is STM32 working with FatFs,This is STM32 working with FatFs,This is STM32 working with FatFs,This is STM32 working with FatFs,This is STM32 working with FatFs,This is STM32 working with FatFs  \
This is STM32 working with FatFs,This is STM32 working with FatFs,This is STM32 working with FatFs,This is STM32 working with FatFs,This is STM32 working with FatFs,This is STM32 working with FatFs,This is STM32 working with FatFs  \
This is STM32 working with FatFs,This is STM32 working with FatFs,This is STM32 working with FatFs,This is STM32 working with FatFs,This is STM32 working with FatFs,This is STM32 working with FatFs,This is STM32 working with FatFs  \
This is STM32 working with FatFs,This is STM32 working with FatFs,This is STM32 working with FatFs,This is STM32 working with FatFs,This is STM32 working with FatFs,This is STM32 working with FatFs,This is STM32 working with FatFs  \
This is STM32 working with FatFs,This is STM32 working with FatFs,This is STM32 working with FatFs,This is STM32 working with FatFs,This is STM32 working with FatFs,This is STM32 working with FatFs,This is STM32 working with FatFs  \
This is STM32 working with FatFs,This is STM32 working with FatFs,This is STM32 working with FatFs,This is STM32 working with FatFs,This is STM32 working with FatFs,This is STM32 working with FatFs,This is STM32 working with FatFs  \
This is STM32 working with FatFs,This is STM32 working with FatFs,This is STM32 working with FatFs,This is STM32 working with FatFs,This is STM32 working with FatFs,This is STM32 working with FatFs,This is STM32 working with FatFs  \
This is STM32 working with FatFs,This is STM32 working with FatFs,This is STM32 working with FatFs,This is STM32 working with FatFs,This is STM32 working with FatFs,This is STM32 working with FatFs,This is STM32 working with FatFs  \
This is STM32 working with FatFs,This is STM32 working with FatFs,This is STM32 working with FatFs,This is STM32 working with FatFs,This is STM32 working with FatFs,This is STM32 working with FatFs,This is STM32 working with FatFs  \
This is STM32 working with FatFs,This is STM32 working with FatFs,This is STM32 working with FatFs,This is STM32 working with FatFs,This is STM32 working with FatFs,This is STM32 working with FatFs,This is STM32 working with FatFs  \
This is STM32 working with FatFs,This is STM32 working with FatFs,This is STM32 working with FatFs,This is STM32 working with FatFs,This is STM32 working with FatFs,This is STM32 working with FatFs,This is STM32 working with FatFs  \
This is STM32 working with FatFs,This is STM32 working with FatFs,This is STM32 working with FatFs,This is STM32 working with FatFs,This is STM32 working with FatFs,This is STM32 working with FatFs,This is STM32 working with FatFs  \
This is STM32 working with FatFs,This is STM32 working with FatFs,This is STM32 working with FatFs,This is STM32 working with FatFs,This is STM32 working with FatFs,This is STM32 working with FatFs,This is STM32 working with FatFs  \
This is STM32 working with FatFs,This is STM32 working with FatFs,This is STM32 working with FatFs,This is STM32 working with FatFs,This is STM32 working with FatFs,This is STM32 working with FatFs,This is STM32 working with FatFs  \
This is STM32 working with FatFs,This is STM32 working with FatFs,This is STM32 working with FatFs,This is STM32 working with FatFs,This is STM32 working with FatFs,This is STM32 working with FatFs,This is STM32 working with FatFs  \
This is STM32 working with FatFs,This is STM32 working with FatFs,This is STM32 working with FatFs,This is STM32 working with FatFs,This is STM32 working with FatFs,This is STM32 working with FatFs,This is STM32 working with FatFs  \
This is STM32 working with FatFs,This is STM32 working with FatFs,This is STM32 working with FatFs,This is STM32 working with FatFs,This is STM32 working with FatFs,This is STM32 working with FatFs,This is STM32 working with FatFs  \
This is STM32 working with FatFs,This is STM32 working with FatFs,This is STM32 working with FatFs,This is STM32 working with FatFs,This is STM32 working with FatFs,This is STM32 working with FatFs,This is STM32 working with FatFs  \
This is STM32 working with FatFs,This is STM32 working with FatFs,This is STM32 working with FatFs,This is STM32 working with FatFs,This is STM32 working with FatFs,This is STM32 working with FatFs,This is STM32 working with FatFs  \
This is STM32 working with FatFs,This is STM32 working with FatFs,This is STM32 working with FatFs,This is STM32 working with FatFs,This is STM32 working with FatFs,This is STM32 working with FatFs,This is STM32 working with FatFs  \
This is STM32 working with FatFs,This is STM32 working with FatFs,This is STM32 working with FatFs,This is STM32 working with FatFs,This is STM32 working with FatFs,This is STM32 working with FatFs,This is STM32 working with FatFs  \
This is STM32 working with FatFs,This is STM32 working with FatFs,This is STM32 working with FatFs,This is STM32 working with FatFs,This is STM32 working with FatFs,This is STM32 working with FatFs,This is STM32 working with FatFs  \
This is STM32 working with FatFs,This is STM32 working with FatFs,This is STM32 working with FatFs,This is STM32 working with FatFs,This is STM32 working with FatFs,This is STM32 working with FatFs,This is STM32 working with FatFs  \
This is STM32 working with FatFs,This is STM32 working with FatFs,This is STM32 working with FatFs,This is STM32 working with FatFs,This is STM32 working with FatFs,This is STM32 working with FatFs,This is STM32 working with FatFs  \
This is STM32 working with FatFs,This is STM32 working with FatFs,This is STM32 working with FatFs,This is STM32 working with FatFs,This is STM32 working with FatFs,This is STM32 working with FatFs,This is STM32 working with FatFs  \
This is STM32 working with FatFs,This is STM32 working with FatFs,This is STM32 working with FatFs,This is STM32 working with FatFs,This is STM32 working with FatFs,This is STM32 working with FatFs,This is STM32 working with FatFs  \
This is STM32 working with FatFs,This is STM32 working with FatFs,This is STM32 working with FatFs,This is STM32 working with FatFs,This is STM32 working with FatFs,This is STM32 working with FatFs,This is STM32 working with FatFs  \
This is STM32 working with FatFs,This is STM32 working with FatFs,This is STM32 working with FatFs,This is STM32 working with FatFs,This is STM32 working with FatFs,This is STM32 working with FatFs,This is STM32 working with FatFs  \
This is STM32 working with FatFs,This is STM32 working with FatFs,This is STM32 working with FatFs,This is STM32 working with FatFs,This is STM32 working with FatFs,This is STM32 working with FatFs,This is STM32 working with FatFs  \
This is STM32 working with FatFs,This is STM32 working with FatFs,This is STM32 working with FatFs,This is STM32 working with FatFs,This is STM32 working with FatFs,This is STM32 working with FatFs,This is STM32 working with FatFs  \
This is STM32 working with FatFs,This is STM32 working with FatFs,This is STM32 working with FatFs,This is STM32 working with FatFs,This is STM32 working with FatFs,This is STM32 working with FatFs,This is STM32 working with FatFs  \
This is STM32 working with FatFs,This is STM32 working with FatFs,This is STM32 working with FatFs,This is STM32 working with FatFs,This is STM32 working with FatFs,This is STM32 working with FatFs,This is STM32 working with FatFs  \
This is STM32 working with FatFs,This is STM32 working with FatFs,This is STM32 working with FatFs,This is STM32 working with FatFs,This is STM32 working with FatFs,This is STM32 working with FatFs,This is STM32 working with FatFs  \
This is STM32 working with FatFs,This is STM32 working with FatFs,This is STM32 working with FatFs,This is STM32 working with FatFs,This is STM32 working with FatFs,This is STM32 working with FatFs,This is STM32 working with FatFs  \
This is STM32 working with FatFs,This is STM32 working with FatFs,This is STM32 working with FatFs,This is STM32 working with FatFs,This is STM32 working with FatFs,This is STM32 working with FatFs,This is STM32 working with FatFs  \
This is STM32 working with FatFs,This is STM32 working with FatFs,This is STM32 working with FatFs,This is STM32 working with FatFs,This is STM32 working with FatFs,This is STM32 working with FatFs,This is STM32 working with FatFs  \
This is STM32 working with FatFs,This is STM32 working with FatFs,This is STM32 working with FatFs,This is STM32 working with FatFs,This is STM32 working with FatFs,This is STM32 working with FatFs,This is STM32 working with FatFs  \
This is STM32 working with FatFs,This is STM32 working with FatFs,This is STM32 working with FatFs,This is STM32 working with FatFs,This is STM32 working with FatFs,This is STM32 working with FatFs,This is STM32 working with FatFs  \
This is STM32 working with FatFs,This is STM32 working with FatFs,This is STM32 working with FatFs,This is STM32 working with FatFs,This is STM32 working with FatFs,This is STM32 working with FatFs,This is STM32 working with FatFs  \
This is STM32 working with FatFs,This is STM32 working with FatFs,This is STM32 working with FatFs,This is STM32 working with FatFs,This is STM32 working with FatFs,This is STM32 working with FatFs,This is STM32 working with FatFs  \
This is STM32 working with FatFs,This is STM32 working with FatFs,This is STM32 working with FatFs,This is STM32 working with FatFs,This is STM32 working with FatFs,This is STM32 working with FatFs,This is STM32 working with FatFs  \
This is STM32 working with FatFs,This is STM32 working with FatFs,This is STM32 working with FatFs,This is STM32 working with FatFs,This is STM32 working with FatFs,This is STM32 working with FatFs,This is STM32 working with FatFs  \
This is STM32 working with FatFs,This is STM32 working with FatFs,This is STM32 working with FatFs,This is STM32 working with FatFs,This is STM32 working with FatFs,This is STM32 working with FatFs,This is STM32 working with FatFs  \
This is STM32 working with FatFs,This is STM32 working with FatFs,This is STM32 working with FatFs,This is STM32 working with FatFs,This is STM32 working with FatFs,This is STM32 working with FatFs,This is STM32 working with FatFs  \
This is STM32 working with FatFs,This is STM32 working with FatFs,This is STM32 working with FatFs,This is STM32 working with FatFs,This is STM32 working with FatFs,This is STM32 working with FatFs,This is STM32 working with FatFs  \
This is STM32 working with FatFs,This is STM32 working with FatFs,This is STM32 working with FatFs,This is STM32 working with FatFs,This is STM32 working with FatFs,This is STM32 working with FatFs,This is STM32 working with FatFs  \
This is STM32 working with FatFs,This is STM32 working with FatFs,This is STM32 working with FatFs,This is STM32 working with FatFs,This is STM32 working with FatFs,This is STM32 working with FatFs,This is STM32 working with FatFs  \
This is STM32 working with FatFs,This is STM32 working with FatFs,This is STM32 working with FatFs,This is STM32 working with FatFs,This is STM32 working with FatFs,This is STM32 working with FatFs,This is STM32 working with FatFs"; /* File write buffer */ 



//uint8_t wtext1[buf_size*1024]={0};
//uint8_t wtext2[buf_size*1024]={0};

uint8_t wtext1[]="1111this is a test string\r\n";
uint8_t wtext2[]="2222this is a test string\r\n";

DAT_type ha_dat;
char hd_buf[1024]={0};

uint8_t rtext[100]; /* File read buffers */ 
char filename[] = "STM32cube.txt";



#define	buf_size	3200	//缓冲区大小，

DAT_ascii_type dat_ascii[buf_size];



/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

uint8_t eBuffercmp(uint32_t* pBuffer, uint32_t BufferLength)
{
	while (BufferLength--) 
	{ /* SD卡擦除后的可能值为 0xff或 0 */ 
		if ((*pBuffer != 0xFFFFFFFF) && (*pBuffer != 0)) 
		{ 
			return 0; 
		} 
		pBuffer++; 
	} return 1;
}

void SD_EraseTest(void)
{
	sd_status=HAL_SD_Erase(&hsd,WRITE_READ_ADDRESS,WRITE_READ_ADDRESS+NUMBER_OF_BLOCKS*4);
	log_info("erase status:%d\r\n",sd_status);
	HAL_Delay(500);
	if (sd_status == HAL_OK)
	{
		sd_status = HAL_SD_ReadBlocks(&hsd,(uint8_t *)Buffer_Block_Rx,WRITE_READ_ADDRESS,NUMBER_OF_BLOCKS,0xffff);
		log_info("erase read status:%d\r\n",sd_status);
		test_status = eBuffercmp(Buffer_Block_Rx,BLOCK_SIZE*NUMBER_OF_BLOCKS);
		if(test_status == PASSED) 
			log_info("》擦除测试成功！ ！\r\n" ); 
		else 
			log_info("》擦除不成功，数据出错！ ！\r\n" );
	}
	else
	{
		log_info("》擦除测试失败！部分 SD不支持擦除，只要读写测试通过即可 \r\n" );
		
	}
}

void Fill_Buffer(uint32_t *pBuffer, uint32_t BufferLength, uint32_t Offset) 
{ 
	uint32_t index = 0; /* 填充数据 */ 
	for (index = 0; index < BufferLength; index++ ) 
	{ 
		pBuffer[index] = index + Offset; 
	} 
}

uint8_t Buffercmp(uint32_t* pBuffer1, uint32_t* pBuffer2, uint32_t BufferLength) 
{ 
	while (BufferLength--) 
	{ 
		if(BufferLength%50==0) 
		{ 
			log_info("buf:0x%08X 0x%08X\r\n",*pBuffer1,*pBuffer2); 
		} 
		if (*pBuffer1 != *pBuffer2) 
		{ 
			return 0; 
		} 
		pBuffer1++; 
		pBuffer2++; 
	} 
	return 1;
}

void SD_Write_Read_Test(void)
{
	int i,j = 0;
	Fill_Buffer(Buffer_Block_Tx,BLOCK_SIZE*NUMBER_OF_BLOCKS, 0x6666);
	sd_status = HAL_SD_WriteBlocks_DMA(&hsd,(uint8_t *)Buffer_Block_Tx,WRITE_READ_ADDRESS,NUMBER_OF_BLOCKS);
	log_info("write status:%d\r\n",sd_status);
	
	HAL_Delay(500);
	
	sd_status = HAL_SD_ReadBlocks_DMA(&hsd,(uint8_t *)Buffer_Block_Rx,WRITE_READ_ADDRESS,NUMBER_OF_BLOCKS); 
	log_info("read status:%d\r\n",sd_status);
	test_status = Buffercmp(Buffer_Block_Tx, Buffer_Block_Rx, BLOCK_SIZE*NUMBER_OF_BLOCKS/4);
	if(test_status == PASSED)
	{
		log_info("》读写测试成功！ ！\r\n" );
		for(i=0;i<BLOCK_SIZE*NUMBER_OF_BLOCKS/4;i++)
		{
			if(j==8)
			{
				log_info("\r\n");
				j=0;
			}
			log_info("%08x ",Buffer_Block_Rx[i]);
			j++;
		}
		log_info("\r\n");
	}
	else
	{
		log_info("》读写测试失败！ ！\r\n " );
	}	
}
void Fatfs_RW_test2(void)
{
	uint32_t systick=0,len=0;
	char lrcr[2]="\r\n";
	short ch[8]={3,65,888,1999,5555,6066,6115,3276};//所采集的数据放大1000倍，再加3276
	float speed=0;
	
	log_info("\r\n ****** FatFs Example ******\r\n\r\n");
	/*##1 Register the file system object to the FatFs module ##############*/
	retSD = f_mount(&fs, "", 1);
	if(retSD){log_info(" mount error : %d \r\n",retSD);Error_Handler();}
	else{log_info(" mount sucess!!! \r\n");}
	
	/*##2 Create and Open new text file objects with write access ######*/
	retSD = f_open(&fil, filename, FA_CREATE_ALWAYS | FA_WRITE);
	if(retSD) {log_info(" open file error : %d\r\n",retSD);}		
	else {log_info(" open file sucess!!! \r\n");}
	
	ha_dat.dat[0].ch_dat[0]=0;
	ha_dat.dat[0].ch_dat[1]=1;
	ha_dat.dat[0].ch_dat[2]=2;
	ha_dat.dat[0].ch_dat[3]=3;
	ha_dat.dat[0].ch_dat[4]=4;
	ha_dat.dat[0].ch_dat[5]=5;
	ha_dat.dat[0].ch_dat[6]=6;
	ha_dat.dat[0].ch_dat[7]=7;
	
	ha_dat.dat[1].ch_dat[0]=8;
	ha_dat.dat[1].ch_dat[1]=9;
	ha_dat.dat[1].ch_dat[2]=10;
	ha_dat.dat[1].ch_dat[3]=11;
	ha_dat.dat[1].ch_dat[4]=12;
	ha_dat.dat[1].ch_dat[5]=13;
	ha_dat.dat[1].ch_dat[6]=14;
	ha_dat.dat[1].ch_dat[7]=15;
	
	
	//dat_ascii[0].dat
	
	{
		//int i=0;
		for(int i=0;i<buf_size;i++)
		{
			len=sprintf(&dat_ascii[i].dat[0],"%04d %04d %04d %04d %04d %04d %04d %04d\r\n",ch[0],ch[1],ch[2],ch[3],ch[4],ch[5],ch[6],ch[7]);
		}
	}
//	len=sprintf(&dat_ascii[0].dat[0],"%04d %04d %04d %04d %04d %04d %04d %04d\r\n",ch[0],ch[1],ch[2],ch[3],ch[4],ch[5],ch[6],ch[7]);
//	log_info("len=%d\r\n",len);
//	len=sprintf(&dat_ascii[1].dat[0],"%04d %04d %04d %04d %04d %04d %04d %04d\r\n",ch[7],ch[6],ch[5],ch[4],ch[3],ch[2],ch[1],ch[0]);
//	log_info("len=%d\r\n",len);
//	len=sprintf(&dat_ascii[2].dat[0],"%04d %04d %04d %04d %04d %04d %04d %04d\r\n",ch[3],ch[2],ch[1],ch[0],ch[7],ch[6],ch[5],ch[4]);
//	log_info("len=%d\r\n",len);
	
	//f_lseek(&fil, fil.fptr);
	/*##3 向文件中写入数据 ###############################*/
//	retSD = f_write(&fil, wtext1, sizeof(wtext1), (void *)&byteswritten);f_lseek(&fil, fil.fptr-1);
//	retSD = f_write(&fil, wtext2, sizeof(wtext2), (void *)&byteswritten);f_lseek(&fil, fil.fptr-1);
//	retSD = f_write(&fil, wtext1, sizeof(wtext1), (void *)&byteswritten);f_lseek(&fil, fil.fptr-1);
//	retSD = f_write(&fil, wtext2, sizeof(wtext2), (void *)&byteswritten);f_lseek(&fil, fil.fptr-1);
//	retSD = f_write(&fil, wtext1, sizeof(wtext1), (void *)&byteswritten);f_lseek(&fil, fil.fptr-1);
//	retSD = f_write(&fil, wtext2, sizeof(wtext2), (void *)&byteswritten);f_lseek(&fil, fil.fptr-1);
//	retSD = f_write(&fil, wtext1, sizeof(wtext1), (void *)&byteswritten);f_lseek(&fil, fil.fptr-1);
//	retSD = f_write(&fil, wtext2, sizeof(wtext2), (void *)&byteswritten);f_lseek(&fil, fil.fptr-1);
//	retSD = f_write(&fil, wtext2, sizeof(wtext2), (void *)&byteswritten);f_lseek(&fil, fil.fptr-1);
	
	systick=HAL_GetTick();
	retSD = f_write(&fil, (void *)&dat_ascii[0].dat[0], sizeof(dat_ascii), (void *)&byteswritten);f_lseek(&fil, fil.fptr);//retSD = f_write(&fil, lrcr, sizeof(lrcr), (void *)&byteswritten);f_lseek(&fil, fil.fptr-1);
	retSD = f_write(&fil, (void *)&dat_ascii[0].dat[0], sizeof(dat_ascii), (void *)&byteswritten);f_lseek(&fil, fil.fptr);//retSD = f_write(&fil, lrcr, sizeof(lrcr), (void *)&byteswritten);f_lseek(&fil, fil.fptr-1);
	retSD = f_write(&fil, (void *)&dat_ascii[0].dat[0], sizeof(dat_ascii), (void *)&byteswritten);f_lseek(&fil, fil.fptr);//retSD = f_write(&fil, lrcr, sizeof(lrcr), (void *)&byteswritten);f_lseek(&fil, fil.fptr-1);
	retSD = f_write(&fil, (void *)&dat_ascii[0].dat[0], sizeof(dat_ascii), (void *)&byteswritten);f_lseek(&fil, fil.fptr);//retSD = f_write(&fil, lrcr, sizeof(lrcr), (void *)&byteswritten);f_lseek(&fil, fil.fptr-1);
	retSD = f_write(&fil, (void *)&dat_ascii[0].dat[0], sizeof(dat_ascii), (void *)&byteswritten);f_lseek(&fil, fil.fptr);//retSD = f_write(&fil, lrcr, sizeof(lrcr), (void *)&byteswritten);f_lseek(&fil, fil.fptr-1);
	retSD = f_write(&fil, (void *)&dat_ascii[0].dat[0], sizeof(dat_ascii), (void *)&byteswritten);f_lseek(&fil, fil.fptr);//retSD = f_write(&fil, lrcr, sizeof(lrcr), (void *)&byteswritten);f_lseek(&fil, fil.fptr-1);
	retSD = f_write(&fil, (void *)&dat_ascii[0].dat[0], sizeof(dat_ascii), (void *)&byteswritten);f_lseek(&fil, fil.fptr);//retSD = f_write(&fil, lrcr, sizeof(lrcr), (void *)&byteswritten);f_lseek(&fil, fil.fptr-1);
	
	systick=HAL_GetTick()-systick;
	speed=1.0*sizeof(dat_ascii[0].dat)*buf_size*7/1024/1024/(systick/1000.0);
	log_info("retSD:%d,byteswritten:%.3fKB,used time:%dms,signal wtext_len:%.3fKB,write speed:%.3fMB/s\r\n",retSD,1.0*sizeof(dat_ascii[0].dat)*buf_size*7/1024,systick,1.0*sizeof(dat_ascii)/1024,speed); 
	
	
	
	//len=sprintf(hd_buf,"%s",&ha_dat.dat[0].ch_dat[0]);
	//log_info("len=%d\r\n",len);
	//retSD = f_write(&fil, (void *)&ha_dat, sizeof(ha_dat), (void *)&byteswritten);f_lseek(&fil, fil.fptr-1);
	
	
	
	
	/*##4 Close the open text files ################################*/
	retSD = f_close(&fil);
	if(retSD){log_info(" close error : %d\r\n",retSD);}
	else{log_info(" close sucess!!! \r\n");}
	
	
	
//	/*##5 Open the text files object with read access ##############*/
//	retSD = f_open(&fil, filename, FA_READ);
//	if(retSD) {log_info(" open file error : %d\r\n",retSD); }
//	else {log_info(" open file sucess!!! \r\n");}
//	
//	/*##6 Read data from the text files ##########################*/
//	retSD = f_read(&fil, rtext, sizeof(rtext), (UINT*)&bytesread);
//	if(retSD){log_info(" read error!!! %d\r\n",retSD);}
//	else { log_info(" read sucess!!! \r\n"); log_info(" read Data : %s\r\n",rtext); }
//	
//	/*##7 Close the open text files ############################*/
//	retSD = f_close(&fil);
//	if(retSD) {log_info(" close error!!! %d\r\n",retSD);}		
//	else {log_info(" close sucess!!! \r\n");}
//	
//	/*##8 Compare read data with the expected data ############*/
//	if(bytesread == byteswritten) { log_info(" FatFs is working well!!!\r\n"); }	
}

void Fatfs_RW_test(void)
{
	uint32_t systick=0;
	float speed=0;
	
	log_info("\r\n ****** FatFs Example ******\r\n\r\n");
	/*##1 Register the file system object to the FatFs module ##############*/
	retSD = f_mount(&fs, "", 1);
	if(retSD)
	{
		log_info(" mount error : %d \r\n",retSD);
		Error_Handler();
	}
	else
	{
		log_info(" mount sucess!!! \r\n");
	}
	
	/*##2 Create and Open new text file objects with write access ######*/
	retSD = f_open(&fil, filename, FA_CREATE_ALWAYS | FA_WRITE);
	if(retSD) 
	{
		log_info(" open file error : %d\r\n",retSD);
	}		
	else 
	{
		log_info(" open file sucess!!! \r\n");
	}
	
	/*##3 Write data to the text files ###############################*/
	systick=HAL_GetTick();
	retSD = f_write(&fil, wtext, sizeof(wtext), (void *)&byteswritten);
	systick=HAL_GetTick()-systick;
	if(systick!=0)
	{
		speed=1.0*sizeof(wtext)/1024/1024/(systick/1000.0);
		
		log_info("retSD:%d,byteswritten:%d,used time:%d,wtext_len:%d,speed:%fMB/s\r\n",retSD,byteswritten,systick,sizeof(wtext),speed); 
	}
	
	if(retSD) 
	{
		log_info(" write file error : %d\r\n",retSD); 
	}
	else 
	{ 
		log_info(" write file sucess!!! \r\n"); 
		log_info(" write Data : %s\r\n",wtext); 
	}
	
	/*##4 Close the open text files ################################*/
	retSD = f_close(&fil);
	if(retSD)
	{
		log_info(" close error : %d\r\n",retSD);
	}
	else
	{
		log_info(" close sucess!!! \r\n");
	}
	
	/*##5 Open the text files object with read access ##############*/
	retSD = f_open(&fil, filename, FA_READ);
	if(retSD) 
	{
		log_info(" open file error : %d\r\n",retSD); 
	}
	else 
	{
		log_info(" open file sucess!!! \r\n");
	}
	
	/*##6 Read data from the text files ##########################*/
	retSD = f_read(&fil, rtext, sizeof(rtext), (UINT*)&bytesread);
	if(retSD)
	{
		log_info(" read error!!! %d\r\n",retSD);
	}
	else 
	{ 
		log_info(" read sucess!!! \r\n"); 
		log_info(" read Data : %s\r\n",rtext); 
	}
	
	/*##7 Close the open text files ############################*/
	retSD = f_close(&fil);
	if(retSD) 
	{
		log_info(" close error!!! %d\r\n",retSD);
	}		
	else 
	{
		log_info(" close sucess!!! \r\n");
	}
	
	/*##8 Compare read data with the expected data ############*/
	if(bytesread == byteswritten) 
	{ 
		log_info(" FatFs is working well!!!\r\n"); 
	}
	else
	{
		log_info(" bytesread:%d,byteswritten:%d\r\n",bytesread,byteswritten); 
	}		
}

//HAL_StatusTypeDef SD_DMAConfigRx(SD_HandleTypeDef *hsd)
//{
//	HAL_StatusTypeDef status = HAL_ERROR;
//	
////	hdma_sdio_rx.Instance = DMA2_Stream3;
////    hdma_sdio_rx.Init.Channel = DMA_CHANNEL_4; 
////	//hdma_sdio_rx.Init.Request = DMA_REQUEST_7;
////	hdma_sdio_rx.Init.Direction = DMA_PERIPH_TO_MEMORY; 
////	hdma_sdio_rx.Init.PeriphInc = DMA_PINC_DISABLE; 
////	hdma_sdio_rx.Init.MemInc = DMA_MINC_ENABLE; 
////	hdma_sdio_rx.Init.PeriphDataAlignment = DMA_PDATAALIGN_WORD; 
////	hdma_sdio_rx.Init.MemDataAlignment = DMA_MDATAALIGN_WORD; 
////	hdma_sdio_rx.Init.Mode = DMA_PFCTRL; 
////	hdma_sdio_rx.Init.Priority = DMA_PRIORITY_LOW;
//	
//	//__HAL_LINKDMA(hsd,hdmarx,hdma_sdio_rx);
//	//HAL_DMA_Abort(&hdma_sdio_rx);
//	HAL_DMA_DeInit(&hdma_sdio_tx);//注意这里！！！ DeInit的是另一个通道！！！
//	//status = HAL_DMA_Init(&hdma_sdio_rx);
//	return (status);
//}

//HAL_StatusTypeDef SD_DMAConfigTx(SD_HandleTypeDef *hsd)
//{
//	HAL_StatusTypeDef status;
//	
////	hdma_sdio_tx.Instance = DMA2_Stream6;
////    hdma_sdio_tx.Init.Channel = DMA_CHANNEL_4;
////	//hdma_sdio_tx.Init.Request = DMA_REQUEST_7;
////	hdma_sdio_tx.Init.Direction = DMA_MEMORY_TO_PERIPH;
////	hdma_sdio_tx.Init.PeriphInc = DMA_PINC_DISABLE;
////	hdma_sdio_tx.Init.MemInc = DMA_MINC_ENABLE;
////	hdma_sdio_tx.Init.PeriphDataAlignment = DMA_PDATAALIGN_WORD;
////	hdma_sdio_tx.Init.MemDataAlignment = DMA_MDATAALIGN_WORD;
////	hdma_sdio_tx.Init.Mode = DMA_PFCTRL;//这里 NORMAL或其他都可以，无所谓
////	hdma_sdio_tx.Init.Priority = DMA_PRIORITY_LOW;
//	
//	//__HAL_LINKDMA(hsd, hdmatx, hdma_sdio_tx);
//	//HAL_DMA_Abort(&hdma_sdio_tx);
//	HAL_DMA_DeInit(&hdma_sdio_rx); //注意这里！！！ DeInit的是另一个通道！！！
//	//status = HAL_DMA_Init(&hdma_sdio_tx);
//	return (status);
//}






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
  MX_DMA_Init();
  MX_SDIO_SD_Init();
  MX_UART7_Init();
  MX_FATFS_Init();
  /* USER CODE BEGIN 2 */
	log_info("Hardware Init OK!\r\n");
	Get_CardInfo();
	//log_info("wtext_len:%d\r\n",sizeof(wtext));
	//SD_EraseTest();
	//SD_Write_Read_Test();
	//Fatfs_RW_test();
	Fatfs_RW_test2();
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
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

  /** Configure the main internal regulator output voltage
  */
  __HAL_RCC_PWR_CLK_ENABLE();
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);
  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_BYPASS;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLM = 25;
  RCC_OscInitStruct.PLL.PLLN = 360;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
  RCC_OscInitStruct.PLL.PLLQ = 8;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }
  /** Activate the Over-Drive mode
  */
  if (HAL_PWREx_EnableOverDrive() != HAL_OK)
  {
    Error_Handler();
  }
  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV4;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV2;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_5) != HAL_OK)
  {
    Error_Handler();
  }
}

/* USER CODE BEGIN 4 */

/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  __disable_irq();
  while (1)
  {
  }
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

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
