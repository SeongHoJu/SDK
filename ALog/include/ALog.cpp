#include <windows.h>
#include <mmsystem.h>
#pragma comment(lib, "winmm.lib")

#include "ALog.h"

ALogSystem* ALogSystem::ALogInstance = NULL;


ALogMessage::ALogMessage() 
	: type(EALog_Notice), 
	  msg("") 
{
}

ALogMessage::ALogMessage( const EALogType LogType, const std::string& LogMessage )
	: type( LogType ), 
	  msg( LogMessage )
{
}

ALogMessage::ALogMessage( const ALogMessage& MessageObj )
	: type ( MessageObj.type ), 
	  msg( MessageObj.msg )
{
}

ALogMessage::~ALogMessage()
{
}

ALogMessage CreateALogMessage(EALogType type, const char* FileName, const unsigned int Line, const char* FunctionName, const char* Description)
{
	if(type >= EALog_Max)
		return ALogMessage( EALog_Warning, std::string( "Invalid LogType " ) );

	std::string RealFileName( FileName );

	int LastIndex = RealFileName.find_last_of('\\');
	if(LastIndex <= 0)
	{
		LastIndex = RealFileName.find_last_of('/');
	}

	char strRealFileName[256] = "";
	strcpy_s(strRealFileName, 256, &RealFileName[LastIndex+1]);

	const int maxloglength = 512;
	char buffer[maxloglength] = "";

	/**
		[ LogType ]
		[ cpp Name ]
		[ cpp Line ]
		[ func Name ]
		[ Log Time ]
		 - 
		[ Description ]
	 */
	sprintf_s(buffer, maxloglength, "[%s][time :%f][%s][Line : %d][Function : %s] - %s\n", AConst_LogString[type].c_str(), 
																	 ( ALogSystem::ALogInstance ) ? ( ALogSystem::ALogInstance->GetLogTime() ) : ( 0.f ),
																	 strRealFileName, 
																	 Line,
																	 FunctionName,
																	 Description);

	return ALogMessage( type, buffer );
}

ALogMessage CreateALogMessage(EALogType type, const char* Description)
{
	if(type >= EALog_Max)
		return ALogMessage( EALog_Warning, std::string( "Invalid LogType " ) );

	const int maxloglength = 512;
	char buffer[maxloglength] = "";

	sprintf_s(buffer, maxloglength, "[%s][%f] - [%s]", AConst_LogString[type].c_str(), 
													   ( ALogSystem::ALogInstance ) ? ( ALogSystem::ALogInstance->GetLogTime() ) : ( 0.f ),
													   Description);

	return ALogMessage( type, buffer );

}

ALogSystem::ALogSystem( HANDLE ConsoleCommandHandle )
	: ConsoleHandle( ConsoleCommandHandle )
{
	callback_EngineTime = NULL;
}

ALogSystem::~ALogSystem()
{


}


bool	ALogSystem::InitALogSystem( callback_GetEngineTime EngiTimeFunction )
{	
	callback_EngineTime = EngiTimeFunction;
 
	// WorkPath 얻어옴 ( 현재 Process 실행중인 Directory )
	char buffer[_MAX_PATH] = "";
	::GetCurrentDirectoryA( sizeof(char)*_MAX_PATH, buffer );

	// ----------------- WorkPath 저장
	WorkPath = buffer;

	strcat_s(buffer, _MAX_PATH, "\\");
	strcat_s(buffer, _MAX_PATH, "Logs");

	// ----------------- LogPath 저장
	LogPath =  buffer;

	char buffer2[_MAX_FNAME] = "";

	//  LogPath\\LogData\\LogName
	strcpy_s(buffer2, _MAX_FNAME, GetDefaultLogName().c_str() );
	// 확장자 적용
	strcat_s(buffer2, _MAX_FNAME, ".txt");

	// ----------------- LogName 저장
	LogName = buffer2;

	// Log Directory 생성
	CreateDirectoryA( LogPath.c_str() , NULL );

	// 최종 LogFile까지의 경로 저장

	strcat_s( buffer, _MAX_PATH, "\\");
	strcat_s( buffer, _MAX_PATH, LogName.c_str() );

	// ----------------- LogFullPath 저장
	LogFullPath = buffer;

	return true;
}

