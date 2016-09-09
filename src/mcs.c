#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include "network_init.h"
#include "lwip/netif.h"
#include "lwip/tcpip.h"
#include "lwip/sockets.h"
#include "ethernetif.h"
#include "lwip/sockets.h"
#include "netif/etharp.h"
#include "timers.h"
#include "os.h"
#include "httpclient.h"
#include "mcs.h"

#include "MQTTClient.h"

#include "fota_download_interface.h"

/* tcp config */
#define SOCK_TCP_SRV_PORT 443

#define MAX_STRING_SIZE 1024
TimerHandle_t heartbeat_timer;

/* RESTful config */
#define BUF_SIZE   (1024 * 1)


#define MAX_DEVICE_KEY_LEN 20
#define MAX_DEVICE_ID_LEN 20
#define MAX_TCPIP_LEN 20
#define MAX_HOST_LEN 5


#define MCS_DELIMITER ","
#define MCS_HEARTBEAT_TIMESTAMP "0"
#define TIMESTAMP_INDEX 2
#define CHANNEL_INDEX   3
#define VALUE_INDEX     4


#define DEFAULT_HOST "com"

static char MCS_TCPIP_ADDRESS [MAX_TCPIP_LEN] = {0};


static uint8_t device_id[MAX_DEVICE_ID_LEN] = DEFAULT_DEVICEID;
static uint8_t device_key[MAX_DEVICE_KEY_LEN] = DEFAULT_DEVICEKEY;
static uint8_t host[MAX_HOST_LEN] = DEFAULT_HOST;

int s;
httpclient_t client = {0}; 

/**
  * @brief      replaces characters in a string by another
  * @param[in]  replaces parts of the string with another string
  * @return     Changed string
  */
static char *mcs_replace(char *st, char *orig, char *repl)
{
    static char buffer[1024];
    char *ch;
    if (!(ch = strstr(st, orig))) {
        return st;
    }
    strncpy(buffer, st, ch - st);
    buffer[ch - st] = 0;
    sprintf(buffer + (ch - st), "%s%s", repl, ch + strlen(orig));
    return buffer;
}
/**
  * @brief      Tokenise a string based on delimter input character.
  * @param[in]  string to split, array to be filled and delimiter 
  * @return     Array with tokenised strings.
  */
void mcs_split(char **arr, char *str, const char *del)
{
    char *s = strtok(str, del);
    while (s != NULL) {
        *arr++ = s;
        s = strtok(NULL, del);
    }
}
/**
  * @brief      Tokenise a string based on delimter input character with count
  * @param[in]  string to split, array to be filled and delimiter 
  * @return     Array with tokenised strings.
  */
void mcs_split_n(char **arr, char *str, const char *del, uint32_t max_split)
{
    uint32_t split_cnt = 0;
    char *saveptr = NULL;
    char *s = strtok_r(str, del, &saveptr);
    while (s != NULL && split_cnt < max_split) {
        *arr++ = s;
        s = strtok_r(NULL, del, &saveptr);
        split_cnt++;
    }
}

/**
  * @brief      Uploads data to MCS cloud service using HTTP POST in .CSV format.
  * @param[in]  Value to be sent to MCS.
  * @return     Result of the upload process.
  */
int mcs_upload_datapoint(char *value)
{
    /* upload mcs datapoint */
    httpclient_t client = {0};
    char *buf = NULL;

    int ret = HTTPCLIENT_ERROR_CONN;
    httpclient_data_t client_data = {0};
    char *content_type = "text/csv";


    /* Set post_url */
    char post_url[70] = {0};

    if (strcmp(host, "com") == 0) {
        strcat(post_url, HTTPS_MTK_CLOUD_URL_COM);
    } else {
        strcat(post_url, HTTPS_MTK_CLOUD_URL_CN);
    }

    strcat(post_url, device_id);
    strcat(post_url, "/datapoints.csv");

    /* Set header */
    char header[40] = {0};
    strcat(header, "deviceKey:");
    strcat(header, device_key);
    strcat(header, "\r\n");

    printf("header: %s\n", header);
    printf("url: %s\n", post_url);
    printf("data: %s\n", value);

    buf = pvPortMalloc(BUF_SIZE);
    if (buf == NULL) {
        printf("buf malloc failed.\r\n");
        return ret;
    }
    buf[0] = '\0';
    ret = httpclient_connect(&client, post_url, HTTP_PORT);

    client_data.response_buf = buf;
    client_data.response_buf_len = BUF_SIZE;
    client_data.post_content_type = content_type;
    client_data.post_buf = value;
    client_data.post_buf_len = strlen(value);
    
    httpclient_set_custom_header(&client, header);
    ret = httpclient_send_request(&client, post_url, HTTPCLIENT_POST, &client_data);
    if (ret < 0) {
        return ret;
    }
    ret = httpclient_recv_response(&client, &client_data);
    if (ret < 0) {
        return ret;
    }
    printf("\n************************\n");
    printf("httpclient_test_keepalive post data every 5 sec, http status:%d, response data: %s\r\n", httpclient_get_response_code(&client), client_data.response_buf);
    printf("\n************************\n");
    vPortFree(buf);
    httpclient_close(&client, HTTP_PORT);
    return ret;
}

