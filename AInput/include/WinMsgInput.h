#ifndef _A_WINDOWS_MESSAGE_INPUT_H_
#define _A_WINDOWS_MESSAGE_INPUT_H_

#include "windows.h"
class AVirtualActionKey;

typedef LRESULT (*DeferredMessageFunction)(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

class AWinMsgInput
{
private:
	
	DeferredMessageFunction DeferredWindowProc;
	AVirtualActionKey*		VirtualKeyAction;

public:

	AWinMsgInput( DeferredMessageFunction WindowDeferredMessageFunction, AVirtualActionKey* VirtualKeyActionClass );
	~AWinMsgInput();

	LRESULT WinInputMsgProcedure(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);





};


#endif