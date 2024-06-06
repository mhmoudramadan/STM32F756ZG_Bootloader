/// \file bootloader.h
/// \author mahmoud Ramadan(Owner)
/// \date 2023-04-01
/// \brief Implementing Bootloader APIs  

#ifndef BOOTLOADER_H
#define BOOTLOADER_H
/************** Global Includes**************/
#include "LSTD_TYPES.h"
#include <stdio.h>
#include <string.h>
#include <stdarg.h>
#include "usart.h"
#include "crc.h"

/*********** Macro declerations**********/
// UART Used for debug and communication
#define BL_DEBUG_UART															&huart2
#define BL_HOST_COMMUNICATION_UART								&huart6
#define BL_CRC_ENGINE															&hcrc

#define DEBUG_INFO_DISABLE												0
#define DEBUG_INFO_ENABLE													1
#define BL_DEBUG_INFO															(DEBUG_INFO_ENABLE)

// Host RX Buffer
#define BL_HOST_BUFFER_RX_LENGTH									200U
#define BL_NO_OF_SUPPORTED_CMD										15U

// Debug Info
#define BL_EN_UART_DEBUG_MSG											0x00
#define BL_EN_CAN_DEBUG_MSG												0x01
#define BL_DEBUG_METHOD														(BL_EN_UART_DEBUG_MSG)

// BL uart supported Commands
#define CBL_GET_HELP_CMD  												0x00
#define CBL_GET_VERSION_CMD  											0x01
#define CBL_GET_ID_CMD  													0x02
#define CBL_READ_MEMORY_CMD  											0x11
#define CBL_GO_TO_ADDR_CMD  											0x21
#define CBL_WRITE_MEMORY_CMD  										0x31
#define CBL_ERASE_CMD  														0x43
#define CBL_EXTENDED_ERASE_CMD  									0x44
#define CBL_SPECIAL_CMD  													0x50
#define CBL_EXTENDED_SPECIAL_CMD  								0x51
#define CBL_WRITE_PROTECT_CMD  										0x63
#define CBL_WRITE_UNPROTECT_CMD  									0x73
#define CBL_READOUT_PROTECT_CMD  									0x82
#define CBL_READOUT_UNPROTECT_CMD 								0x92
#define CBL_CHECK_SUM_CMD  												0xA1


#define CBL_SEND_ACK															0x79
#define CBL_SEND_NACK															0x1F

/******* start address of sector 1 ****/
#define FLASH_SECTOR1_BASE_ADDRESS								0x08008000

/***Crc Macros ****/
#define CRC_VERFIY_SUCCESS   											0x00
#define CRC_VERIFY_FAILED		 											0x01
#define CRC_SIZE_BYTE				 											4U

/****Get Version data ***/
#define CBL_VENDOR_ID															200U
#define CBL_SW_MAJOR_VERSION											1U
#define CBL_SW_MINOR_VERSION											0U
#define CBL_SW_PATCH_VERSION											0U


#define ADDRESS_IS_VALID        									0x01
#define ADDRESS_IS_INVALID												0x00
/********Go Cmd Info*********/
#define STM32F756_SRAM1_SIZE    									((240U * 1024U) - 1U)
#define STM32F756_SRAM2_SIZE    									((16U * 1024U) - 1U)
#define STM32F756_FLASH_SIZE											((1024U * 1024U) -1U)
/****memory End (base + size)****/
#define STM32F756_SRAM1_END												(SRAM1_BASE +STM32F756_SRAM1_SIZE)
#define STM32F756_SRAM2_END												(SRAM1_BASE +STM32F756_SRAM2_SIZE)
#define STM32F756_FLASH_END												(FLASH_BASE + STM32F756_FLASH_SIZE)

/***********Write memory Info***********/
#define FLASH_WRITE_STATUS_PASS										0x01
#define FLASH_WRITE_STATUS_FAIL										0x00

#define FLASH_SUCCESS_ERASE												0x01
#define FLASH_FAILED_ERASE												0x00

#define FLASH_MAX_SECTORS													8U
#define SECTOR_IS_VALID														0x01
#define SECTOR_IS_INVALID													0x00

#define FLASH_MASS_ERASE 													0xFF
#define HAL_ERASE_SUCCESS   											0xFFFFFFFFU

/***** change RDP*****/
#define ROP_LEVEL_CHANGE_INVALID  								0x00
#define ROP_LEVEL_CHANGE_VALID										0x01
/****ROP levels*******/
#define ROP_LV_0																	0x00
#define ROP_LV_1																	0x01
#define ROP_LV_2																	0x02

/***** EN/DIS WRP protection ********/
#define WRP_STATE_EN															(OB_WRPSTATE_ENABLE)		
#define WRP_STATE_DIS															(OB_WRPSTATE_DISABLE) 

#define WRP_ACTIVATION_SUCCESS										0x00
#define WRP_ACTIVATION_FAILED											0x01

/********* Macro Functions Declerations****/


/*********** Data Type Declerations*****/
/****Bootloader status enum***/
typedef enum tagE__Bl_Status{
	BL_NACK=0U,
	BL_ACK=1U
}Bl_Status;

typedef void (*Ptr_JumpAdd) (void);
typedef void (*Ptr_app) (void);

/********* Software Function Prototype*******/
/**function Bl_Print_Msg -> vardic function
**@param[in] format pointer to data 
*/
void Bl_Print_Msg(char *format,...); 
/**function Bl_Uart_Fetch_Host_Cmd
**@param[in] format pointer 
**@return BL_ACK if there is no error else return BL_NACK
*/
Bl_Status Bl_Uart_Fetch_Host_Cmd(void);

#endif /*BOOTLOADER_H*/
