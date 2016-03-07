/*
File:		UFRMainWindow.cpp
Project:	Unhappy Flying Reptiles
Author(s):	Jorge Ramirez
Description:
	This file contains the method definitions for the UFRMainWindow class.
*/

#include "UFRMainWindow.h"

#define DEFAULT_GAME_LOOP_TIMER_ID 1
#define GAME_LOOP_INTERVAL 50
#define DEFAULT_REDRAW_TIMER_ID 2
#define REDRAW_INTERVAL 1000/60

// Map the window messages to methods.
BEGIN_MESSAGE_MAP(UFRMainWindow, CFrameWnd)
	ON_WM_PAINT()
	ON_WM_ERASEBKGND()
	ON_WM_TIMER()
	ON_WM_CLOSE()
	ON_WM_LBUTTONDOWN()
END_MESSAGE_MAP()


/*
Name:	UFRMainWindow()
Params: void
Return: void
Description:
	This is the constructor for the UFRMainWindow class.
	GDI+ is started here along with the window and the timers.
*/
UFRMainWindow::UFRMainWindow()
{
	// Start GDI+
	GdiplusStartup(&gdiplusToken, &gdiplusStartupInput, NULL);

	// Create the window and window title
	Create(NULL, TEXT("Unhappy Flying Reptiles"));

	// Set the game and redraw timers
	gameLoopTimerID = DEFAULT_GAME_LOOP_TIMER_ID;
	gameLoopTimerID = SetTimer(gameLoopTimerID, GAME_LOOP_INTERVAL, NULL);
	redrawTimerID = DEFAULT_REDRAW_TIMER_ID;
	redrawTimerID = SetTimer(redrawTimerID, REDRAW_INTERVAL, NULL);

	gameLogic = new UFRGame();
}



/*
Name:	~UFRMainWindow()
Params: void
Return: void
Description:
	This is the destructor for the UFRMainWindow class.
	GDI+ is shut down here.
*/
UFRMainWindow::~UFRMainWindow()
{
	delete gameLogic;

	// Shutdown GDI+
	GdiplusShutdown(gdiplusToken);
}



/*
Name:	OnPaint()
Params: void
Return: void
Description:
	This method is called by the window any time the paint message is received by the window.
	The device context for the window is created here and the current game state is drawn.
*/
void UFRMainWindow::OnPaint()
{
	// Get device context, create Graphics object and find the dimensions of the window
	CPaintDC dc(this);
	Graphics canvas(dc.m_hDC);
	CRect windowDimensions;
	GetClientRect(windowDimensions);

	// Draw the game onto the Grphics object
	gameLogic->Draw(&canvas, &windowDimensions);
}



/*
Name:	OnEraseBkgnd()
Params: void
Return: BOOL - Whether or not the background will be erased.
Description:
	This method is called any time the device context is being redrawn.
*/
BOOL UFRMainWindow::OnEraseBkgnd(CDC* pDC)
{
	// This prevents the window background from being erased for every redraw
	return TRUE;
}



/*
Name:	OnTimer()
Params: 
	UINT_PTR nIDEvent - The identifier for the timer event fired.
Return: void
Description:
	This method exectutes every time a timer message is received.
*/
void UFRMainWindow::OnTimer(UINT_PTR nIDEvent)
{
	if (nIDEvent == gameLoopTimerID)
	{
		// Calculate new game state
		gameLogic->CalcGameState();
	}
	if (nIDEvent == redrawTimerID)
	{
		// Redraw screen
		Invalidate();
	}

	// Call the handler for the base class
	CFrameWnd::OnTimer(nIDEvent);
}



/*
Name:	OnClose()
Params: void
Return: void
Description:
	This method executes when the user exits the window.
	The timers are shut down here.
*/
void UFRMainWindow::OnClose()
{
	KillTimer(gameLoopTimerID);
	KillTimer(redrawTimerID);

	CFrameWnd::OnClose();
}


void UFRMainWindow::OnLButtonDown(UINT nFlags, CPoint point)
{
	CRect windowDimensions;
	GetClientRect(windowDimensions);
	gameLogic->Click(point.x, point.y, &windowDimensions);

	CFrameWnd::OnLButtonDown(nFlags, point);
}
