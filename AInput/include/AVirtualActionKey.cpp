#include "AVirtualActionKey.h"

static std::string UnRegisteredKeyName("UnRegisteredKeyName");

std::vector< AsciiBindData > AVirtualActionKey::Config_AsciiInfoList;
std::vector< VKeyAllData > AVirtualActionKey::Config_ActionData;

AVirtualActionKey::AVirtualActionKey()
{


}

AVirtualActionKey::~AVirtualActionKey()
{


}

VOID	AVirtualActionKey::ConfigBindAllKeyActionData()
{
	// 여기서 KeyBind시킴

}

BOOL	AVirtualActionKey::IsValidKey( INT AsciiCode )
{
	return Config_AsciiInfoList[AsciiCode].bValidAsciiCode;
}

const std::string&	AVirtualActionKey::GetVKeyName( INT AsciiCode ) const
{
	return ( Config_AsciiInfoList[AsciiCode].bValidAsciiCode ) ? Config_AsciiInfoList[AsciiCode].VKeyName : UnRegisteredKeyName;
}

const INT		AVirtualActionKey::GetVKeyIndex( INT AsciiCode ) const
{
	return Config_AsciiInfoList[AsciiCode].ActionKeyIndex;
}

VKeyAllData* AVirtualActionKey::GetVKeyAllInfo( INT AsciiCode )
{
	if( IsValidKey(AsciiCode) == FALSE )
		return NULL;

	INT VKeyIndex = GetVKeyIndex( AsciiCode );
	return ( VKeyIndex > 0 ) ? ( &Config_ActionData[ VKeyIndex ] ) : ( NULL );
}

VOID	AVirtualActionKey::OnKeyDown( INT AsciiCode )
{
	VKeyAllData* KeyActionInfo = NULL;
	if( NULL == ( KeyActionInfo = GetVKeyAllInfo( AsciiCode ) ) )
	{
		return;
	}

	const std::vector< ActionBindData >& ActionList = KeyActionInfo->OnStartActions;
	for( unsigned int i=0; i<ActionList.size(); i++ )
	{
		DeferredKeyActionList.push( ActionList[i] );
	}
}

VOID	AVirtualActionKey::OnKeyUp( INT AsciiCode )
{
	VKeyAllData* KeyActionInfo = NULL;
	if( NULL == ( KeyActionInfo = GetVKeyAllInfo( AsciiCode ) ) )
	{
		return;
	}

	const std::vector< ActionBindData >& ActionList = KeyActionInfo->OnReleaseActions;
	for( unsigned int i=0; i<ActionList.size(); i++ )
	{
		DeferredKeyActionList.push( ActionList[i] );
	}
}

VOID	AVirtualActionKey::OnMouseKeyDown( EMouseBtn ButtonType )
{
	const std::vector< ActionBindData >& ActionList = MouseActionDataList[ButtonType].OnStartActions;
	for( unsigned int i=0; i<ActionList.size(); i++ )
	{
		DeferredKeyActionList.push( ActionList[i] );
	}
}

VOID	AVirtualActionKey::OnMouseKeyUp( EMouseBtn ButtonType )
{
	const std::vector< ActionBindData >& ActionList = MouseActionDataList[ButtonType].OnReleaseActions;
	for( unsigned int i=0; i<ActionList.size(); i++ )
	{
		DeferredKeyActionList.push( ActionList[i] );
	}
}

VOID	AVirtualActionKey::OnMouseDoubleClick( EMouseBtn ButtonType )
{
	const std::vector< ActionBindData >& ActionList = MouseDoubleClickDataList[ButtonType].OnStartActions;
	for( unsigned int i=0; i<ActionList.size(); i++ )
	{
		DeferredKeyActionList.push( ActionList[i] );
	}
}

VOID	AVirtualActionKey::ProcessDeferredKeyAction()
{
	ActionBindData Action;

	while( !DeferredKeyActionList.empty() ) // push된 Action 모두 실행하고 나감
	{
		Action = DeferredKeyActionList.front();
		Action.KeyActionFunction( Action.BindValue );

		DeferredKeyActionList.pop();
	}
}