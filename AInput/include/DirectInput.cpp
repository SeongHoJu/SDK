#include "stdio.h"

#include "AInput.h"

DirectInputData::DirectInputData()
{
	memset(KeyState, 0, KEYSTATE_BUFFERSIZE);
	memset(OldKeyState, 0, KEYSTATE_BUFFERSIZE);
	memset(NewKeyState, 0, KEYSTATE_BUFFERSIZE);
	memset(MKeyState, 0, EMouseBtn_Max);
	memset(MOldKeyState, 0, EMouseBtn_Max);
	memset(MNewKeyState, 0, EMouseBtn_Max);
}

DirectInputData::~DirectInputData()
{
}

DirectInputSystem::DirectInputSystem()
	: DirectInputInterface( NULL ),
	  KeyboardDevice( NULL ),
	  MouseDevice( NULL ),
	  InstanceHandle( NULL ),
	  WindowHandle( NULL )
{
	::ZeroMemory( &InputBuffer, sizeof( DirectInputData) );
	::ZeroMemory( &ScreenRectInfo, sizeof( RECT ) );
}

DirectInputSystem::~DirectInputSystem()
{
}

BOOL	DirectInputSystem::InitDirectInputSystem( HINSTANCE hInst, HWND hWnd )
{
	if( FAILED( DirectInput8Create( hInst, 
									DIRECTINPUT_VERSION, 
									IID_IDirectInput8, 
									(VOID**)DirectInputInterface,
									NULL ) ) )
	{
		printf("DirectInput8Create Failed\n");
		return FALSE;
	}

	if( InitKeyboardDevice( hWnd ) == FALSE )
	{
		printf("InitKeyboardDevice Failed\n");
		return FALSE;
	}

	if( InitMouseDevice( hWnd ) == FALSE )
	{
		printf("InitMouseDevice Failed\n");
		return FALSE;
	}

	CleanInputBuffer();
	SetScreenRectInfo( hWnd );
}

VOID	DirectInputSystem::ReleaseDirectInputSystem()
{
	if( DirectInputInterface ) { DirectInputInterface->Release(); DirectInputInterface = NULL; }
	if( KeyboardDevice ) { KeyboardDevice->Unacquire(); KeyboardDevice->Release(); KeyboardDevice = NULL; }
	if( MouseDevice ) { MouseDevice->Unacquire(); MouseDevice->Release(); MouseDevice = NULL; }	
}

BOOL	DirectInputSystem::InitKeyboardDevice(HWND hWnd)
{
	if( FAILED( DirectInputInterface->CreateDevice( GUID_SysKeyboard, &KeyboardDevice, NULL) ) )
		return FALSE;
	
	if( FAILED( KeyboardDevice->SetDataFormat(&c_dfDIKeyboard) ) )
		return FALSE;
	
	HRESULT result = KeyboardDevice->SetCooperativeLevel( hWnd, DISCL_NONEXCLUSIVE | DISCL_FOREGROUND );
	if( result == DIERR_UNSUPPORTED || FAILED( result ) )
		return FALSE;

	KeyboardDevice->Acquire();
	
	return TRUE;
}

BOOL	DirectInputSystem::InitMouseDevice(HWND hWnd)
{
	if( FAILED( DirectInputInterface->CreateDevice( GUID_SysMouse, &MouseDevice, NULL) ) ) 
		return FALSE;
	if( FAILED( MouseDevice->SetDataFormat(&c_dfDIMouse) ) ) 
		return FALSE;

	HRESULT result = MouseDevice->SetCooperativeLevel( hWnd, DISCL_NONEXCLUSIVE | DISCL_FOREGROUND );

	if( result == DIERR_UNSUPPORTED || FAILED( result ) ) 
		return FALSE;

	MouseDevice->Acquire();

	return TRUE;
}