/**
  * @brief      Creates MCS HTTP URL service from deviceId
  * @param[in]  None
  * @return     MCS URL 
  */
void construct_mcs_httpurl(char* pURL)
{
	printf("======= host : %s ======== \n", host);

	if (strcmp(host, "com") == 0) 
	{
		strcat(pURL, HTTPS_MTK_CLOUD_URL_COM);
	}
	else 
	{
		strcat(pURL, HTTPS_MTK_CLOUD_URL_CN);
	}
	
	strcat(pURL, device_id);
	strcat(pURL, "/connections.csv");
	
}

/**
  * @brief      Creates HTTP connection with MCS server
  * @param[in]  MCS HTTP URL for the device you created in MCS console.
  * @return     connection status
  */
int make_mcs_http_connection(char* pMCSConnectionURL)
{
	int ret = HTTPCLIENT_ERROR_CONN;
    
  ret = httpclient_connect(&client, pMCSConnectionURL, HTTP_PORT);
    
	return ret;
}
/**
  * @brief      Sends HTTP request to MCS
  * @param[in]  URL
  * @return     status and response buffer.
  */
int send_httprequestto_mcs(char* pMCScnnectionURL,char* pBuffer)
{
  int ret = HTTPCLIENT_ERROR_CONN ;
  httpclient_data_t client_data = {0};
  
	/* Set header */
	char header[40] = {0};
	strcat(header, "deviceKey:");
	strcat(header, device_key);
	strcat(header, "\r\n");
  
  client_data.response_buf     = pBuffer;
  client_data.response_buf_len = BUF_SIZE;
  httpclient_set_custom_header(&client, header);

    ret = httpclient_send_request(&client, pMCScnnectionURL, HTTPCLIENT_GET, &client_data);
    if (ret < 0) {
        return ret;
    }

    ret = httpclient_recv_response(&client, &client_data);
    if (ret < 0) {
        return ret;
    }

    printf("Response content:%s\n", client_data.response_buf);
    if (200 == httpclient_get_response_code(&client))
    {
    	ret = HTTPCLIENT_OK;
    }
    else
    {
    	ret = HTTPCLIENT_ERROR_CONN;
    }
  
  return ret;	
	
}

/**
  * @brief      Parse http response to find out IP address of MCS service.
  * @param[in]  Buffer to be parsed
  * @return     IP address will be filled.
  */
void fill_ipaddr_from_response(char* pResponseBuffer)
{
	char split_buf[MAX_STRING_SIZE] = {0};
	char *arr[2];
  char *del = ",";
  
  printf("fill_ipaddr_from_response: Response:%s\n", pResponseBuffer);
  strcpy(split_buf, pResponseBuffer);
  printf("fill_ipaddr_from_response: split_buf:%s\n", split_buf);

  mcs_split(arr, split_buf, del);
  
  printf("arr[0]:: %s\n", arr[0]);
  printf("arr[1]:: %s\n", arr[1]);

  strcpy(MCS_TCPIP_ADDRESS,arr[0]);
  printf("2-->MCS_TCPIP_ADDRESS:: %s\n", MCS_TCPIP_ADDRESS);
  
}
/**
  * @brief      To get IP address of the MCS service created for this usecase.
  * @param[in]  None
  * @return     Status of the connection.
  */
