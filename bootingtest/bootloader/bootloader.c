/******** file bootloader.c
*		Created on : Apr 1 , 2023   1:15:28 AM
*		Topic : Bootloader 
*   Author : Mahmoud Ramadan
*/

/************Global Includes*************/
#include "bootloader.h"

/********* Static Function Prototypes************/
/*****BL_uint8CRC_Verify 
*@param[in] pdata pointer to data
*@param[in] datalen data length to verify
*@param[in] host_crc  crc passed by host
return CRC_verify_passed if success else  CRC_verify_Failed
**/
static uint8	BL_uint8CRC_Verify(uint8*pdata , uint32 datalen , uint32 host_crc);

/*****BL_VidSendAck 
**@param[in] bl_reply_len bootloader reply length
**/
static void BL_VidSendAck(uint8 bl_reply_len);

/*****BL_VidSendNack 
**@param[in] 
**/
static void BL_VidSendNack(void);

/*****BL_VidSendReplyTo_Host 
**@param[in] host_buffer pointer to data
**@param[in] data_len length of data
**/
static void BL_VidSendReplyTo_Host(uint8 * host_buffer, uint32 data_len);

/*****Host_uint8AddressVerification 
**@param[in] address 
**@return address verification valid or Not
**/
static	uint8 Host_uint8AddressVerification(uint32 address);

/*****Flash_Mem_Write_Payload 
**@param[in] payload pointer to data
**@param[in] payload_address Address
**@param[in] payload_address payload length
**/
static uint8 Flash_Mem_Write_Payload(uint8 *payload , uint32 payload_address , uint16 payload_len);

/*****Flash_Mem_Write_Payload 
**@param[in] sector_number sector for start from it
**@param[in] numberofsectors  No.of sectors to erase
**/
static uint8 Perform_uint8FlashErase(uint8 sector_number , uint8 numberofsectors);

/*****STM32F756_Get_RDP_LEVEL
**@param[in] 
**/
static uint8 STM32F756_Get_RDP_LEVEL(void);

/*****STM32F756_Get_WDP_LEVEL
**@param[in] 
**/
static uint8 STM32F756_Get_WRP_LEVEL(void);

/*****STM32F756_Change_ROP_LV
**@param[in] rop_level
**/
static uint8 STM32F756_Change_ROP_LV(uint32 rop_level);
/*****STM32F756_Change_WRP_Activation
**@param[in] sectors
**@param[in] sector_code
**@param[in] wrp_level
**/
static uint8 STM32F756_Change_WRP_Activation(uint8 sectors ,uint8 sector_code,uint8 wrp_level);

/*****BL_Jump_To_App
**@param[in] 
**/
static void BL_Jump_To_App(void);

/*****BL_VidGetHelp 
**@description 
	Gets the version and the allowed commands supported by the current version of the protocol.
**@param[in] Host_buffer pointer to data
**/
static void BL_VidGetHelp(uint8 *Host_buffer);

/*****BL_VidGetVersion 
**@description 
	Gets the protocol version.
**@param[in] Host_buffer pointer to data
**/
static void BL_VidGetVersion(uint8 *Host_buffer);

/*****BL_VidGetID 
**@description 
	Gets the chip ID.
**@param[in] Host_buffer pointer to data
**/
static void BL_VidGetID(uint8 *Host_buffer);

/*****BL_VidReadMemory
**@description 
	Reads up to 256 bytes of memory starting from an address specified by the application
**@param[in] Host_buffer pointer to data
**/
static void BL_VidReadMemory(uint8 *Host_buffer);

/*****BL_VidGoToAddr
**@description 
	Jumps to user application code located in the internal flash memory or in the SRAM.
**@param[in] Host_buffer pointer to data
**/
static void BL_VidGoToAddr(uint8 *Host_buffer);

/*****BL_VidWriteMemory
**@description 
	Writes up to 256 bytes to the RAM or flash memory starting from an address specified by the application.
**@param[in] Host_buffer pointer to data
**/
static void BL_VidWriteMemory(uint8 *Host_buffer);

/*****BL_VidErase 
**@description 
	Erases from one to all the flash memory pages
**@param[in] Host_buffer pointer to data
**/
static void BL_VidErase(uint8 *Host_buffer);

/*****BL_VidExtendedErase 
**@description 
	Erases from one to all the flash memory pages using two-byte addressing mode (available only for USART bootloader v3.0 and
higher).
**@param[in] Host_buffer pointer to data
**/
static void BL_VidExtendedErase(uint8 *Host_buffer);

/*****BL_VidSpecial 
**@description 
	Generic command that allows to add new features depending on the product constraints, without adding a new command for every feature
**@param[in] Host_buffer pointer to data
**/
static void BL_VidSpecial(uint8 *Host_buffer);

/*****BL_VidExtendedSpecial
**@description 
	Generic command that allows the user to send more data compared to the Special command
**@param[in] Host_buffer pointer to data
**/
static void BL_VidExtendedSpecial(uint8 *Host_buffer);

/*****BL_VidWriteProtect
**@description 
	Enables the write protection for some sectors.
**@param[in] Host_buffer pointer to data
**/
static void BL_VidWriteProtect(uint8 *Host_buffer);

/*****BL_VidWriteUnProtect
**@description 
	Disables the write protection for all flash memory sectors
**@param[in] Host_buffer pointer to data
**/
static void BL_VidWriteUnProtect(uint8 *Host_buffer);

/*****BL_VidReadOutProtect 
**@description
	Enables the read protection.
**@param[in] Host_buffer pointer to data
**/
static void BL_VidReadOutProtect(uint8 *Host_buffer);

/*****BL_VidReadOutUnProtect 
**@description
	Disable the read protection.
**@param[in] Host_buffer pointer to data
**/
static void BL_VidReadOutUnProtect(uint8 *Host_buffer);

/*****BL_VidCheckSum 
**@description 
	Computes a CRC value on a given memory area with a size multiple of 4 bytes.
**@param[in] Host_buffer pointer to data
**/
static void BL_VidCheckSum(uint8 *Host_buffer);