VOID	DirectInputSystem::CleanInputBuffer()
{
	::ZeroMemory(InputBuffer.OldKeyState, sizeof(BYTE) * KEYSTATE_BUFFERSIZE);
	::ZeroMemory(InputBuffer.NewKeyState, sizeof(BYTE) * KEYSTATE_BUFFERSIZE);
	::ZeroMemory(InputBuffer.KeyState,	  sizeof(BYTE) * KEYSTATE_BUFFERSIZE);
	::ZeroMemory(&InputBuffer.MouseState, sizeof(BYTE) * KEYSTATE_BUFFERSIZE);
}

VOID	DirectInputSystem::SetScreenRectInfo( HWND hWnd )
{
	if( hWnd == NULL )
		return;
	
	GetClientRect(hWnd, &ScreenRectInfo);
}

BOOL	DirectInputSystem::RestoreInputDevice( IDirectInputDevice8* Device )
{
	if( Device )
	{
		int CheckCount = 0;
		while( (Device->Acquire() == DIERR_INPUTLOST) && CheckCount < DEVICELOST_CHECKLIMITCOUNT )
		{
			CheckCount++;
		}
	}

	return TRUE;
}

BOOL	DirectInputSystem::ReadKeyboardInput()
{
	if( KeyboardDevice && DI_OK != KeyboardDevice->GetDeviceState( sizeof(BYTE) * KEYSTATE_BUFFERSIZE, InputBuffer.NewKeyState ) )
		return RestoreInputDevice( KeyboardDevice );

	RecordKeyboardUpDown();

	return TRUE;
}

BOOL	DirectInputSystem::ReadMouseInput()
{
	if( MouseDevice && DI_OK != MouseDevice->GetDeviceState( sizeof(DIMOUSESTATE), (LPVOID)&InputBuffer.MouseState) )
		return RestoreInputDevice( MouseDevice );

	RecordMouseUpDown();

	return TRUE;

}

BOOL	DirectInputSystem::ProcessInput()
{
	if( ReadKeyboardInput() && ReadMouseInput() )
		return TRUE;
	
	return FALSE;
}

VOID	DirectInputSystem::RecordKeyboardUpDown()
{
	for(int i=0; i<KEYSTATE_BUFFERSIZE; i++)
	{
		if(InputBuffer.NewKeyState[i] & 0x80)
		{
			if(!(InputBuffer.OldKeyState[i] & 0x80))
				InputBuffer.KeyState[i] = 0x01;
	
			else if(InputBuffer.OldKeyState[i] & 0x80)
				InputBuffer.KeyState[i] = 0x81;
		}

		else
		{
			if(!(InputBuffer.OldKeyState[i] & 0x80))
				InputBuffer.KeyState[i] = 0x00;
			else if(InputBuffer.OldKeyState[i] & 0x80)
				InputBuffer.KeyState[i] = 0x80;
		}
	}

	memcpy(InputBuffer.OldKeyState, InputBuffer.NewKeyState, sizeof(BYTE) * KEYSTATE_BUFFERSIZE);
}

VOID	DirectInputSystem::RecordMouseUpDown()
{
	for(int i=0; i<EMouseBtn_Max; i++)
	{
		InputBuffer.MNewKeyState[i] = InputBuffer.MouseState.rgbButtons[i];
		if(InputBuffer.MNewKeyState[i] & 0x80)				
		{
			if(!(InputBuffer.MOldKeyState[i] & 0x80)) 
				InputBuffer.MKeyState[i] = 0x01;
			
			else if(InputBuffer.MOldKeyState[i] & 0x80)			
				InputBuffer.MKeyState[i] = 0x81;	
		}
		else 
		{
			if(!(InputBuffer.MOldKeyState[i] & 0x80))		
				InputBuffer.MKeyState[i] = 0x00;
			else if(InputBuffer.MOldKeyState[i] & 0x80)
				InputBuffer.MKeyState[i] = 0x80;
		}
	}

	memcpy(InputBuffer.MOldKeyState, InputBuffer.MNewKeyState, sizeof(BYTE) * EMouseBtn_Max);
}