static int getmcsIP()
{
    HTTPCLIENT_RESULT ret = HTTPCLIENT_ERROR_CONN;
    char mcscnnectionURL[70] = {0};
    
    construct_mcs_httpurl(&mcscnnectionURL);
    printf("======= URL : %s ======== \n", mcscnnectionURL);   

    ret = make_mcs_http_connection(mcscnnectionURL);
    if(ret != HTTPCLIENT_OK)
    {
    	if(ret == HTTPCLIENT_UNRESOLVED_DNS)
    		printf("\No Internet : Check your internet connection.");
    		printf(" httpclient_connect FAILED: Error code:%d ",ret);
    	return ret;
    }
    
    char *pDataBuffer = NULL;
    pDataBuffer = pvPortMalloc(BUF_SIZE);
    if (pDataBuffer == NULL) {
        return ret;
    }
    pDataBuffer[0] = '\0';
    
    ret = send_httprequestto_mcs(mcscnnectionURL,pDataBuffer);
    
    if(ret == HTTPCLIENT_OK)
    {
 		    fill_ipaddr_from_response(pDataBuffer);
    }
    else
    {
    	 printf("Http Request/Response FAILED.");
    }
    
    vPortFree(pDataBuffer);
    httpclient_close(&client, HTTP_PORT);
    return ret;
}

/**
  * @brief      Send Heartbeat message to MCS cloud to keep the connection alive.
  * @param[in]  None
  * @return     socket write result
  */   

int send_heartbeatmsg()
{
	  int ret;
	  char cmd_buf [50] = {0};
	  /* Format of the heartbeat message is : "deviceid,devicekey" */
	  strcat(cmd_buf,  device_id);
	  strcat(cmd_buf, ",");
	  strcat(cmd_buf,  device_key);
	  strcat(cmd_buf, ",0");
	
	  LOG_I(common, "cmd_buf: %s\n", cmd_buf);
	
	  ret = lwip_write(s, cmd_buf, sizeof(cmd_buf));
	
	  return ret;
}

/**
  * @brief      Timer callback function to send heartbeat message to MCS using socket.
  * @param[in]  pxTimer details
  * @return     None
  */
void tcpTimerCallback(TimerHandle_t pxTimer) 
{
	/* Keep sending heartbeat message to keep the connection alive */
  send_heartbeatmsg();
}
/**
  * @brief      Creates socket and connect to MCS service.
  * @param[in]  None
  * @return     status of the socket connection
  */
int mcs_socket_connect()
{   
	  int ret;
	  struct sockaddr_in addr;
    os_memset(&addr, 0, sizeof(addr));
    addr.sin_len = sizeof(addr);
    addr.sin_family = AF_INET;
    addr.sin_port = htons(SOCK_TCP_SRV_PORT);
    addr.sin_addr.s_addr = inet_addr(MCS_TCPIP_ADDRESS);

    LOG_I(common, "============MCS TCP IP ADDRESS: %s connection ============\n", MCS_TCPIP_ADDRESS);

    /* create the socket */
    s = lwip_socket(AF_INET, SOCK_STREAM, 0);
    if (s < 0) {
        LOG_I(common, "tcp client create fail 0\n");
        return -1;
    }

    ret = lwip_connect(s, (struct sockaddr *) &addr, sizeof(addr));

    if (ret < 0) {
        lwip_close(s);
        LOG_I(common, "tcp client connect fail 1\n");
        return -1;
    }
    return 0;
}
/**
  * @brief      Wait in the socket to get data
  * @param[in]  pBuffer : Buffer to receive data
  * @return     socket connection result.
  */
  
int mcs_waitfor_mcsdata(char* pBuffer)
{
	int rcv_len, rlen;	
  char rcv_buf[MAX_STRING_SIZE] = {0};

  rcv_len = 0;
  LOG_I(common, "MCS tcp-client waiting for data...");
  rlen = lwip_recv(s, &rcv_buf[rcv_len], sizeof(rcv_buf) - 1 - rcv_len, 0);
  rcv_len += rlen;

  if (0 == rcv_len) 
  {
      LOG_I(common, "MCS tcp-client disconnected !");
      return -1;
  }
  
 	strcpy(pBuffer,rcv_buf);
	return 0;
	
}

