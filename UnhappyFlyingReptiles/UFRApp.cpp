/*
File:		UFRApp.cpp
Project:	Unhappy Flying Reptiles
Author(s):	Jorge Ramirez
Description:
	This file contains the class and method definitions for the UFRApp.
*/

#include <afxwin.h>
#include "UFRMainWindow.h"

#define DEFAULT_WINDOW_WIDTH 640
#define DEFAULT_WINDOW_HEIGHT 400

using namespace Gdiplus;


/*
Name: UFRApp
Inherit: public CWinApp
Description:
	This class is designed to handle the creation of the Unhappy Flying Reptiles main app.
	The window for the game is created here.
*/
class UFRApp :public CWinApp
{
private:
	UFRMainWindow *gameWindow;

public:

	/*
	Name:	InitInstance()
	Params: void
	Return: BOOL - TRUE
	Description:
		This is executes during intialization of the CWinApp.
		The main game window is created here and displayed.
	*/
	BOOL InitInstance()
	{
		gameWindow = new UFRMainWindow();
		m_pMainWnd = gameWindow;
		m_pMainWnd->SetWindowPos(&CWnd::wndTop, 0, 0, DEFAULT_WINDOW_WIDTH, DEFAULT_WINDOW_HEIGHT, SWP_NOMOVE);
		m_pMainWnd->ShowWindow(SW_SHOW);
		return TRUE;
	}



	/*
	Name:	ExitInstance()
	Params: void
	Return: int
	Description:
		This is executes during shutting down of the CWinApp.
	*/
	int ExitInstance()
	{
		return CWinApp::ExitInstance();
	}
};

// Starts the app.
UFRApp gameApp;