#ifndef _DIRECT_INPUT_SYSTEM_H_
#define _DIRECT_INPUT_SYSTEM_H_


#include "dinput.h"
#pragma comment(lib, "dinput8.lib")
#pragma comment(lib, "dxguid.lib")

enum EMouseBtn
{
	EMouseBtn_Left,
	EMouseBtn_Right,
	EMouseBtn_Wheel,

	EMouseBtn_Max
};

#define KEYSTATE_BUFFERSIZE 256
#define DEVICELOST_CHECKLIMITCOUNT	1000


struct DirectInputData
{
	BYTE			KeyState[KEYSTATE_BUFFERSIZE];
	DIMOUSESTATE	MouseState;
	POINT			MousePos;

	BYTE			OldKeyState[KEYSTATE_BUFFERSIZE];
	BYTE			NewKeyState[KEYSTATE_BUFFERSIZE];

	BYTE			MKeyState[EMouseBtn_Max];
	BYTE			MOldKeyState[EMouseBtn_Max];
	BYTE			MNewKeyState[EMouseBtn_Max];

	DirectInputData();
	~DirectInputData();
};

class DirectInputSystem
{
	IDirectInput8*			DirectInputInterface;
	IDirectInputDevice8*	KeyboardDevice;
	IDirectInputDevice8*	MouseDevice;

	DirectInputData			InputBuffer;

	HINSTANCE				InstanceHandle;
	HWND					WindowHandle;

	RECT					ScreenRectInfo;

public:

	DirectInputSystem();
	~DirectInputSystem();

	BOOL	InitDirectInputSystem( HINSTANCE hInst, HWND hWnd );
	VOID	ReleaseDirectInputSystem();
	
	BOOL	InitKeyboardDevice(HWND hWnd);
	BOOL	InitMouseDevice(HWND hWnd);

	VOID	CleanInputBuffer();
	VOID	SetScreenRectInfo( HWND hWnd );

	BOOL	RestoreInputDevice( IDirectInputDevice8* Device );

public:

	BOOL	ReadKeyboardInput();
	BOOL	ReadMouseInput();
	BOOL	ProcessInput();

	VOID	RecordKeyboardUpDown();
	VOID	RecordMouseUpDown();

public:

	BOOL	GetKeyDown( BYTE DIKValue );
	BOOL	GetKeyUp( BYTE DIKValue );

	BOOL	GetMouseDown( EMouseBtn MouseButtonType );
	BOOL	GetMouseUp( EMouseBtn MouseButtonType );	// 이부분 cpp 에 완성하자
	

};


#endif