/********* Global Variables Declerations************/
static uint8 BL_Host_Buf[BL_HOST_BUFFER_RX_LENGTH];  // Host Buffer
// Array of Bootloader Supported Commands 
static uint8 Bl_Supported_Commands[BL_NO_OF_SUPPORTED_CMD] ={
	CBL_GET_HELP_CMD,
  CBL_GET_VERSION_CMD, 			
  CBL_GET_ID_CMD,					
  CBL_READ_MEMORY_CMD, 			
  CBL_GO_TO_ADDR_CMD,			
  CBL_WRITE_MEMORY_CMD, 		
  CBL_ERASE_CMD,		
  CBL_EXTENDED_ERASE_CMD, 	
  CBL_SPECIAL_CMD,	
  CBL_EXTENDED_SPECIAL_CMD, 
  CBL_WRITE_PROTECT_CMD,  		
  CBL_WRITE_UNPROTECT_CMD,  	
  CBL_READOUT_PROTECT_CMD,  	
  CBL_READOUT_UNPROTECT_CMD, 
  CBL_CHECK_SUM_CMD,  				
};


/********* Software Function Definition *******/
/**function Bl_Print_Msg -> vardic function
@param[in] format pointer to data 
*/
void Bl_Print_Msg(char *format,...)
{
	char ui_locmsg[200] = {0U};
	/*********hold info need by vardiac function****/
	va_list	args;
	/********enable access******/
	va_start(args , format);
	/***********write data*****/
	vsprintf(ui_locmsg,format,args);
	#if BL_DEBUG_METHOD == BL_EN_UART_DEBUG_MSG
	/***********transmit data through uart*******/
	HAL_UART_Transmit(BL_DEBUG_UART,(uint8*)ui_locmsg,sizeof(ui_locmsg),HAL_MAX_DELAY);
	#elif BL_DEBUG_METHOD == BL_EN_CAN_DEBUG_MSG
	/***********transmit data through CAN*******/
	#endif
	va_end(args);
}
/**function Bl_Uart_Fetch_Host_Cmd 
*@param[in] format pointer 
*@return BL_ACK if there is no error else return BL_NACK
*/
Bl_Status Bl_Uart_Fetch_Host_Cmd(void)
{
	Bl_Status	loc_bl_status = BL_NACK;
	HAL_StatusTypeDef	loc_status = HAL_ERROR;
	uint8 ui_data_len = 0U;
	
	/******** clear Host buffer******/
	memset(BL_Host_Buf,0,BL_HOST_BUFFER_RX_LENGTH);
	/********** Read Length of cmd packet******/
	// Receive command length "cmd code + (optional)info + crc"
	loc_status = HAL_UART_Receive(BL_HOST_COMMUNICATION_UART , BL_Host_Buf , 1U , HAL_MAX_DELAY); 
	if (HAL_OK != loc_status)
	{
		// TODO error handling
		loc_bl_status = BL_NACK;
	}else{
		ui_data_len = BL_Host_Buf[0U];  // store command packet length 
		/********** Read command Info**********/
		loc_status = HAL_UART_Receive(BL_HOST_COMMUNICATION_UART , &BL_Host_Buf[1U] , ui_data_len , HAL_MAX_DELAY);
		if(HAL_OK != loc_status)
			{
					// TODO error handling
				loc_bl_status = BL_NACK;
			} else{
				switch(BL_Host_Buf[1U])
				 {
					case CBL_GET_HELP_CMD:
					BL_VidGetHelp(BL_Host_Buf);	
					loc_bl_status = BL_ACK;
						break;
					case CBL_GET_VERSION_CMD:
					BL_VidGetVersion(BL_Host_Buf);
					loc_bl_status = BL_ACK;
						break;
					case CBL_GET_ID_CMD:
					BL_VidGetID(BL_Host_Buf);
					loc_bl_status = BL_ACK;
						break;
					case CBL_READ_MEMORY_CMD:
					Bl_Print_Msg("Read memory Cmd Received \r\n");
					BL_VidReadMemory(BL_Host_Buf);
					loc_bl_status = BL_ACK;
						break;
					case CBL_GO_TO_ADDR_CMD:
					BL_VidGoToAddr(BL_Host_Buf);
					loc_bl_status = BL_ACK;
						break;
					case CBL_WRITE_MEMORY_CMD:
					Bl_Print_Msg("Write memory Cmd Received \r\n");
					BL_VidWriteMemory(BL_Host_Buf);
					loc_bl_status = BL_ACK;
						break;
					case CBL_ERASE_CMD:
					BL_VidErase(BL_Host_Buf);
					loc_bl_status = BL_ACK;
						break;
					case CBL_EXTENDED_ERASE_CMD:
					Bl_Print_Msg("Extended Erase Cmd Received \r\n");
					BL_VidExtendedErase(BL_Host_Buf);
					loc_bl_status = BL_ACK;
						break;
					case CBL_SPECIAL_CMD:
					Bl_Print_Msg("Special Cmd Received \r\n");
					BL_VidSpecial(BL_Host_Buf);
					loc_bl_status = BL_ACK;
						break;
					case CBL_EXTENDED_SPECIAL_CMD:
					Bl_Print_Msg("Extended special Cmd Received \r\n");
					BL_VidExtendedSpecial(BL_Host_Buf);
					loc_bl_status = BL_ACK;
						break;
					case CBL_WRITE_PROTECT_CMD:
					Bl_Print_Msg("Write protect Cmd Received \r\n");
					BL_VidWriteMemory(BL_Host_Buf);
					loc_bl_status = BL_ACK;
						break;
					case CBL_WRITE_UNPROTECT_CMD:
					Bl_Print_Msg("Write unprotect Cmd Received \r\n");
					BL_VidWriteUnProtect(BL_Host_Buf);
					loc_bl_status = BL_ACK;
					break;
					case CBL_READOUT_PROTECT_CMD:
					Bl_Print_Msg("Readout protect Cmd Received \r\n");
					BL_VidReadOutProtect(BL_Host_Buf);
					loc_bl_status = BL_ACK;
						break;
					case CBL_READOUT_UNPROTECT_CMD:
					Bl_Print_Msg("Readout unprotect Cmd Received \r\n");
					BL_VidReadOutUnProtect(BL_Host_Buf);
					loc_bl_status = BL_ACK;
						break;
					case CBL_CHECK_SUM_CMD:
					Bl_Print_Msg("Check sum Cmd Received \r\n");
					BL_VidCheckSum(BL_Host_Buf);
					loc_bl_status = BL_ACK;
						break;
					default:
					Bl_Print_Msg("Error in Host communication invalid command \r\n");
					loc_bl_status = BL_NACK;
						break;
				 }
			}
	}
	return loc_bl_status;
}



