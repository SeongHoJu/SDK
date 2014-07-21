#ifndef _A_TIMER_H_
#define _A_TIMER_H_

#include "windows.h"
#include <hash_map>
#include <list>
#include <string>

#ifndef TO_STRING 
#define TO_STRING(s) #s
#endif

typedef VOID (*ACallBack_VOIDToVOID)(VOID);
typedef VOID (*ACallBack_VoidToInt)(INT);

#define INVALID_TIME	-1.0

struct ATimerData
{
	LARGE_INTEGER	CounterBegin;
	LARGE_INTEGER	CounterEnd;
	LARGE_INTEGER	LastRecordedCounter;

	BOOL					bUseFunctionCall;
	std::string				FunctionName;
	ACallBack_VOIDToVOID	CallFunctionPtr;
	FLOAT					CallTimeCycle;
	FLOAT					AccumulateTime;
	INT						CallCount;
	
	ATimerData();
	ATimerData(const ATimerData& rhs);
};

class ATimerSystem
{
private:

	static stdext::hash_map<const char*, ATimerData*>	TimerDataHashMap;
	static std::list<ATimerData*>	FunctionTimerList;
	
	static HANDLE	MutexHandle;
	static DWORD	OwnerThreadID;

	static LARGE_INTEGER	Frequency;

	static VOID		MutexLock();
	static VOID		MutexUnLock();

	static BOOL		GetTimerData( const CHAR* TimerName, ATimerData** ATDPtrAddress );
	static BOOL		UpdateFunctionTimer( ATimerData* TargetTimer );


public:

	ATimerSystem();
	~ATimerSystem();

	static BOOL		InitATimerSystem();
	static VOID		ReleaseATimerSystem();

	static ATimerData*	BeginTimer( const CHAR* TimerName );	// ¾µ³ð
	
	static DOUBLE	EndTimer( const CHAR* TimerName );		
	static FLOAT	EndTimerFloat( const CHAR* TimerName );	// ¾µ³ð

	static DOUBLE	AFunctionTimeChecker( const CHAR* FunctionName, ACallBack_VOIDToVOID function_ptr );
	static DOUBLE	AFunctionTimeChecker( const CHAR* FunctionName, ACallBack_VoidToInt  function_ptr, INT function_arg );

	static DOUBLE	GetTimerTick( ATimerData* TimerData );
	static DOUBLE	GetTimerTick( const CHAR* TimerName );

	static FLOAT	GetTimerTickFloat( ATimerData* TimerData );
	static FLOAT	GetTimerTickFloat( const CHAR* TimerName );

	static DOUBLE	GetMainTick();
	static FLOAT	GetMainTickFloat();

	static DOUBLE	GetMainFullTime();
	static FLOAT	GetMainFulltimeFloat();

	static DOUBLE	GetTimerFullTime( const CHAR* TimerName, BOOL bErase = FALSE );
	static FLOAT	GetTimerFullTimeFloat( const CHAR* TimerName, BOOL bErase = FALSE );

	//////////////////////////////////////////////////////////////////////////////////////
	static FLOAT	ATimerSystemTick(); 	// Engine¿¡¼­ MainTick ±¸ÇÏ´Âµ¥ ¾µ³ð Á¦ÀÏ Áß¿ä
	//////////////////////////////////////////////////////////////////////////////////////

	static BOOL		SetFunctionTimer( const CHAR* FunctionName, ACallBack_VOIDToVOID function_ptr, FLOAT TimerTick );
	static VOID		ClearFunctionTimer( const CHAR* FunctionName );
};

#define ATimerBegin( TimerName )	ATimerSystem::BeginTimer( TO_STRING( TimerName ) )
#define ATimerEnd( TimerName )		ATimerSystem::EndTimer( TO_STRING( TimerName ) )

#define APerformanceChecker( FunctionName )					ATimerSystem::AFunctionTimeChecker( TO_STRING( FunctionName ), FunctionName )
#define APerformanceCheckerInt( FunctionName, IntValue )	ATimerSystem::AFunctionTimeChecker( TO_STRING( FunctionName ), FunctionName, IntValue )

#define ASetTimer( FunctionName, FloatTimeCycle )	ATimerSystem::SetFunctionTimer( TO_STRING( FunctionName), FunctionName, FloatTimeCycle )
#define AClearTimer( FunctionName )					ATimerSystem::ClearFunctionTimer( TO_STRING( FunctionName ) )

BOOL	ATimerSystemBegin();


#endif