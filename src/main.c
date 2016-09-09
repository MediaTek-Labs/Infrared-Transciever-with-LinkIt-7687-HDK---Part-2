/* Copyright Statement:
 *
 * (C) 2005-2016  MediaTek Inc. All rights reserved.
 *
 * This software/firmware and related documentation ("MediaTek Software") are
 * protected under relevant copyright laws. The information contained herein
 * is confidential and proprietary to MediaTek Inc. ("MediaTek") and/or its licensors.
 * Without the prior written permission of MediaTek and/or its licensors,
 * any reproduction, modification, use or disclosure of MediaTek Software,
 * and information contained herein, in whole or in part, shall be strictly prohibited.
 * You may only use, reproduce, modify, or distribute (as applicable) MediaTek Software
 * if you have agreed to and been bound by the applicable license agreement with
 * MediaTek ("License Agreement") and been granted explicit permission to do so within
 * the License Agreement ("Permitted User").  If you are not a Permitted User,
 * please cease any access or use of MediaTek Software immediately.
 * BY OPENING THIS FILE, RECEIVER HEREBY UNEQUIVOCALLY ACKNOWLEDGES AND AGREES
 * THAT MEDIATEK SOFTWARE RECEIVED FROM MEDIATEK AND/OR ITS REPRESENTATIVES
 * ARE PROVIDED TO RECEIVER ON AN "AS-IS" BASIS ONLY. MEDIATEK EXPRESSLY DISCLAIMS ANY AND ALL
 * WARRANTIES, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE OR NONINFRINGEMENT.
 * NEITHER DOES MEDIATEK PROVIDE ANY WARRANTY WHATSOEVER WITH RESPECT TO THE
 * SOFTWARE OF ANY THIRD PARTY WHICH MAY BE USED BY, INCORPORATED IN, OR
 * SUPPLIED WITH MEDIATEK SOFTWARE, AND RECEIVER AGREES TO LOOK ONLY TO SUCH
 * THIRD PARTY FOR ANY WARRANTY CLAIM RELATING THERETO. RECEIVER EXPRESSLY ACKNOWLEDGES
 * THAT IT IS RECEIVER'S SOLE RESPONSIBILITY TO OBTAIN FROM ANY THIRD PARTY ALL PROPER LICENSES
 * CONTAINED IN MEDIATEK SOFTWARE. MEDIATEK SHALL ALSO NOT BE RESPONSIBLE FOR ANY MEDIATEK
 * SOFTWARE RELEASES MADE TO RECEIVER'S SPECIFICATION OR TO CONFORM TO A PARTICULAR
 * STANDARD OR OPEN FORUM. RECEIVER'S SOLE AND EXCLUSIVE REMEDY AND MEDIATEK'S ENTIRE AND
 * CUMULATIVE LIABILITY WITH RESPECT TO MEDIATEK SOFTWARE RELEASED HEREUNDER WILL BE,
 * AT MEDIATEK'S OPTION, TO REVISE OR REPLACE MEDIATEK SOFTWARE AT ISSUE,
 * OR REFUND ANY SOFTWARE LICENSE FEES OR SERVICE CHARGE PAID BY RECEIVER TO
 * MEDIATEK FOR SUCH MEDIATEK SOFTWARE AT ISSUE.
 */

#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include "FreeRTOS.h"
#include "task.h"
#include "os.h"
#include "sys_init.h"
#include "wifi_api.h"
#include "sta_network.h"

#include "sntp.h"
#include "syslog.h"
#include "hal_rtc.h"

#include "mcs.h"
/* hal includes */
#include "hal.h"
/*hal pinmux define*/
#include "hal_pinmux_define.h"

/* Header file for IR PWD Data that needs to be emitted/broadcaste */
#include "ir_data.h"



#define MAX_STRING_SIZE 1024
/*  Task handle for mcs_listener, ir_listener   */
static TaskHandle_t mcs_iremitter_task_handle = NULL;



/**
  * @brief      Emit IR data using HAL IR TX API.Based on the command value received from the MCS socket
  							different IR data will be emitted/broadcasted.
  * @param[in]  command : integer command value sent by the MCS console.
  							
  * @return     None
  */