/**
  * @brief      Check whether the MCS data is to do firmware upgrade.
  							If the message is for FOTA, trigger the FW upgrade using the firmare link provided by MCS.
  * @param[in]  void *pBufferReceived: Message from the MCS command server.
  * @return     1 if the message is for FOTA. 0 otherwise.
  */
int checkforFOTA(char* pBufferReceived)
{
	/* split the string of rcv_buffer */
	char split_buf[MAX_STRING_SIZE] = {0};
	strcpy(split_buf, pBufferReceived);
	
	char *arr[7];
	char *del = ",";
	mcs_split(arr, split_buf, del);
	/* Check whether the command message is to do FOTA */
	if (0 == strncmp(arr[3], "FOTA", 4)) 
	{
		LOG_I(common, "Command is for FOTA : fota url: %s\n", s);
		
		/* Make the FOTA URL from https to http */
		char *s = mcs_replace(arr[6], "https", "http");
		char data_buf [100] = {0};
		
		strcat(data_buf, "status");
		strcat(data_buf, ",,fotaing");
		/* Inform MCS about FOTA status */
		mcs_upload_datapoint(data_buf);
		
		LOG_I(common, "Starting FOTA....\n");
		/* Trigger FOTA with the http URL */
		fota_download_by_http(s);
		
		return 1;
	} 
	else 
	{
		return 0;
	}
}
/**
  * @brief      Parse the data received in the socket to identify the channel name and value.
  * @param[in]  arg : string to be parsed.
  * @return     command identified.
  */
int mcs_identify_command(char *arg)
{
    
    /* Split received string */
    char *mcs_split_buffer[MAX_SPLIT] = {0};
    
    /*Check whether the command is to do FOTA. If yes do the FOTA, otherwise identify the IR command */
    if( checkforFOTA(arg))
    {
    	return 0;
    }
    
    mcs_split_n(mcs_split_buffer, arg, MCS_DELIMITER, MAX_SPLIT);

    if (mcs_split_buffer[TIMESTAMP_INDEX] == NULL) {
        LOG_I(common, "mcs_tcp_init_callback(): timestamp must not be null, format error.\r\n");
        return -1;
    }

    if (strcmp(mcs_split_buffer[TIMESTAMP_INDEX], MCS_HEARTBEAT_TIMESTAMP) != 0) {
        if (mcs_split_buffer[VALUE_INDEX] == NULL) {
            LOG_I(common, "mcs_tcp_init_callback(): value must not be null when packet is not a heatbeat packet.\r\n");
             return -1;
        }
        if (strcmp(mcs_split_buffer[CHANNEL_INDEX], IRDA_CHANNEL_CONTROLLER) == 0) {           
            uint32_t code;
            sscanf(mcs_split_buffer[VALUE_INDEX], "%u", &code);
            LOG_I(common, "\r\n Received Command Code = %d",code);
            return code;
        }	
        else {
            LOG_I(common, "Unknown channel: %s Switches on/on++", mcs_split_buffer[CHANNEL_INDEX]);
        }
    } else {
        LOG_I(common, "MCS Heartbeat packet\r\n");
    }
    return -1;
}
/**
  * @brief      connect to MCS, creates socket and creates timer to send heartbeat message.
  * @param[in]  None
  * @return     result.
  */
int mcs_connect()
{
    int ret;
    int rcv_len, rlen;

    /* Get the MCS server IP Address. */
    ret = getmcsIP();
    if(ret != HTTPCLIENT_OK)
    {
    	printf("\n HTTP ERROR : Could not connect to MCS server . \n");
    	return -1;
    }
    /* Establish TCP socket connection */
    ret = mcs_socket_connect();
    if(ret < 0)
    {
    	printf("\n SOCKET ERROR : Could not connect to MCS socket . \n");
    	return -1;
    }
    /* Create a timer and keep sending heartbeat message to MCS server to keep the connection alive  */
    heartbeat_timer = xTimerCreate("HeartbeatMsgTimer", (30 * 1000 / portTICK_RATE_MS), pdTRUE, (void *) 0, tcpTimerCallback);
    xTimerStart(heartbeat_timer, 0);
    send_heartbeatmsg();
        
    return 0;
}