/********* Static Function Definitions************/
/*****BL_uint8CRC_Verify 
*@param[in] pdata pointer to data
*@param[in] datalen data length to verify
*@param[in] host_crc  crc passed by host
return CRC_verify_passed if success else  CRC_verify_Failed
**/
static uint8	BL_uint8CRC_Verify(uint8*pdata , uint32 datalen , uint32 host_crc)
{
	uint8 loc_crc_status = CRC_VERIFY_FAILED;
	uint32 mc_crc_calc =0U;
	uint8	loc_data_counter =0U;
	uint32 loc_dataBuf = 0U;
	/*********calculate Crc for first Two bytes to be compared***/
	for (loc_data_counter =0U; loc_data_counter<datalen ; loc_data_counter++)
	{
		loc_dataBuf = (uint32)pdata[loc_data_counter]; // assign single byte of buffer for crc check
		mc_crc_calc = HAL_CRC_Accumulate(BL_CRC_ENGINE , &loc_dataBuf , 1U); // calc for one byte every time
	}
	
	/**********Reset Crc engine***/
	__HAL_CRC_DR_RESET(BL_CRC_ENGINE);
	
	/******Check crc values*****/
	if(mc_crc_calc == host_crc){
	/*CRC success*/
		loc_crc_status = CRC_VERFIY_SUCCESS;
	}else{
		/**crc failed*/
		loc_crc_status = CRC_VERIFY_FAILED;
	}
	return loc_crc_status;
}
/*****BL_VidSendAck 
**@param[in] bl_reply_len bootloader reply length
**/
static void BL_VidSendAck(uint8 bl_reply_len)
{
	uint8 loc_Ack[2U] ={0U};
	loc_Ack[0U] = CBL_SEND_ACK;
	loc_Ack[1U] = bl_reply_len;
	HAL_UART_Transmit(BL_HOST_COMMUNICATION_UART,(uint8*)loc_Ack,2U,HAL_MAX_DELAY);
}
/*****BL_VidSendNack 
**@param[in] 
**/
static void BL_VidSendNack(void)
{
	uint8 loc_Nack = CBL_SEND_NACK;
	HAL_UART_Transmit(BL_HOST_COMMUNICATION_UART,&loc_Nack,1U,HAL_MAX_DELAY); // transmit NACK
}
/*****BL_VidSendReplyTo_Host 
**@param[in] host_buffer pointer to data
**@param[in] data_len length of data
**/
static void BL_VidSendReplyTo_Host(uint8 * host_buffer, uint32 data_len){
	HAL_UART_Transmit(BL_HOST_COMMUNICATION_UART , host_buffer,data_len,HAL_MAX_DELAY);
}

/*****Host_uint8AddressVerification 
**@param[in] address 
**@return address verification valid or Not
**/
static	uint8 Host_uint8AddressVerification(uint32 address)
{
	uint8	address_verification = ADDRESS_IS_INVALID;
	if(((address >= SRAM1_BASE) && (address <=STM32F756_SRAM1_END)) ||
		((address >= SRAM2_BASE) && (address <=STM32F756_SRAM2_END)) ||
	((address >= FLASH_BASE) && (address <=STM32F756_FLASH_END)))
	{
		address_verification = ADDRESS_IS_VALID; 
	}else{
		address_verification = ADDRESS_IS_INVALID;
	}
	return address_verification;
}

