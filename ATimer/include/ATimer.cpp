#include "ATimer.h"


/**
 *	@struct 
 *	@author 주성호
 *
 *	Performance Check 구조체 ( 시작 & 끝 시간 기록 )
 *
 */
ATimerData::ATimerData() 
	: bUseFunctionCall(FALSE),
	  FunctionName(""),
	  CallTimeCycle(0.f),
	  AccumulateTime(0.f),
	  CallCount(0)
{
	ZeroMemory( &CounterBegin, sizeof(LARGE_INTEGER) );
	ZeroMemory( &CounterEnd, sizeof(LARGE_INTEGER) );
	ZeroMemory( &LastRecordedCounter, sizeof(LARGE_INTEGER) );
	CallFunctionPtr = NULL;
}

ATimerData::ATimerData(const ATimerData& rhs)	
	: CounterBegin(rhs.CounterBegin),
	  CounterEnd(rhs.CounterEnd),
	  bUseFunctionCall(rhs.bUseFunctionCall),
	  FunctionName(rhs.FunctionName),
	  CallFunctionPtr(rhs.CallFunctionPtr),
	  CallTimeCycle(rhs.CallTimeCycle),
	  AccumulateTime(rhs.AccumulateTime),
	  CallCount(rhs.CallCount)
{
}


/**
 *	@static value : ATimerSystem
 *	@author 주성호
 *
 *	ATimerSystem static 변수 정의
 *
 */
stdext::hash_map<const CHAR*, ATimerData*> ATimerSystem::TimerDataHashMap;
std::list<ATimerData*>	ATimerSystem::FunctionTimerList;
HANDLE			ATimerSystem::MutexHandle;
DWORD			ATimerSystem::OwnerThreadID;
LARGE_INTEGER	ATimerSystem::Frequency;

const CHAR*	MainTimerName = "ATimerMainTimer";


VOID	ATimerSystem::MutexLock()
{
	DWORD ThreadID = GetCurrentThreadId();
	if(OwnerThreadID != ThreadID)
	{
		WaitForSingleObject(MutexHandle, INFINITE);
		OwnerThreadID = ThreadID;
	}
}

VOID	ATimerSystem::MutexUnLock()
{
	DWORD ThreadID = GetCurrentThreadId();
	if(OwnerThreadID == ThreadID)
	{
		ReleaseMutex(MutexHandle);
		OwnerThreadID = 0;
	}
}

ATimerSystem::ATimerSystem()
{
}

ATimerSystem::~ATimerSystem()
{
	WaitForSingleObject(MutexHandle, INFINITE);
	CloseHandle(MutexHandle);
}

BOOL ATimerSystem::GetTimerData( const CHAR* TimerName, ATimerData** ATDPtrAddress )
{
	stdext::hash_map<const CHAR*, ATimerData*>::iterator apd_iterator;
	apd_iterator = TimerDataHashMap.find(TimerName);

	if(apd_iterator == TimerDataHashMap.end())
	{
		return FALSE;
	}

	(*ATDPtrAddress) = apd_iterator->second;

	return TRUE;
}

BOOL	ATimerSystem::UpdateFunctionTimer( ATimerData* TargetTimer )
{
	if( TargetTimer == NULL || TargetTimer->bUseFunctionCall == FALSE || TargetTimer->CallFunctionPtr == NULL )
		return FALSE;

	if( TargetTimer->bUseFunctionCall && TargetTimer->CallFunctionPtr != NULL )
	{
		FLOAT Tick = GetTimerTickFloat(TargetTimer);
		TargetTimer->AccumulateTime += Tick;

		// 누적 시간이 Call 시간 이상으로 지났다면
		if( TargetTimer->AccumulateTime >= TargetTimer->CallTimeCycle ) 
		{
			TargetTimer->AccumulateTime = 0.f;
			TargetTimer->CallFunctionPtr();
		}
	}

	return TRUE;
}

BOOL	ATimerSystem::InitATimerSystem()
{
	MutexHandle = CreateMutex(NULL, FALSE, NULL);
	OwnerThreadID = 0;

	QueryPerformanceFrequency(&Frequency);
	if(Frequency.QuadPart == 0)
	{
		printf("Does not Support QueryPerformanceFrequency\n");
		return FALSE;
	}

	BeginTimer( MainTimerName );

	return TRUE;
}

VOID	ATimerSystem::ReleaseATimerSystem()
{
	FLOAT EndTime = EndTimerFloat( MainTimerName );
	printf("MainTimer RunTime : %f\n", EndTime);
}	