float	ALogSystem::GetLogTime()
{
	return ( callback_EngineTime != NULL ) ? ( callback_EngineTime() ) : 0.f;
}

std::string	ALogSystem::GetDefaultLogName()
{
	SYSTEMTIME systime;
	GetLocalTime(&systime);

	char* pDate = "Day";
	if(systime.wHour - 12 >= 0)
	{		
		pDate = "Night";
	}

	char buffer[_MAX_PATH] = "";
	sprintf_s(buffer, _MAX_PATH, "%04d_%02d_%02d_[%s]_%02d_%02d_%02d_LogData", 
		systime.wYear, systime.wMonth, systime.wDay,
		pDate,
		systime.wHour % 12, 
		systime.wMinute, 
		systime.wSecond);

	return std::string( buffer );
}

void	ALogSystem::RecordDetailLog( EALogType type, const char* FileName, const unsigned int Line, const char* FunctionName, char* format, ...)
{
	va_list va_ptr;
	char mm[ALogMaxLength] = "";

	// 가변 매개변수를 통해 printf처럼 mm에 sprintf함.
	va_start(va_ptr, format);
	::vsprintf_s(mm, format, va_ptr);
	va_end(va_ptr);

	ALogMessage MsgData( CreateALogMessage( type, FileName, Line, FunctionName, mm ) );

	switch( type )
	{
	case EALog_Success	: SetConsoleTextAttribute( ConsoleHandle, ECmd_GREEN ); break;
	case EALog_Warning  : SetConsoleTextAttribute( ConsoleHandle, ECmd_YELLOW); break;
	case EALog_Error    :
	case EALog_CriticalError :
		SetConsoleTextAttribute( ConsoleHandle, ECmd_RED ); 
		break;
	}

	printf_s("%s", MsgData.msg.c_str() );
	SetConsoleTextAttribute( ConsoleHandle, ECmd_GRAY );

	LogList.push_back( MsgData );

	if( LogList.size() > 50 )
		SaveLog();
}

void	ALogSystem::RecordLog( EALogType type, char* format, ...)
{ 
	va_list va_ptr;
	char mm[ALogMaxLength] = "";

	// 가변 매개변수를 통해 printf처럼 mm에 sprintf함.
	va_start(va_ptr, format);
	::vsprintf_s(mm, format, va_ptr);
	va_end(va_ptr);

	ALogMessage MsgData( CreateALogMessage( type, mm ) );

	LogList.push_back( MsgData );

	if( LogList.size() > 50 )
		SaveLog();
}

void	ALogSystem::SaveLog()
{
	FILE* pFile = NULL;
	::fopen_s(&pFile, LogFullPath.c_str(), "at");

	if(!pFile) 
		return;

	for(UINT i=0; i<LogList.size(); i++)
	{
		::fprintf(pFile, "%s", LogList[i].msg.c_str() );
	}

	::fclose(pFile);

	std::vector< ALogMessage > emptyList(0);
	LogList.swap( emptyList );
	LogList.clear();
}

bool	ALogSystemBegin( callback_GetEngineTime EngineTimeFunction )
{
	if( ALogSystem::ALogInstance == NULL )
	{
		ALogSystem::ALogInstance = new ALogSystem( GetStdHandle( STD_OUTPUT_HANDLE ) );
	}

	return ALogSystem::ALogInstance->InitALogSystem( EngineTimeFunction );
}

void	ALogSystemEnd()
{
	ALogSystem::ALogInstance->SaveLog();
	
	delete ALogSystem::ALogInstance;
	ALogSystem::ALogInstance = NULL;
}