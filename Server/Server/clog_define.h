#ifndef C_LOG_DEFINE_H
#define C_LOG_DEFINE_H

namespace chen
{
	enum ELogLevelType
	{
		ELogLevel_None = 0,
		ELogLevel_System,
		ELogLevel_Fatal,
		ELogLevel_Error,
		ELogLevel_Warn,
		ELogLevel_Info,
		ELogLevel_Debug,
		ELogLevel_Num,
	};

	enum ELogNameType
	{
		ELogName_None = 0,
		ELogName_Date,
		ELogName_DateTime,
		ELogName_AutoDate,
	};
}//namespace chen



#endif  // !#define C_LOG_DEFINE_H_