/*****Flash_Mem_Write_Payload 
**@param[in] payload pointer to data
**@param[in] payload_address Address
**@param[in] payload_address payload length
**/
static uint8 Flash_Mem_Write_Payload(uint8 *payload , uint32 payload_address , uint16 payload_len)
{
	HAL_StatusTypeDef loc_status = HAL_ERROR;
	uint16	loc_payload_counter =0U;
	uint8  loc_flash_status  = FLASH_WRITE_STATUS_FAIL;
	/****Flash control register access 
	* first -> unlock flash
	* End   -> Lock flash
	*/
	/******unlock flash****/
	loc_status = HAL_FLASH_Unlock();
	if(HAL_OK != loc_status){
	loc_flash_status = FLASH_WRITE_STATUS_FAIL;
	}else{
	for(loc_payload_counter = 0U ; loc_payload_counter<payload_len;loc_payload_counter++ )
	{
		/********Program Flash one byte*******/
		loc_status = HAL_FLASH_Program(FLASH_TYPEPROGRAM_BYTE ,payload_address + loc_payload_counter,payload[loc_payload_counter]);
		if(HAL_OK !=  loc_status){
			loc_flash_status = FLASH_WRITE_STATUS_FAIL;
		break;
		}else{
		loc_flash_status = FLASH_WRITE_STATUS_PASS;
		}
	}
}
	if ((FLASH_WRITE_STATUS_PASS == loc_flash_status) && (HAL_OK == loc_status)){
		loc_status = HAL_FLASH_Lock();
		if(HAL_OK !=loc_status ){
		loc_flash_status = FLASH_WRITE_STATUS_FAIL;
		}else{
		loc_flash_status = FLASH_WRITE_STATUS_PASS;
		}
	}
	return loc_flash_status;
}
/*****Flash_Mem_Write_Payload 
**@param[in] sector_number sector for start from it
**@param[in] numberofsectors  No.of sectors to erase
**/
static uint8 Perform_uint8FlashErase(uint8 sector_number , uint8 numberofsectors)
{
	HAL_StatusTypeDef	loc_status =HAL_ERROR;
	uint8 sector_validity = SECTOR_IS_INVALID;
	FLASH_EraseInitTypeDef  Eraseinit_;
	uint8 Remaining_Sector =0U;
	uint32 sectorerror_ =0U;
	/****** check for no of sectors *********/
	if((numberofsectors > FLASH_MAX_SECTORS)){
		/*** sectors out of Rang */		
	sector_validity = SECTOR_IS_INVALID;  
	}else{
			if((sector_number <= (FLASH_MAX_SECTORS - 1U)) || (FLASH_MASS_ERASE == sector_number )){
				/********* Check Mass Erase*******/
				if((FLASH_MASS_ERASE == sector_number )){
					/******* perform mass Erase ****/
				  Eraseinit_.TypeErase = FLASH_TYPEERASE_MASSERASE;
						/*****Log message***/	
#if DEBUG_INFO_ENABLE == BL_DEBUG_INFO
		Bl_Print_Msg("Flash Mass Erase Activation \r\n");
#endif
					
				}else{
#if DEBUG_INFO_ENABLE == BL_DEBUG_INFO
		Bl_Print_Msg("Flash Sector Erase Activation \r\n");
#endif
					Remaining_Sector = FLASH_MAX_SECTORS - sector_number ;
					if(numberofsectors > Remaining_Sector){
						numberofsectors = Remaining_Sector;    /***** assign remaining value to NoofSectors**/
					}
				/******* perform mass Erase ****/
				  Eraseinit_.TypeErase = FLASH_TYPEERASE_SECTORS;
					/********* initial flash sector*****/
					Eraseinit_.Sector = sector_number; 
					/*************** No of sectors to erase**/
					Eraseinit_.NbSectors = numberofsectors;  
				}	
					Eraseinit_.VoltageRange = FLASH_VOLTAGE_RANGE_3 ; /* Device operating range: 2.7V to 3.6V */
				/********unlock flash********/
					loc_status = HAL_FLASH_Unlock();
				/*********** perform mass  or sector erase ********/
					loc_status = HAL_FLASHEx_Erase(&Eraseinit_,&sectorerror_);
				if((HAL_ERASE_SUCCESS == sectorerror_)){
					sector_validity = FLASH_SUCCESS_ERASE;
				}else{
					sector_validity = FLASH_FAILED_ERASE;
				}
				/*********lock flash*****/
				loc_status = HAL_FLASH_Lock();
		}
	  else{
					sector_validity = FLASH_FAILED_ERASE;
		}
	}
	return sector_validity;
}
/*****STM32F756_Get_RDP_LEVEL
**@param[in] 
**/
static uint8 STM32F756_Get_RDP_LEVEL(void){
	FLASH_OBProgramInitTypeDef OBP_;
	/*****Get the Option byte configuration******/
	HAL_FLASHEx_OBGetConfig(&OBP_);
	/*******return RDP level********/
	return (uint8)(OBP_.RDPLevel);
}
/*****STM32F756_Get_WDP_LEVEL
**@param[in] 
**/
static uint8 STM32F756_Get_WRP_LEVEL(void){
	FLASH_OBProgramInitTypeDef OBP_;
	/*****Get the Option byte configuration******/
	HAL_FLASHEx_OBGetConfig(&OBP_);
	/*******return WRP level********/
	return (uint8)(OBP_.WRPSector);
}
/*****STM32F756_Change_ROP_LV
**@param[in] 
**/
static uint8 STM32F756_Change_ROP_LV(uint32 rop_level){
	HAL_StatusTypeDef	loc_status = HAL_ERROR;
	FLASH_OBProgramInitTypeDef obp_;
	uint8 Rop_level = ROP_LEVEL_CHANGE_INVALID;
	
	/*****unlock Flash option control register access******/
	loc_status = HAL_FLASH_OB_Unlock();
	if(HAL_OK != loc_status){
		Rop_level = ROP_LEVEL_CHANGE_INVALID;
			/*****Log message***/	
#if DEBUG_INFO_ENABLE == BL_DEBUG_INFO
		Bl_Print_Msg("Failed To unlock Flash OP Register \r\n");
#endif
	}else{
				/*****Log message***/	
#if DEBUG_INFO_ENABLE == BL_DEBUG_INFO
		Bl_Print_Msg("Passed To unlock Flash OP Register \r\n");
#endif
		obp_.OptionType  = OPTIONBYTE_RDP; /*!< RDP option byte configuration  */
		obp_.RDPLevel = Rop_level;
		/*** program option bytes*******/
		loc_status = HAL_FLASHEx_OBProgram(&obp_);
		if(HAL_OK !=  loc_status){
#if DEBUG_INFO_ENABLE == BL_DEBUG_INFO
		Bl_Print_Msg("Failed program option bytes \r\n");
#endif
			/*********** lock OB*****/
			loc_status = HAL_FLASH_OB_Lock();
			Rop_level = ROP_LEVEL_CHANGE_INVALID;
		}else{
#if DEBUG_INFO_ENABLE == BL_DEBUG_INFO
		Bl_Print_Msg("passed program option bytes \r\n");
#endif
			/**** lanuch OB *****/
			loc_status = HAL_FLASH_OB_Launch();
			if(HAL_OK != loc_status){
			Rop_level = ROP_LEVEL_CHANGE_INVALID;
			}else{
			/********** lock ob flash **/
				loc_status = HAL_FLASH_OB_Lock();
			 if(HAL_OK != loc_status){
			 Rop_level = ROP_LEVEL_CHANGE_INVALID;
			 }else{
			 Rop_level = ROP_LEVEL_CHANGE_VALID;
#if DEBUG_INFO_ENABLE == BL_DEBUG_INFO
				 Bl_Print_Msg("program ROP to level : 0x%X \r\n",Rop_level);
#endif
			 }	
			}
		}
	}
	return Rop_level;
}
/*****STM32F756_Change_WRP_Activation
**@param[in] sectors
**@param[in] sector_code
**@param[in] wrp_level
**/
static uint8 STM32F756_Change_WRP_Activation(uint8 sectors ,uint8 sector_code,uint8 wrp_level){
	HAL_StatusTypeDef	loc_status = HAL_ERROR;
	FLASH_OBProgramInitTypeDef obp_;
	uint8 wrp_activation = WRP_ACTIVATION_FAILED;
	
	obp_.OptionType = OPTIONBYTE_WRP;
	obp_.WRPState =  wrp_level;
	obp_.WRPSector = sector_code;
	
	if(sectors > FLASH_MAX_SECTORS){
#if DEBUG_INFO_ENABLE == BL_DEBUG_INFO
			Bl_Print_Msg("you should enter valid Number sectors");
#endif
		return wrp_activation;  // Exit function 
	}
	/****** perform flash unlock******/
	loc_status = HAL_FLASH_OB_Unlock();
	if(loc_status != HAL_OK){
	wrp_activation = WRP_ACTIVATION_FAILED;
#if DEBUG_INFO_ENABLE == BL_DEBUG_INFO
		Bl_Print_Msg("Failed To unlock Flash");
#endif
	} 
	else{
						/*****Log message***/	
#if DEBUG_INFO_ENABLE == BL_DEBUG_INFO
		Bl_Print_Msg("Passed To unlock Flash OP Register \r\n");
#endif
		/*** program option bytes*******/
		loc_status = HAL_FLASHEx_OBProgram(&obp_);
		if(HAL_OK !=  loc_status){
#if DEBUG_INFO_ENABLE == BL_DEBUG_INFO
		Bl_Print_Msg("Failed program option bytes \r\n");
#endif
	}else{
#if DEBUG_INFO_ENABLE == BL_DEBUG_INFO
		Bl_Print_Msg("passed program option bytes \r\n");
#endif
			/**** lanuch OB *****/
			loc_status = HAL_FLASH_OB_Launch();
			if(HAL_OK != loc_status){
			wrp_activation = WRP_ACTIVATION_FAILED;
			}else{
			/********** lock ob flash **/
				loc_status = HAL_FLASH_OB_Lock();
			 if(HAL_OK != loc_status){
			 wrp_activation = WRP_ACTIVATION_FAILED;
			 }else{
			 wrp_activation = WRP_ACTIVATION_SUCCESS;
#if DEBUG_INFO_ENABLE == BL_DEBUG_INFO
				 Bl_Print_Msg("Write Protection Activation : 0x%X \r\n",wrp_activation);
#endif
			 }	
			}
		}
	}
	return wrp_activation;
}

