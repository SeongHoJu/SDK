#ifndef _A_VIRTUAL_ACTION_KEY_H_
#define _A_VIRTUAL_ACTION_KEY_H_

#include <string>
#include <vector>
#include <hash_map>
#include <queue>
#include "Windows.h"


#ifndef TO_STRING 
#define TO_STRING(s) #s
#endif

typedef VOID (*KeyActionINT)(INT);

struct AsciiBindData
{
	BOOL			bValidAsciiCode;
	std::string		VKeyName;
	INT				ActionKeyIndex; // 해당 AsciiCode가 Config_ActionData의 몇번째 Index인지

	AsciiBindData() : bValidAsciiCode( FALSE ), VKeyName(""), ActionKeyIndex(-1) {}
	~AsciiBindData() {}
};

struct ActionBindData
{
	std::string		KeyActionName;
	INT				BindValue;
	KeyActionINT	KeyActionFunction;
	
	ActionBindData() : KeyActionName(""), BindValue( 0 ) { KeyActionFunction = NULL; }
	ActionBindData( const ActionBindData& rhs )
		: KeyActionName( rhs.KeyActionName ), BindValue( rhs.BindValue )
	{
		KeyActionFunction = rhs.KeyActionFunction;
	}
};

struct VKeyAllData
{
	std::vector< ActionBindData > OnStartActions;
	std::vector< ActionBindData > OnReleaseActions;

	VKeyAllData() {}
	~VKeyAllData() {}
};


enum EMouseBtn
{
	EMouseBtn_Left,
	EMouseBtn_Right,
	EMouseBtn_Wheel,

	EMouseBtn_Max
};


class AVirtualActionKey
{
private:

	// 활용되는 AsciiCode 인지 확인 ( 256개 들어있어 Ascii값 그대로 Indexing )
	static std::vector< AsciiBindData >		Config_AsciiInfoList; 

	// Config로 빼서 Key이름에 함수 연결
	static std::vector< VKeyAllData >		Config_ActionData;

	// Key이름 검색을 빠르게 할 HashMap
	stdext::hash_map<const CHAR*, VKeyAllData*>	KeyActionDataHashMap;

	// Insert된 Key에 따라 지연 실행해야 할 KeyAction Queue
	std::queue< ActionBindData > DeferredKeyActionList;

	VKeyAllData					MouseActionDataList[EMouseBtn_Max];
	VKeyAllData					MouseDoubleClickDataList[EMouseBtn_Max];

public:

	AVirtualActionKey();
	~AVirtualActionKey();


	VOID				ConfigBindAllKeyActionData();

	BOOL				IsValidKey( INT AsciiCode );
	const std::string&	GetVKeyName( INT AsciiCode ) const;
	const INT			GetVKeyIndex( INT AsciiCode ) const;

	VKeyAllData*		GetVKeyAllInfo( INT AsciiCode );


public:

	// 외부에서 사용할 키는 요거 세개

	VOID	OnKeyDown( INT AsciiCode );
	VOID	OnKeyUp( INT AsciiCode );

	VOID	OnMouseKeyDown( EMouseBtn ButtonType );
	VOID	OnMouseKeyUp( EMouseBtn ButtonType );

	VOID	OnMouseDoubleClick( EMouseBtn ButtonType );

	VOID	ProcessDeferredKeyAction();
};



#endif