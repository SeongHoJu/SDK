#include "AVirtualActionKey.h"

#include "WinMsgInput.h"


AWinMsgInput::AWinMsgInput( DeferredMessageFunction WindowDeferredMessageFunction, 
							AVirtualActionKey* VirtualKeyActionClass )
	: DeferredWindowProc( WindowDeferredMessageFunction ),
	  VirtualKeyAction( VirtualKeyActionClass )
{

}

AWinMsgInput::~AWinMsgInput()
{


}


LRESULT AWinMsgInput::WinInputMsgProcedure(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch(msg){
		case WM_SYSKEYDOWN :
		case WM_KEYDOWN:	
			{
				const INT AsciiCode = wParam;
				VirtualKeyAction->OnKeyDown( AsciiCode );		
		
				return 0;
			}
			break;

		case WM_SYSKEYUP:
		case WM_KEYUP:
			{
				const INT AsciiCode = wParam;
				VirtualKeyAction->OnKeyUp( AsciiCode );
				
				return 0;
			}
			break;

		case WM_LBUTTONDBLCLK:
		case WM_LBUTTONDOWN:
			{
				if( msg == WM_LBUTTONDOWN )
				{
					VirtualKeyAction->OnMouseKeyDown( EMouseBtn_Left );
				}
				else
				{
					VirtualKeyAction->OnMouseDoubleClick( EMouseBtn_Left );
				}
				return 0;
			}
			break;

		case WM_MBUTTONDBLCLK:
		case WM_MBUTTONDOWN:
			{
				if( msg == WM_MBUTTONDOWN )
				{
					VirtualKeyAction->OnMouseKeyDown( EMouseBtn_Wheel );
				}
				else
				{
					VirtualKeyAction->OnMouseDoubleClick( EMouseBtn_Wheel );
				}
				return 0;
			}
			break;

		case WM_RBUTTONDBLCLK:
		case WM_RBUTTONDOWN:
			{
				if( msg == WM_RBUTTONDOWN )
				{
					VirtualKeyAction->OnMouseKeyDown( EMouseBtn_Right );
				}
				else
				{
					VirtualKeyAction->OnMouseDoubleClick( EMouseBtn_Right );
				}
				return 0;
			}
			break;

			// Mouse Button Up
		case WM_LBUTTONUP:
			{
				VirtualKeyAction->OnMouseKeyUp( EMouseBtn_Left );
				return 0;
			}
			break;

		case WM_MBUTTONUP:
			{
				VirtualKeyAction->OnMouseKeyUp( EMouseBtn_Wheel );
				return 0;
			}
			break;

		case WM_RBUTTONUP:
			{
				VirtualKeyAction->OnMouseKeyUp( EMouseBtn_Right );
				return 0;
			}
			break;
	}

	return DeferredWindowProc(hWnd, msg, wParam, lParam);
}