/*****BL_Jump_To_App
*@description store application start from sector 2
**@param[in] 
**/
static void BL_Jump_To_App(void){
	
	/********Value of Main Satck pointer***/
	uint32 MSP_Val = *((volatile uint32*) FLASH_SECTOR2_BASE_ADDRESS);
	
	/********** Reset handler of application****/ 
	uint32 MainAppAddress = *((volatile uint32*)(FLASH_SECTOR2_BASE_ADDRESS+4U));
	
	/*****Fetch of reset handler address******/
	Ptr_app Reset_handler_address = (Ptr_app) MainAppAddress;
	/** Set Main Stack Pointer function from CMSIS_ARMCC file**/
	__set_MSP(MSP_Val);
	
	/********** deinitialize modules to reset state**/
	HAL_RCC_DeInit();
	
	/********* Jump To Application **********/
	Reset_handler_address();
}

/**********************Bootloader Commands **********************************/
/*****BL_VidGetHelp 
**@param[in] Host_buffer pointer to data
**/
static void BL_VidGetHelp(uint8 *Host_buffer)
{
	uint8 bl_version[4U] = {CBL_VENDOR_ID , CBL_SW_MAJOR_VERSION ,CBL_SW_MINOR_VERSION,CBL_SW_PATCH_VERSION};
	uint16 Host_cmd_packet_len = 0U;
	uint32 Host_Crc32 = 0U;
	
	/***********Extract crc32 and cmd packet from host****/
	Host_cmd_packet_len = Host_buffer[0U] + 1U;
	Host_Crc32 = *((uint32 *)((Host_buffer + Host_cmd_packet_len) - CRC_SIZE_BYTE));
	
	/*****Log message***/	
#if DEBUG_INFO_ENABLE == BL_DEBUG_INFO
		Bl_Print_Msg(" Get Cmd Received \r\n");
#endif
	/****CRC verify check*****/
	if(CRC_VERFIY_SUCCESS == BL_uint8CRC_Verify((uint8 *)&Host_buffer[0],Host_cmd_packet_len - 4U ,Host_Crc32)){
#if DEBUG_INFO_ENABLE == BL_DEBUG_INFO
		Bl_Print_Msg("CRC Verification Successed \r\n");
#endif
		BL_VidSendAck(19U); // 4 bytes for bootloader version and 15 bytes for commands
		BL_VidSendReplyTo_Host((uint8*)&bl_version[0U],4U); // reply with version
		BL_VidSendReplyTo_Host((uint8*)&Bl_Supported_Commands[0U],15U); // reply with supported commands
#if DEBUG_INFO_ENABLE == BL_DEBUG_INFO
		Bl_Print_Msg("Bootloader Vendor is : %d.\r\n",bl_version[0U]);
		Bl_Print_Msg("Bootloader Version is: %d.%d.%d \r\n",bl_version[1U],bl_version[2U],bl_version[3U]);
#endif
	}
	else {
#if DEBUG_INFO_ENABLE == BL_DEBUG_INFO
		Bl_Print_Msg("CRC Verification Failed \r\n");
#endif
		BL_VidSendNack();
	}
}
/*****BL_VidGetVersion 
**@param[in] Host_buffer pointer to data
**/
static void BL_VidGetVersion(uint8 *Host_buffer)
{
	uint8 bl_version[4U] = {CBL_VENDOR_ID , CBL_SW_MAJOR_VERSION ,CBL_SW_MINOR_VERSION,CBL_SW_PATCH_VERSION};
	uint16 Host_cmd_packet_len = 0U;
	uint32 Host_Crc32 = 0U;
	
	/****Extract Crc32 and cmd packet from host***/
	Host_cmd_packet_len = Host_buffer[0U] +1U;  
	Host_Crc32 = *((uint32*)((Host_buffer + Host_cmd_packet_len) - CRC_SIZE_BYTE));  // derefernce to first byte of CRC

/*****Log message***/	
#if DEBUG_INFO_ENABLE == BL_DEBUG_INFO
		Bl_Print_Msg("Read Bootloader Version \r\n");
#endif
	/****CRC verify check*****/
	if (CRC_VERFIY_SUCCESS == BL_uint8CRC_Verify((uint8*)&Host_buffer[0U],Host_cmd_packet_len - 4U,Host_Crc32)){
		#if DEBUG_INFO_ENABLE == BL_DEBUG_INFO
		Bl_Print_Msg("CRC verification successed \r\n");
		#endif
	/** 1 - send ACK */
		BL_VidSendAck(4U); 
		/****send bootloader reply to Host****/
		BL_VidSendReplyTo_Host((uint8*)&bl_version[0U],4U);
		#if DEBUG_INFO_ENABLE == BL_DEBUG_INFO
		Bl_Print_Msg("Bootloader Vendor is : %d.\r\n",bl_version[0U]);
		Bl_Print_Msg("Bootloader Version is: %d.%d.%d \r\n",bl_version[1U],bl_version[2U],bl_version[3U]);
		#endif
		
	}else{
		#if DEBUG_INFO_ENABLE == BL_DEBUG_INFO
		Bl_Print_Msg("CRC verification Failed \r\n");
		#endif
	/***
		** Send Nack 
		***/
		BL_VidSendNack();
	}
}
/*****BL_VidGetID 
**@param[in] Host_buffer pointer to data
**/
static void BL_VidGetID(uint8 *Host_buffer)
{
	uint16 MC_Identification_Number = 0U;
	uint16 Host_cmd_packet_len = 0U;
	uint32 Host_Crc32 = 0U;
	
	/*******Extract Crc and cmd packet from host*****/
	Host_cmd_packet_len = Host_buffer[0U] +1U;
	Host_Crc32 =*((uint32*)((Host_buffer + Host_cmd_packet_len) - CRC_SIZE_BYTE));
	
	/*****Log message***/	
#if DEBUG_INFO_ENABLE == BL_DEBUG_INFO
		Bl_Print_Msg("Get MCU ID Cmd Received \r\n");
#endif
	
	/****** CRC Verification ***/
	if(CRC_VERFIY_SUCCESS == BL_uint8CRC_Verify((uint8 *)&Host_buffer[0U],Host_cmd_packet_len - 4U,Host_Crc32))
	{
#if DEBUG_INFO_ENABLE == BL_DEBUG_INFO
		Bl_Print_Msg("CRC Verification Successed \r\n");
#endif
		/******Get MC Identification Number ****/
		MC_Identification_Number = (uint16)(DBGMCU->IDCODE) & 0x00000FFF;
		/**********Reply To Host*******/
		BL_VidSendAck(2U);
		BL_VidSendReplyTo_Host((uint8 *)&MC_Identification_Number,2U);
	}else{
#if DEBUG_INFO_ENABLE == BL_DEBUG_INFO
		Bl_Print_Msg("CRC Verification Failed \r\n");
#endif
	BL_VidSendNack();
	}
	
}
/*****BL_VidReadMemory 
**@param[in] Host_buffer pointer to data
**/
static void BL_VidReadMemory(uint8 *Host_buffer)
{
}
/*****BL_VidGoToAddr 
**@param[in] Host_buffer pointer to data
**/
static void BL_VidGoToAddr(uint8 *Host_buffer)
{
	uint16 Host_cmd_packet_len = 0U;
	uint32 Host_Crc32 = 0U;
	uint32 Host_address =0U;
	uint8	 address_verification = ADDRESS_IS_INVALID;
	
	/*******Extract Crc and cmd packet from host*****/
	Host_cmd_packet_len = Host_buffer[0U] +1U;
	Host_Crc32 =*((uint32*)((Host_buffer + Host_cmd_packet_len) - CRC_SIZE_BYTE));
	
	/*****Log message***/	
#if DEBUG_INFO_ENABLE == BL_DEBUG_INFO
	Bl_Print_Msg("Jump To Specified Address Command Received \r\n");
#endif
	/****CRC verify check*****/
	if(CRC_VERFIY_SUCCESS == BL_uint8CRC_Verify((uint8 *)&Host_buffer[0],Host_cmd_packet_len - 4U ,Host_Crc32)){
#if DEBUG_INFO_ENABLE == BL_DEBUG_INFO
		Bl_Print_Msg("CRC Verification Successed \r\n");
#endif
		BL_VidSendAck(1U);
		/*******Extract address from packet*******/
		Host_address = *((uint32 *)&Host_buffer[2U]);
		/*****Check address Verification******/
		address_verification = Host_uint8AddressVerification(Host_address);
		if (ADDRESS_IS_VALID == address_verification)
		{
			/*****Log Msg*******/
#if DEBUG_INFO_ENABLE == BL_DEBUG_INFO
		Bl_Print_Msg("Address is valid \r\n");
#endif
			/*****Report Address verification successed****/
		  BL_VidSendReplyTo_Host((uint8 *)&address_verification,1U);
			/*****prepare address to jump******/
		  Ptr_JumpAdd jump_add = (Ptr_JumpAdd)(Host_address+1U);  // 1U --> for LSB bit(T bit) must be 1 for avoid ARM instruction
			/*****Log Msg*******/
#if DEBUG_INFO_ENABLE == BL_DEBUG_INFO
			Bl_Print_Msg("Jump To : 0x%X \r\n",jump_add);
#endif
			jump_add();
		}else{
#if DEBUG_INFO_ENABLE == BL_DEBUG_INFO
		Bl_Print_Msg("Address is invalid \r\n");
#endif
			/*******Report Address verification failed******/
			BL_VidSendReplyTo_Host((uint8 *)&address_verification,1U);
		}
	}
	else {
#if DEBUG_INFO_ENABLE == BL_DEBUG_INFO
		Bl_Print_Msg("CRC Verification Failed \r\n");
#endif
		BL_VidSendNack();
	}
}