ATimerData*	ATimerSystem::BeginTimer(const CHAR* TimerName)
{
	// Thread Lock!
	ATimerSystem::MutexLock();

	ATimerData* ATD = NULL;
	if( GetTimerData( TimerName, &ATD ) == TRUE )
	{
		printf("Already exist Timer : %s", TimerName ); 
		ATimerSystem::MutexUnLock();	// Mutex 풀자
		return ATD;
	}

	ATD = new ATimerData();

	// 다중 Core일 수 있으므로 Core 고정 - Core간 동기화 
	DWORD_PTR OldMask = SetThreadAffinityMask(GetCurrentThread(), 0);
	QueryPerformanceCounter( &(ATD->CounterBegin) );
	ATD->LastRecordedCounter = ATD->CounterBegin; // Last값도 Begin값과 동일하게 설정해줘야 함

	SetThreadAffinityMask(GetCurrentThread(), OldMask);

	// Container에 TimerName으로 측정 시작 정보 Input!
	TimerDataHashMap.insert(std::pair<const CHAR*, ATimerData*>(TimerName, ATD));

	// Thread UnLock!
	ATimerSystem::MutexUnLock();

	return ATD;
}

DOUBLE	ATimerSystem::EndTimer(const CHAR* TimerName)
{
	DOUBLE TimerRunTime = GetTimerFullTime( TimerName, true );
	printf("ATimerSystem - TimerName[%s] - %f ms\n", TimerName, TimerRunTime);

	return TimerRunTime;
}

FLOAT	ATimerSystem::EndTimerFloat( const CHAR* TimerName )
{
	DOUBLE TimeValue = EndTimer( TimerName );
	return (FLOAT)( (( TimeValue != INVALID_TIME ) ? ( TimeValue ) : ( INVALID_TIME )) * 0.001f );
}

DOUBLE	ATimerSystem::AFunctionTimeChecker(const CHAR* FunctionName, ACallBack_VOIDToVOID function_ptr)
{
	ATimerSystem::BeginTimer(FunctionName);

	function_ptr();

	return ATimerSystem::EndTimer(FunctionName);
}

DOUBLE	 ATimerSystem::AFunctionTimeChecker(const CHAR* FunctionName, ACallBack_VoidToInt  function_ptr, INT function_arg)
{
	ATimerSystem::BeginTimer(FunctionName);

	function_ptr( function_arg );

	return ATimerSystem::EndTimer(FunctionName);
}


DOUBLE	ATimerSystem::GetTimerTick( ATimerData* TimerData )
{
	ATimerSystem::MutexLock();

	DWORD_PTR OldMask = SetThreadAffinityMask(GetCurrentThread(), 0);
	QueryPerformanceCounter( &(TimerData->CounterEnd) );
	SetThreadAffinityMask(GetCurrentThread(), OldMask);

	DOUBLE PerformanceMiliSeconds = 0.0;
	PerformanceMiliSeconds = (TimerData->CounterEnd.QuadPart - TimerData->LastRecordedCounter.QuadPart) / (DOUBLE)Frequency.QuadPart * 1000;
	TimerData->LastRecordedCounter = TimerData->CounterEnd;

	ATimerSystem::MutexUnLock();

	return PerformanceMiliSeconds;
}

DOUBLE	ATimerSystem::GetTimerTick( const CHAR* TimerName )
{
	ATimerSystem::MutexLock();

	ATimerData* ATD = NULL;
	if( GetTimerData( TimerName, &ATD ) == FALSE )
	{
		printf( "ATimerSystem - Can't Find Timer : %s\n", TimerName );
		ATimerSystem::MutexUnLock();	// Mutex는 풀고 나가자
		return INVALID_TIME;
	}

	DWORD_PTR OldMask = SetThreadAffinityMask(GetCurrentThread(), 0);
	QueryPerformanceCounter( &(ATD->CounterEnd) );
	SetThreadAffinityMask(GetCurrentThread(), OldMask);

	DOUBLE PerformanceMiliSeconds = 0.0;
	PerformanceMiliSeconds = (ATD->CounterEnd.QuadPart - ATD->LastRecordedCounter.QuadPart) / (DOUBLE)Frequency.QuadPart * 1000;
	ATD->LastRecordedCounter = ATD->CounterEnd;

	ATimerSystem::MutexUnLock();

	return PerformanceMiliSeconds;
}

FLOAT	ATimerSystem::GetTimerTickFloat( ATimerData* TimerData )
{
	return (FLOAT)(GetTimerTick( TimerData ) * 0.001f);
}

