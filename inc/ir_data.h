#ifndef _IR_DATA_HEADER_
#define _IR_DATA_HEADER_


#ifdef __cplusplus
extern "C" {
#endif
  /* The IR PWD data and length are created using an IR receiver. Below declared data
     are from a TV IR remote controller. You can follow the tutorial Part 1 to create
     IR data from your home appliance IR remote controller.
  
  */
  
  // Mute
	#define PWDIRDATA_MUTE_LENGTH 67
	uint8_t PWDIRDATA_MUTE[PWDIRDATA_MUTE_LENGTH] = 
	{
		0x71,0x70,0x0e,0x29,0x0e,0x29,0x0e,0x29,0x0e,0x0e,0x0d,0x0d,0x0e,0x0d,0x0e,0x0d,
		0x0e,0x0d,0x0e,0x29,0x0e,0x29,0x0e,0x29,0x0e,0x0e,0x0d,0x0e,0x0d,0x0e,0x0d,0x0d,
		0x0e,0x0e,0x0d,0x29,0x0e,0x29,0x0e,0x29,0x0e,0x29,0x0e,0x0e,0x0d,0x0e,0x0d,0x0d,
		0x0e,0x0d,0x0e,0x0d,0x0e,0x0e,0x0d,0x0e,0x0d,0x0d,0x0e,0x29,0x0e,0x29,0x0e,0x29,
		0x0e,0x29,0x0e
	};
	// Switch Off/On 
	#define PWDIRDATA_SWITCHONOFF_LENGTH 67
	uint8_t PWDIRDATA_SWITCHONOFF[PWDIRDATA_SWITCHONOFF_LENGTH] = 
	{
		0x72,0x70,0x0f,0x29,0x0f,0x29,0x0e,0x29,0x0e,0x0d,0x0f,0x0d,0x0e,0x0d,0x0e,0x0d,
		0x0e,0x0d,0x0f,0x29,0x0f,0x29,0x0e,0x29,0x0f,0x0c,0x0e,0x0d,0x0e,0x0d,0x0f,0x0d,
		0x0e,0x0d,0x0f,0x0c,0x0f,0x29,0x0e,0x0d,0x0f,0x0c,0x0f,0x0d,0x0e,0x0d,0x0e,0x0d,
		0x0e,0x0d,0x0f,0x29,0x0e,0x0d,0x0f,0x29,0x0f,0x29,0x0e,0x2a,0x0f,0x29,0x0e,0x29,
		0x0e,0x29,0x0f
	};
	// Volume up
	#define PWDIRDATA_VOLUMEINC_LENGTH 67
	uint8_t PWDIRDATA_VOLUMEINC[PWDIRDATA_VOLUMEINC_LENGTH] = 
	{
		0x71,0x70,0x0f,0x29,0x0e,0x2a,0x0e,0x29,0x0e,0x0d,0x0e,0x0d,0x0e,0x0d,0x0e,0x0d,
		0x0e,0x0d,0x0e,0x2a,0x0f,0x29,0x0e,0x29,0x0f,0x0d,0x0f,0x0d,0x0e,0x0e,0x0d,0x0d,
		0x0f,0x0c,0x0e,0x2a,0x0f,0x29,0x0d,0x29,0x0e,0x0d,0x0e,0x0d,0x0e,0x0d,0x0e,0x0d,
		0x0e,0x0d,0x0e,0x0d,0x0e,0x0d,0x0f,0x0c,0x0f,0x29,0x0e,0x2a,0x0d,0x29,0x0e,0x29,
		0x0e,0x29,0x0e
	};
	#define PWDIRDATA_VOLUMEDEC_LENGTH 67
	uint8_t PWDIRDATA_VOLUMEDEC[PWDIRDATA_VOLUMEDEC_LENGTH] = 
	// Volumedown
	{
		0x71,0x70,0x0e,0x29,0x0e,0x29,0x0e,0x29,0x0e,0x0d,0x0e,0x0d,0x0e,0x0d,0x0e,0x0d,
		0x0e,0x0d,0x0e,0x29,0x0e,0x29,0x0e,0x29,0x0e,0x0d,0x0e,0x0d,0x0e,0x0d,0x0e,0x0d,
		0x0e,0x0d,0x0e,0x29,0x0e,0x29,0x0e,0x0d,0x0e,0x29,0x0e,0x0d,0x0e,0x0d,0x0e,0x0d,
		0x0e,0x0d,0x0e,0x0d,0x0e,0x0d,0x0e,0x29,0x0e,0x0d,0x0e,0x29,0x0e,0x29,0x0e,0x29,
		0x0e,0x29,0x0e
	};
	
	// Channel up
	#define PWDIRDATA_CHANNELINC_LENGTH 67
	uint8_t PWDIRDATA_CHANNELINC[PWDIRDATA_CHANNELINC_LENGTH] = 
	{
    0xe0,0x6f,0x0e,0x0d,0x0d,0x0d,0x0d,0x0e,0x0d,0x0d,0x0d,0x0d,0x0d,0x0d,0x0d,0x0e,
    0x0d,0x0d,0x0d,0x2a,0x0d,0x29,0x0e,0x29,0x0e,0x29,0x0e,0x29,0x0e,0x29,0x0e,0x29,
    0x0e,0x29,0x0e,0x29,0x0e,0x29,0x0e,0x0d,0x0d,0x0d,0x0d,0x0d,0x0d,0x0d,0x0d,0x29,
    0x0e,0x0d,0x0d,0x0d,0x0d,0x0d,0x0d,0x29,0x0e,0x29,0x0e,0x29,0x0e,0x29,0x0e,0x0d,
    0x0d,0x29,0x0e
  };
	
	//Channel down
	#define PWDIRDATA_CHANNELDEC_LENGTH 67
	uint8_t PWDIRDATA_CHANNELDEC[PWDIRDATA_CHANNELDEC_LENGTH] = 
	{
		0xe0,0x6f,0x0e,0x0d,0x0d,0x0e,0x0d,0x0d,0x0d,0x0d,0x0d,0x0e,0x0d,0x0d,0x0d,0x0d,
    0x0d,0x0e,0x0d,0x29,0x0e,0x2a,0x0d,0x29,0x0e,0x29,0x0e,0x29,0x0e,0x29,0x0e,0x29,
    0x0e,0x29,0x0e,0x0d,0x0d,0x0d,0x0d,0x0e,0x0d,0x0d,0x0d,0x0d,0x0d,0x0e,0x0d,0x29,
    0x0e,0x0d,0x0d,0x29,0x0e,0x29,0x0e,0x29,0x0e,0x29,0x0e,0x29,0x0e,0x29,0x0e,0x0e,
    0x0d,0x29,0x0e
	};
			
			

#ifdef __cplusplus
}
#endif

#endif