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

	int imageWidth;
	int imageHeight;

	Bitmap* slingshot1;
	Bitmap* slingshot2;

	Bitmap* reptile;

	Bitmap* buffer;
	Graphics* bufferCanvas;

	UFReptileLogic* reptileLogic;
	int deadTicks; // To keep track of how long the reptile has been dead
	int scaleRptlWidth;
	int scaleRptlHeight;

	void MakeTransparent(Bitmap* bmp, Color color);

	bool reptileFliesLeft; // To keep track of the direction of flight of the reptile

public:
	UFRGame();
	~UFRGame();

	void Draw(Graphics* canvas, CRect* dimensions);
	void CalcGameState();

	void Click(int windowX, int windowY, CRect* windowDimensions);
};

