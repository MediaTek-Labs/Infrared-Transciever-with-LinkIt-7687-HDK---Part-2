#ifndef _MCS_
#define _MCS_


#define MAX_MCS_BUFFER 512
#define MAX_SPLIT 6
#define MAX_CODE_LEN 32
#define MCS_DELIMITER ","
#define MCS_HEARTBEAT_TIMESTAMP "0"

/* MCS Device Id and Devicekey 
   Modify these values to your deviceId and deviceKey
*/
#define DEFAULT_DEVICEKEY "8M5CXU5kWi6f07QJ"
#define DEFAULT_DEVICEID "DTBP0NY1"

/* MCS Data controller channel id you have given in the MCS cloud service for gamepad 
   Modify the "controller_id" string, if you have given different controller channel Id in MCS.
*/
#define IRDA_CHANNEL_CONTROLLER "controller_id"

/* MCS Data controller channel values, you have given in the MCS gamepad controller.
   Modify these values ( 1-6) if you have given different values for the game pad controllers.
 */

#define IR_COMMAND_VOLUMEINC 	 1
#define IR_COMMAND_VOLUMEDEC 	 2
#define IR_COMMAND_CHANNELINC  3
#define IR_COMMAND_CHANNELDEC  4
#define IR_COMMAND_ONOFF 			 5
#define IR_COMMAND_MUTE  			 6


/* Now only .com , must do for china : NO NEED TO CHANGE THIS  */
#define HTTPS_MTK_CLOUD_URL_COM "http://api.mediatek.com/mcs/v2/devices/"
#define HTTPS_MTK_CLOUD_URL_CN "http://api.mediatek.cn/mcs/v2/devices/"

#ifdef __cplusplus
extern "C" {
#endif

typedef void (*mcs_tcp_callback_t)(char *);

/*
	@brief	split string into tokens
	@param[Out]	char **arr: 		output tokens buffer
	@param[In]	char *str:			input string
	@param[In]	const char *del:	delimiter
	@return None

	@thread-safe:	unsafe
*/
void mcs_split(char **arr, char *str, const char *del) ;

/*
	@brief	split string into tokens with maximum number of tokens
	@param[Out]	char **arr: 		output tokens buffer
	@param[In]	char *str:			input string
	@param[In]	const char *del:	delimiter
	@param[In]	uint32_t max_split:	maximum number of tokens
	@return None

	@thread-safe:	safe
*/
void mcs_split_n(char **arr, char *str, const char *del, uint32_t max_split) ;

/*
	@brief	upload a ASCII encoded data in csv format to MCS server
	@param[In]	char *value:	ASCII encoded data in csv format
	@return HTTPCLIENT_RESULT
*/
int mcs_upload_datapoint(char *value);

/*
	@brief		  connect to MCS server using HTTP request and create socket
	@param[In]	None
	@return result code
*/
int mcs_connect();

/*
	@brief		  Wait for the data in the socket. Data channel buffer will be received in this function
	@param[In]	None
	@return result code
*/
int mcs_waitfor_mcsdata(char* pBuffer);
/*
	@brief		  Parse the received data to identify the command.
	@param[In]	None
	@return result code
*/
int mcs_identify_command(char *arg);

#ifdef __cplusplus
}
#endif

#endif