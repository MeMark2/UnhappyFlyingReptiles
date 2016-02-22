/*
File:		UFRMainWindow.h
Project:	Unhappy Flying Reptiles
Author(s):	Jorge Ramirez
Description:
	This file contains the class definition for the UFRMainWindow class.
*/

#pragma once
#include "afxwin.h"
#include <gdiplus.h>
#include "UFRGame.h"

using namespace Gdiplus;


/*
Name: UFRMainWindow
Inherit: public CFrameWnd
Description:
	This class is designed to handle the window and game timer logic for the
	Unhappy Flying Reptiles game.
*/
class UFRMainWindow : public CFrameWnd
{
private:
	ULONG_PTR gdiplusToken;
	GdiplusStartupInput gdiplusStartupInput;
	UFRGame* gameLogic;
	int gameLoopTimerID;
	int redrawTimerID;

protected:
	afx_msg void OnPaint();
	DECLARE_MESSAGE_MAP();

public:
	UFRMainWindow();
	~UFRMainWindow();
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg void OnClose();
};