void send_ir_tv_data(int command)
{
	int result = HAL_IRTX_STATUS_OK;
	 
  uint8_t base_period = 80;
	
	  switch(command)
		{
			case IR_COMMAND_ONOFF:
								printf("\n Received Command : IR_COMMAND_ONOFF");
								result = hal_irtx_send_pulse_data(base_period, PWDIRDATA_SWITCHONOFF, PWDIRDATA_SWITCHONOFF_LENGTH);
			break;
			case IR_COMMAND_MUTE:
				        printf("\n Received Command : Mute");
								result = hal_irtx_send_pulse_data(base_period, PWDIRDATA_MUTE, PWDIRDATA_MUTE_LENGTH);
			break;
			case IR_COMMAND_VOLUMEINC:
				printf("\n Received Command : Volume ++");
								result = hal_irtx_send_pulse_data(base_period, PWDIRDATA_VOLUMEINC, PWDIRDATA_VOLUMEINC_LENGTH);
			break;
			case IR_COMMAND_VOLUMEDEC:
				printf("\n Received Command : Volume --");
								result = hal_irtx_send_pulse_data(base_period, PWDIRDATA_VOLUMEDEC, PWDIRDATA_VOLUMEDEC_LENGTH);
			break;
			case IR_COMMAND_CHANNELINC:
				printf("\n Received Command : Channel ++");
								result = hal_irtx_send_pulse_data(base_period, PWDIRDATA_CHANNELINC, PWDIRDATA_CHANNELINC_LENGTH);
			break;
			case IR_COMMAND_CHANNELDEC:
				printf("\n Received Command : Channel --");
							  result = hal_irtx_send_pulse_data(base_period, PWDIRDATA_CHANNELDEC, PWDIRDATA_CHANNELDEC_LENGTH);
			break;
			default:
				printf("\n Received Command : Defaullt/Unknown : Setting to Mute");
							  result = hal_irtx_send_pulse_data(base_period, PWDIRDATA_MUTE, PWDIRDATA_MUTE_LENGTH);
				break;
		}
		
		if (result != HAL_IRTX_STATUS_OK)
		{ 
			printf("hal_irtx_send_pulse_data failed.");
 		}
 		else
 		{
 			printf("hal_irtx_send_pulse_data Success.");
 		}
	
}
/**
  * @brief      Initialise, configure IR GPIO 33 , send IR data and de-initialise IR GPIO 33.
  							
  * @param[in]  command : integer command value sent by the MCS console.
  							
  * @return     None
  */

	void send_ir_data(int command)
	{
	    /* Set frequency to 40Khz*/
	    uint16_t frequency = 40;
	    /* Set duty cycle to 25%*/
	    uint8_t duty_cycle = 25;
	
	    hal_gpio_init(HAL_GPIO_33);
	    /* Call hal_pinmux_set_function to set GPIO pinmux. */
	    hal_pinmux_set_function(HAL_GPIO_33, HAL_GPIO_33_IR_TX);    
	    hal_irtx_configure_pulse_data_carrier(frequency, duty_cycle);
	    send_ir_tv_data(command);  
	    hal_gpio_deinit(HAL_GPIO_33);   
	    log_hal_info("\n Sending IR data completed.");
	}



/**
  * @brief      Create a task for MCS IR example
  * @param[in]  void *args:Not used
  * @return     None
  */
  

void mcs_iremitter_task(void *args)
{   int ret;
    LOG_I(common, "--- mcs_listener_task begin ---\r\n");
    for (;;) {
       
        /*  Network must be ready & get connected with 7687 before MCS socket connection */
        sta_network_ready();
        
         /* Start MCS connection and keep receiving data over socket */
        ret = mcs_connect();
        if(ret < 0)
        {
        	LOG_I(common, "Error: Could not connect to MCS Server.");
        }
        else
        {
	        char dataBuffer[MAX_STRING_SIZE] = {0};
	        int ret,command;
	        while(1)
	        {
	        	LOG_I(common, "While loop --> waiting for data");
	    	  	ret = mcs_waitfor_mcsdata(dataBuffer);
	    	  	if(ret < 0)
	    	  	{
	    	  		LOG_I(common, "Error: Discard the received buffer.");
	    	  	}
	    	  	else
	    	  	{
	    	  		command = mcs_identify_command(dataBuffer);
	    	  		if(command > 0)
	    	  		{
	    	  			send_ir_data(command);
	    	  		}
	    	  		else
	    	  		{
	    	  			LOG_I(common, "Undefined MCS Command.");
	    	  		}
	    	  	}
    	  	}
    	  }
        
    }
    //vTaskDelete(NULL);
}


 
/**
  * @brief     Main entry for MCS IR client example
  * @param[in] void *args:Not used
  * @retval    None
  */

int main(void)
{
	  
    
    system_init();
     
    /* Initialize network & connect to WiFi Router */
    sta_network_init();
    
    /* Create a task to start MCS http connection and wait for the data command from MCS */
    xTaskCreate(mcs_iremitter_task, "mcs", 2048, NULL, 1 , &mcs_iremitter_task_handle);

    vTaskStartScheduler();
    /* If all is well, the scheduler will now be running, and the following line
    will never be reached.  If the following line does execute, then there was
    insufficient FreeRTOS heap memory available for the idle and/or timer tasks
    to be created.  See the memory management section on the FreeRTOS web site
    for more details. */
    for ( ;; );
}