/*****BL_VidWriteMemory 
**@param[in] Host_buffer pointer to data
**/
static void BL_VidWriteMemory(uint8 *Host_buffer)
{
	uint16 Host_cmd_packet_len = 0U;
	uint32 Host_Crc32 = 0U;
	uint32 Host_address =0U;
	uint32 Payload_len =0U;
	uint8	 address_verification = ADDRESS_IS_INVALID;
	uint8  flash_status = FLASH_WRITE_STATUS_FAIL;
	
	/*******Extract Crc and cmd packet from host*****/
	Host_cmd_packet_len = Host_buffer[0U] +1U;
	Host_Crc32 =*((uint32*)((Host_buffer + Host_cmd_packet_len) - CRC_SIZE_BYTE));
	
	/*****Log message***/	
#if DEBUG_INFO_ENABLE == BL_DEBUG_INFO
	Bl_Print_Msg("Jump To Specified Address Command Received \r\n");
#endif
	/****CRC verify check*****/
		if(CRC_VERFIY_SUCCESS == BL_uint8CRC_Verify((uint8 *)&Host_buffer[0],Host_cmd_packet_len - 4U ,Host_Crc32)){
#if DEBUG_INFO_ENABLE == BL_DEBUG_INFO
		Bl_Print_Msg("CRC Verification Successed \r\n");
#endif
			BL_VidSendAck(1U);
		/*******Extract address  and payload from packet*******/
			Host_address = *((uint32 *)&Host_buffer[2U]);
			Payload_len = Host_buffer[6U];
#if DEBUG_INFO_ENABLE == BL_DEBUG_INFO
			Bl_Print_Msg("Host Address is : 0x%X \r\n",Host_address);
#endif
			/*****Check address Verification***/
			address_verification = Host_uint8AddressVerification(Host_address);
			if(ADDRESS_IS_VALID == address_verification){
#if DEBUG_INFO_ENABLE == BL_DEBUG_INFO
		Bl_Print_Msg("Address Validation Successed \r\n");
#endif
				/******Write payload to flash******/
				flash_status = Flash_Mem_Write_Payload((uint8*)&Host_buffer[7U],Host_address,Payload_len);
				if(FLASH_WRITE_STATUS_PASS == flash_status){
					/*********Reply payload to host******/
					BL_VidSendReplyTo_Host((uint8*)&flash_status,1U);
#if DEBUG_INFO_ENABLE == BL_DEBUG_INFO
		Bl_Print_Msg("Payload valid \r\n");
#endif
				}else{
					/*********Reply payload to host******/
					BL_VidSendReplyTo_Host((uint8*)&flash_status,1U);
#if DEBUG_INFO_ENABLE == BL_DEBUG_INFO
		Bl_Print_Msg("Payload Invalid \r\n");
#endif
				}
			}
			else{
#if DEBUG_INFO_ENABLE == BL_DEBUG_INFO
		Bl_Print_Msg("Address Validation Failed \r\n");
#endif
				BL_VidSendReplyTo_Host((uint8*)&address_verification,1U);
			}
	}
	else {
#if DEBUG_INFO_ENABLE == BL_DEBUG_INFO
		Bl_Print_Msg("CRC Verification Failed \r\n");
#endif
		BL_VidSendNack();
	}
}

