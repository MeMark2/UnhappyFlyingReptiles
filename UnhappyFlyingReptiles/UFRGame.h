/*
File:		UFRGame.h
Project:	Unhappy Flying Reptiles
Author(s):	Jorge Ramirez
Description:
	This method contains the class definition for the UFRGame class.
*/

#pragma once
#include "afxwin.h"
#include <gdiplus.h>
#include "UFReptileLogic.h"

using namespace Gdiplus;


/*
Name: UFRGame
Description:
	This class is designed primarily to draw the images of the Unhappy Flying Reptiles according
	to the logic of the game.
*/
class UFRGame
{
private:
	Bitmap* background;
	Bitmap* midground;
	Bitmap* foreground;

	Bitmap* slingshot1;
	Bitmap* slingshot2;

	Bitmap* reptile;

	Bitmap* buffer;
	Graphics* bufferCanvas;

	UFReptileLogic* reptileLogic;

	void MakeTransparent(Bitmap* bmp, Color color);

public:
	UFRGame();
	~UFRGame();

	void Draw(Graphics* canvas, CRect* dimensions);
	void CalcGameState();
};