FLOAT	ATimerSystem::GetTimerTickFloat( const CHAR* TimerName )
{
	return (FLOAT)(GetTimerTick( TimerName ) * 0.001f);
}


DOUBLE	ATimerSystem::GetTimerFullTime( const CHAR* TimerName, BOOL bErase )
{
	ATimerSystem::MutexLock();

	ATimerData* ATD = NULL;
	if( GetTimerData( TimerName, &ATD ) == FALSE )
	{
		printf( "ATimerSystem - Can't Find Timer : %s\n", TimerName );
		ATimerSystem::MutexUnLock();	// Mutex는 풀고 나가자
		return INVALID_TIME;
	}

	DWORD_PTR OldMask = SetThreadAffinityMask(GetCurrentThread(), 0);
	QueryPerformanceCounter( &(ATD->CounterEnd) );
	SetThreadAffinityMask(GetCurrentThread(), OldMask);

	DOUBLE PerformanceMiliSeconds = 0.0;
	PerformanceMiliSeconds = (ATD->CounterEnd.QuadPart - ATD->CounterBegin.QuadPart) / (DOUBLE)Frequency.QuadPart * 1000;
	ATD->LastRecordedCounter = ATD->CounterEnd;

	if( bErase )
	{		
		delete ATD;
		TimerDataHashMap.erase(TimerName);
	}

	ATimerSystem::MutexUnLock();

	return PerformanceMiliSeconds;
}

FLOAT	ATimerSystem::GetTimerFullTimeFloat( const CHAR* TimerName, BOOL bErase )
{
	return static_cast<FLOAT>( GetTimerFullTime( TimerName, bErase ) * 0.001f );
}


DOUBLE	ATimerSystem::GetMainTick()
{
	return GetTimerTick( MainTimerName );
}

FLOAT	ATimerSystem::GetMainTickFloat()
{
	return static_cast<FLOAT>( GetTimerTick( MainTimerName ) * 0.001f );
}

FLOAT	ATimerSystem::ATimerSystemTick()
{
	FLOAT Tick = GetMainTickFloat();

	MutexLock();

	static std::list<ATimerData*>::const_iterator citer;
	for( citer = FunctionTimerList.begin(); citer != FunctionTimerList.end(); ++citer )
	{
		if( UpdateFunctionTimer( (*citer) ) == FALSE ) // 제 몫을 다한 놈
		{
			TimerDataHashMap.erase( (*citer)->FunctionName.c_str() );
			citer = FunctionTimerList.erase( citer );
			if( citer == FunctionTimerList.end() )
				break;
		}
	}

	MutexUnLock();

	return Tick;
}

DOUBLE	ATimerSystem::GetMainFullTime()
{
	return GetTimerFullTime( MainTimerName );
}

FLOAT	ATimerSystem::GetMainFulltimeFloat()
{
	return GetTimerFullTimeFloat( MainTimerName );
}
BOOL	ATimerSystem::SetFunctionTimer( const CHAR* FunctionName, ACallBack_VOIDToVOID function_ptr, FLOAT TimerTick )
{
	ATimerData* ATD = BeginTimer( FunctionName );

	if( ATD == NULL || function_ptr == NULL )
	{
		printf( "Wrong Timer Name : %s\n", FunctionName );
		return FALSE;
	}

	ATD->bUseFunctionCall = TRUE;
	ATD->FunctionName = FunctionName;
	ATD->CallFunctionPtr = function_ptr;
	ATD->CallTimeCycle = TimerTick;

	FunctionTimerList.push_back(ATD);

	return TRUE;
}

VOID	ATimerSystem::ClearFunctionTimer( const CHAR* FunctionName )
{
	// 여기서 FunctionTimerList.erase 해버리면 ATimerSystemTick 에서 뻑남
	// 일단 사용 안함으로만 만들고 ATimerSystemTick 에서 사용안하는것들 삭제 하자

	static std::list<ATimerData*>::const_iterator citer;
	for( citer = FunctionTimerList.begin(); citer != FunctionTimerList.end(); ++citer )
	{
		if( strcmp( FunctionName, (*citer)->FunctionName.c_str() ) == 0 )
		{
			(*citer)->bUseFunctionCall = FALSE;
			(*citer)->FunctionName = std::string("");
			(*citer)->CallFunctionPtr = NULL;
			(*citer)->CallTimeCycle = 0.f;
			(*citer)->CallCount = 0;

			return;
		}
	}
}


BOOL ATimerSystemBegin()
{
	return ATimerSystem::InitATimerSystem();
}