/*****BL_VidErase 
**@param[in] Host_buffer pointer to data
**/
static void BL_VidErase(uint8 *Host_buffer)
{
	uint16 Host_cmd_packet_len = 0U;
	uint32 Host_Crc32 = 0U;
	uint8 flash_erase_status = FLASH_FAILED_ERASE;
	
	/*******Extract Crc and cmd packet from host*****/
	Host_cmd_packet_len = Host_buffer[0U] +1U;
	Host_Crc32 =*((uint32*)((Host_buffer + Host_cmd_packet_len) - CRC_SIZE_BYTE));
	
	/*****Log message***/	
#if DEBUG_INFO_ENABLE == BL_DEBUG_INFO
	Bl_Print_Msg("Sector or Mass Erase of Flash \r\n");
#endif
	/*********Crc verification ***********/ 
	if(CRC_VERFIY_SUCCESS == BL_uint8CRC_Verify((uint8 *)&Host_buffer[0U],Host_cmd_packet_len -4U , Host_Crc32))
{
		/*****Log message***/	
#if DEBUG_INFO_ENABLE == BL_DEBUG_INFO
	Bl_Print_Msg("Crc Verification Passed \r\n");
#endif
	BL_VidSendAck(1U);
	
	/********** perform Erase ***********/
	flash_erase_status = Perform_uint8FlashErase(Host_buffer[2U] , Host_buffer[3U]);
	if(FLASH_SUCCESS_ERASE == flash_erase_status)
	{
				/*****Log message***/	
#if DEBUG_INFO_ENABLE == BL_DEBUG_INFO
	Bl_Print_Msg("Erase success \r\n");
#endif
		/********* Report to host erase pass**********/
		BL_VidSendReplyTo_Host((uint8*)&flash_erase_status , 1U);
	}	else{
						/*****Log message***/	
#if DEBUG_INFO_ENABLE == BL_DEBUG_INFO
	Bl_Print_Msg("Erase Failed \r\n");
#endif
				/********* Report to host failed **********/
		BL_VidSendReplyTo_Host((uint8*)&flash_erase_status , 1U);
	}
	
}else{
		/*****Log message***/	
#if DEBUG_INFO_ENABLE == BL_DEBUG_INFO
	Bl_Print_Msg("Crc Verification Failed \r\n");
#endif
	BL_VidSendNack();

}
}
/*****BL_VidExtendedErase 
**@param[in] Host_buffer pointer to data
**/
static void BL_VidExtendedErase(uint8 *Host_buffer)
	{
}
/*****BL_VidSpecial 
**@param[in] Host_buffer pointer to data
**/
static void BL_VidSpecial(uint8 *Host_buffer)
	{
}
/*****BL_VidExtendedSpecial 
**@param[in] Host_buffer pointer to data
**/
static void BL_VidExtendedSpecial(uint8 *Host_buffer)
{
}
/*****BL_VidWriteProtect 
**@param[in] Host_buffer pointer to data
**/
static void BL_VidWriteProtect(uint8 *Host_buffer)
{
		uint16 Host_cmd_packet_len = 0U;
	uint32 Host_Crc32 = 0U;
	uint8 WRP_level =0xEE;
	
	/***********Extract crc and command packet ***/
	Host_cmd_packet_len = Host_buffer[0U] + 1U;
	Host_Crc32 = *((uint32*)((Host_buffer + Host_cmd_packet_len) - CRC_SIZE_BYTE));
	
		/*****Log message***/	
#if DEBUG_INFO_ENABLE == BL_DEBUG_INFO
	Bl_Print_Msg("Write Protection Level \r\n");
#endif
	/*********Crc verification ***********/ 
	if(CRC_VERFIY_SUCCESS == BL_uint8CRC_Verify((uint8 *)&Host_buffer[0U],Host_cmd_packet_len -4U , Host_Crc32)){
			/*****Log message***/	
#if DEBUG_INFO_ENABLE == BL_DEBUG_INFO
	Bl_Print_Msg("CRC Verification Passed \r\n");
#endif
		BL_VidSendAck(1U);
		/*********Write protection activation Level**********/
		WRP_level = STM32F756_Change_WRP_Activation(Host_buffer[2U],Host_buffer[3U],WRP_STATE_EN);
		/******** Report protection level to host*******/
		BL_VidSendReplyTo_Host((uint8*)&WRP_level,1U);
	}else{
					/*****Log message***/	
#if DEBUG_INFO_ENABLE == BL_DEBUG_INFO
	Bl_Print_Msg("CRC Verification Failed \r\n");
#endif
		BL_VidSendNack();
	}
}
/*****BL_VidWriteUnProtect 
**@param[in] Host_buffer pointer to data
**/
static void BL_VidWriteUnProtect(uint8 *Host_buffer)
{
	uint16 Host_cmd_packet_len = 0U;
	uint32 Host_Crc32 = 0U;
	uint8 WRP_LEVEL = 0U;
	
	/**************** Extract CRC and Command Packet******/
	Host_cmd_packet_len = Host_buffer[0U]+1U;
	Host_Crc32 = *((uint32*)((Host_buffer + Host_cmd_packet_len) - CRC_SIZE_BYTE));
			/*****Log message***/	
#if DEBUG_INFO_ENABLE == BL_DEBUG_INFO
	Bl_Print_Msg("Disable Write Protection Level \r\n");
#endif
	/*********Crc verification ***********/ 
	if(CRC_VERFIY_SUCCESS == BL_uint8CRC_Verify((uint8 *)&Host_buffer[0U],Host_cmd_packet_len -4U , Host_Crc32)){
			/*****Log message***/	
#if DEBUG_INFO_ENABLE == BL_DEBUG_INFO
	Bl_Print_Msg("CRC Verification Passed \r\n");
#endif
		BL_VidSendAck(1U);
		/*******Diable Write protection******/
		WRP_LEVEL = STM32F756_Change_WRP_Activation(Host_buffer[2U],Host_buffer[3U],WRP_STATE_DIS);
		/******** Report Write Protection Level***/
		BL_VidSendReplyTo_Host((uint8*)&WRP_LEVEL ,1U);
}else{
					/*****Log message***/	
#if DEBUG_INFO_ENABLE == BL_DEBUG_INFO
	Bl_Print_Msg("CRC Verification Failed \r\n");
#endif
		BL_VidSendNack();
	}
}
/*****BL_VidReadOutProtect 
**@param[in] Host_buffer pointer to data
**/
static void BL_VidReadOutProtect(uint8 *Host_buffer)
{
	uint16 Host_cmd_packet_len = 0U;
	uint32 Host_Crc32 = 0U;
	uint8 RDP_level =0xEE;
	
	/***********Extract crc and command packet ***/
	Host_cmd_packet_len = Host_buffer[0U] + 1U;
	Host_Crc32 = *((uint32*)((Host_buffer + Host_cmd_packet_len) - CRC_SIZE_BYTE));
	
		/*****Log message***/	
#if DEBUG_INFO_ENABLE == BL_DEBUG_INFO
	Bl_Print_Msg("Enable Read Protection Level \r\n");
#endif
	/*********Crc verification ***********/ 
	if(CRC_VERFIY_SUCCESS == BL_uint8CRC_Verify((uint8 *)&Host_buffer[0U],Host_cmd_packet_len -4U , Host_Crc32)){
			/*****Log message***/	
#if DEBUG_INFO_ENABLE == BL_DEBUG_INFO
	Bl_Print_Msg("CRC Verification Passed \r\n");
#endif
		BL_VidSendAck(1U);
		/*********Read protection Level**********/
		RDP_level = STM32F756_Get_RDP_LEVEL();
		/******** Report protection level to host*******/
		BL_VidSendReplyTo_Host((uint8*)&RDP_level,1U);
	}else{
					/*****Log message***/	
#if DEBUG_INFO_ENABLE == BL_DEBUG_INFO
	Bl_Print_Msg("CRC Verification Failed \r\n");
#endif
		BL_VidSendNack();
	}
}
/*****BL_VidReadOutUnProtect 
**@param[in] Host_buffer pointer to data
**/
static void BL_VidReadOutUnProtect(uint8 *Host_buffer)
{
	uint16 Host_cmd_packet_len = 0U;
	uint32 Host_Crc32 = 0U;
	uint8 ROP_level_status = ROP_LEVEL_CHANGE_INVALID;
	uint8 host_ROP_level = 0U;
	
	/***********Extract crc and command packet ***/
	Host_cmd_packet_len = Host_buffer[0U] + 1U;
	Host_Crc32 = *((uint32*)((Host_buffer + Host_cmd_packet_len) - CRC_SIZE_BYTE));
	
		/*****Log message***/	
#if DEBUG_INFO_ENABLE == BL_DEBUG_INFO
	Bl_Print_Msg("Disable Read Protection Level \r\n");
#endif
	/*********Crc verification ***********/ 
	if(CRC_VERFIY_SUCCESS == BL_uint8CRC_Verify((uint8 *)&Host_buffer[0U],Host_cmd_packet_len -4U , Host_Crc32)){
			/*****Log message***/	
#if DEBUG_INFO_ENABLE == BL_DEBUG_INFO
	Bl_Print_Msg("CRC Verification Passed \r\n");
#endif
		BL_VidSendAck(1U);
		/*********** assign request change*******/
		host_ROP_level = Host_buffer[2U];
		/*********Check Reading protection **********/
		if((ROP_LV_2 == host_ROP_level) || (OB_RDP_LEVEL_2 == host_ROP_level)){
			ROP_level_status = ROP_LEVEL_CHANGE_INVALID; // warnning to go in level 2 , as it is no more possible go back to any level
		}else{  
			if((ROP_LV_0 == host_ROP_level)){
				host_ROP_level = OB_RDP_LEVEL_0 ; // 0xAAU
			}else if((ROP_LV_1 == host_ROP_level)){
				host_ROP_level = OB_RDP_LEVEL_1 ; // 0x55U
			}
			ROP_level_status = STM32F756_Change_ROP_LV(host_ROP_level);
		}
		/******** Report change protection level to host*******/
		BL_VidSendReplyTo_Host((uint8*)&ROP_level_status,1U);
	}else{
					/*****Log message***/	
#if DEBUG_INFO_ENABLE == BL_DEBUG_INFO
	Bl_Print_Msg("CRC Verification Failed \r\n");
#endif
		BL_VidSendNack();
	}
}
/*****BL_VidCheckSum 
**@param[in] Host_buffer pointer to data
**/
static void BL_VidCheckSum(uint8 *Host_buffer)
{
}
