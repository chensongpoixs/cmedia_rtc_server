/***********************************************************************************************
created: 		2022-05-22

author:			chensong

purpose:		log


************************************************************************************************/
#ifndef _C_LOG_H_
#define _C_LOG_H_
 
	


#ifdef __cplusplus


	 
	extern "C" {

#endif
		void LOG(const char* format, ...);

#define WARNING_EX_LOG(format, ...)	LOG("[%s][%d][warn]" format, __FUNCTION__, __LINE__, ##__VA_ARGS__)
#define DEBUG_EX_LOG(format, ...)   LOG("[%s][%d][debug]" format, __FUNCTION__, __LINE__, ##__VA_ARGS__)
#define ERROR_EX_LOG(format, ...)   LOG("[%s][%d][error]" format, __FUNCTION__, __LINE__, ##__VA_ARGS__)

  
#ifdef __cplusplus

	}

#endif
#endif // _C_LOG_H_
 

