#ifndef _A_LOG_H_
#define _A_LOG_H_

#include <string>
#include <vector>

#include <windows.h>


enum EALogType
{
	EALog_Notice = 0,
	EALog_Success,
	EALog_Warning,
	EALog_Error,
	EALog_CriticalError,

	EALog_Max
};

static const std::string	AConst_LogString[EALog_Max] = 
{
	std::string("NOTICE  "),
	std::string("SUCCESS "),
	std::string("WARNING "),
	std::string("ERROR   "),
	std::string("CRITICAL")
};


/**
 *	@Function
 *	@author 주성호
 *
 *	매크로 함수로 만듬. 따로 호출될 일이 없다.
 *	밑의 #define ANOTICE 같은거 참조
 */

struct ALogMessage
{
	EALogType	type;
	std::string	msg;
	
	ALogMessage();
	ALogMessage( const EALogType LogType, const std::string& LogMessage );
	ALogMessage( const ALogMessage& MessageObj );

	~ALogMessage();

};

ALogMessage CreateALogMessage(EALogType type, const char* FileName, const unsigned int Line, const char* FunctionName, const char* Description);
ALogMessage CreateALogMessage(EALogType type, const char* Description);

typedef float (*callback_GetEngineTime)(void);
#define	ALogMaxLength 1024

struct ALogSystem
{
private:

	std::string		WorkPath;
	std::string		LogPath;
	std::string		LogName;
	std::string		LogFullPath;

	std::vector< ALogMessage >	LogList;

	callback_GetEngineTime callback_EngineTime;
	
	HANDLE			ConsoleHandle;

public:

	ALogSystem( HANDLE ConsoleCommandHandle );
	~ALogSystem();

	bool		InitALogSystem( callback_GetEngineTime EngineTimeFunction );
	float		GetLogTime();
	std::string	GetDefaultLogName();

	void		RecordLog( EALogType type, char* format, ...);
	void		RecordDetailLog( EALogType type, const char* FileName, const unsigned int Line, const char* FunctionName, char* format, ...);
	void		SaveLog();

public:

	static ALogSystem*	ALogInstance;
};


bool	ALogSystemBegin( callback_GetEngineTime EngineTimeFunction );
void	ALogSystemEnd();

/**
	[ LogType ]
	[ Log Time ]
	[ cpp Name ]
	[ cpp Line ]
	[ func Name ]
	 - 
	 Description  
*/

#define __ALOG( logtype, format, msg, ...)  ALogSystem::ALogInstance->RecordLog( logtype, format, msg, __VA_ARGS__)
#define __ALOG_JustMsg( logtype, msg )		ALogSystem::ALogInstance->RecordLog( logtype, msg )

#define __ALOG_DEBUG( logtype, format, msg, ...)  ALogSystem::ALogInstance->RecordDetailLog( logtype,  __FILE__,  __LINE__, __FUNCTION__, format, msg, __VA_ARGS__)
#define __ALOG_JustMsg_DEBUG( logtype, msg )	  ALogSystem::ALogInstance->RecordDetailLog( logtype,  __FILE__,  __LINE__, __FUNCTION__, msg )

enum ECmdColor{
	ECmd_BLACK,
	ECmd_D_BLUE,
	ECmd_D_GREEN,
	ECmd_D_SKYBLUE,
	ECmd_D_RED,
	ECmd_D_VIOLET,
	ECmd_D_YELLOW,
	ECmd_GRAY,
	ECmd_D_GRAY,
	ECmd_BLUE,
	ECmd_GREEN,
	ECmd_SKYBLUE,
	ECmd_RED,
	ECmd_VIOLET,
	ECmd_YELLOW,
	ECmd_WHITE,
};

#ifdef _DEBUG

	#define _ANOTICE( format, msg, ... )	__ALOG_DEBUG( EALog_Notice, format, msg );
	#define __ANOTICE( msg )				__ALOG_JustMsg_DEBUG( EALog_Notice, msg );

	#define _ASUCCESS( format, msg, ... )	__ALOG_DEBUG( EALog_Success, format, msg );
	#define __ASUCCESS( msg )				__ALOG_JustMsg_DEBUG( EALog_Success, msg );

	#define _AWARNING( format, msg, ... )	__ALOG_DEBUG( EALog_Warning, format, msg );
	#define __AWARNING( msg )				__ALOG_JustMsg_DEBUG( EALog_Warning, msg );

	#define _AERROR( format, msg, ... )		__ALOG_DEBUG( EALog_Error, format, msg );
	#define __AERROR( msg )					__ALOG_JustMsg_DEBUG( EALog_Error, msg );

	#define _ACRITICAL( format, msg, ... )	__ALOG_DEBUG( EALog_CriticalError, format, msg );
	#define __ACRITICAL( msg )				__ALOG_JustMsg_DEBUG( EALog_CriticalError, msg );

#else

	#define _ANOTICE( format, msg, ... )	__ALOG( EALog_Notice, format, msg );
	#define __ANOTICE( msg )				__ALOG_JustMsg( EALog_Notice, msg );

	#define _ASUCCESS( format, msg, ... )	__ALOG( EALog_Success, format, msg );
	#define __ASUCCESS( msg )				__ALOG_JustMsg( EALog_Success, msg );

	#define _AWARNING( format, msg, ... )	__ALOG( EALog_Warning, format, msg );
	#define __AWARNING( msg )				__ALOG_JustMsg( EALog_Warning, msg );

	#define _AERROR( format, msg, ... )		__ALOG( EALog_Error, format, msg );
	#define __AERROR( msg )					__ALOG_JustMsg( EALog_Error, msg );

	#define _ACRITICAL( format, msg, ... )	__ALOG( EALog_CriticalError, format, msg );
	#define __ACRITICAL( msg )				__ALOG_JustMsg( EALog_CriticalError, msg );
#endif

#endif