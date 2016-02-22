/*
File:		UFRGame.cpp
Project:	Unhappy Flying Reptiles
Author(s):	Jorge Ramirez
Description:
	This file contains the method definitions for the UFRGame class.
*/

#include "UFRGame.h"
#include <list>

#define BYTES_PER_PIXEL 4
#define SLINGSHOT_SCALE 0.5
#define REPTILE_SCALE 0.5

#define INIT_LEFT_OFFSET 0
#define INIT_GROUND_OFFSET 35
#define DEFAULT_HORIZONTAL_VELOCITY 10
#define DEFAULT_VERTICAL_VELOCITY 20



/*
Name:	UFRGame()
Params: void
Return: void
Description:
	Constructor for the UFRGame class.
	The necessary game bitmaps are loaded and color adjusted here.
*/
UFRGame::UFRGame()
{
	reptileLogic = new UFReptileLogic(INIT_LEFT_OFFSET, INIT_GROUND_OFFSET);

	background = new Bitmap(TEXT(".\\Background.bmp"));
	midground = new Bitmap(TEXT(".\\Midground.bmp"));
	foreground = new Bitmap(TEXT(".\\Foreground.bmp"));

	slingshot1 = new Bitmap(TEXT(".\\slingshot1.png"));
	slingshot2 = new Bitmap(TEXT(".\\slingshot2.png"));

	reptile = new Bitmap(TEXT(".\\reptile.png"));

	buffer = background->Clone(0, 0, background->GetWidth(), background->GetHeight(), PixelFormat32bppARGB);
	bufferCanvas = Graphics::FromImage(buffer);

	MakeTransparent(midground, Color(0, 255, 0));
	MakeTransparent(foreground, Color(0, 255, 0));
}



/*
Name:	~UFRGame()
Params: void
Return: void
Description:
	Destructor for the UFRGame class.
	Allocated memory is freed.
*/
UFRGame::~UFRGame()
{
	delete background;
	delete midground;
	delete foreground;

	delete slingshot1;
	delete slingshot2;

	delete reptile;

	delete buffer;
	delete bufferCanvas;

	delete reptileLogic;
}



/*
Name:	MakeTransparent()
Params: 
	Bitmap* bmp - The image to change the colors of.
	Color color - The color to change to transparent.
Return: void
Description:
	This method finds every pixel within a bitmap of a specific color and makes it transparent.
*/
void UFRGame::MakeTransparent(Bitmap* bmp, Color color)
{
	int width = bmp->GetWidth();
	int length = bmp->GetHeight();
	Rect bmpDimensions(0, 0, width, length);
	BitmapData* bmpData = new BitmapData;
	UINT* pixels;

	// Convert the color into a single composite value
	UINT colorNum = color.GetA();
	colorNum = color.GetR() + (colorNum << 8);
	colorNum = color.GetG() + (colorNum << 8);
	colorNum = color.GetB() + (colorNum << 8);

	bmp->LockBits(&bmpDimensions, ImageLockModeWrite, PixelFormat32bppARGB, bmpData);
	pixels = (UINT*)bmpData->Scan0;


	// Find every pixel of the specific color
	for (int pixel = 0; pixel < width * length; pixel++)
	{
		if (pixels[pixel] == colorNum)
		{
			pixels[pixel] = 0;
		}
	}

	bmp->UnlockBits(bmpData);

	delete bmpData;
}



/*
Name:	Draw()
Params: 
	Graphics* canvas - The Graphics object to draw the game images onto.
	CRect* dimensions - The desired dimensions of the images to be drawn to the Graphics object.
Return: void
Description:
	This method draws all the game images onto a Graphics object.
*/
void UFRGame::Draw(Graphics* canvas, CRect* dimensions)
{
	// The stretched size of the window
	int windowWidth = dimensions->Width();
	int windowHeight = dimensions->Height();

	// The natural size of the background image
	int imageWidth = background->GetWidth();
	int imageHeight = background->GetHeight();

	// Calculate the scaled size of the slingshot
	int scaleSlngWidth = slingshot1->GetWidth() * SLINGSHOT_SCALE;
	int scaleSlngHeight = slingshot1->GetHeight() * SLINGSHOT_SCALE;

	// Calculate the scaled size of the reptile
	int scaleRptlWidth = reptile->GetWidth() * REPTILE_SCALE;
	int scaleRptlHeight = reptile->GetHeight() * REPTILE_SCALE;

	// Draw Backdrop to buffer
	bufferCanvas->DrawImage(background, 0, 0);
	bufferCanvas->DrawImage(midground, 0, 0);
	bufferCanvas->DrawImage(foreground, 0, 0);

	// Draw further part of slingshot to buffer
	bufferCanvas->DrawImage(slingshot1, 0, imageHeight - scaleSlngHeight, scaleSlngWidth, scaleSlngHeight);

	// Draw reptile to buffer
	bufferCanvas->DrawImage(reptile, reptileLogic->GetLeftOffset(), imageHeight - scaleRptlHeight - reptileLogic->GetBottomOffset(),
		scaleRptlWidth, scaleRptlHeight);

	// Draw closer part of slingshot to buffer
	bufferCanvas->DrawImage(slingshot2, 0, imageHeight - scaleSlngHeight, scaleSlngWidth, scaleSlngHeight);

	// Draw buffer to canvas
	canvas->DrawImage(buffer, 0, 0, windowWidth, windowHeight);
}


/*
Name:	CalcGameState()
Params: void
Return: void
Description:
	This method calculates a new game state every time it is called.
*/
void UFRGame::CalcGameState()
{
	// Calculate new reptile location.
	reptileLogic->Tick();

	// If the reptile has stopped moving, reset itsvelocity and starting location.
	if (reptileLogic->GetHorizontalVel() == 0 && reptileLogic->GetVerticaltalVel() == 0)
	{
		reptileLogic->SetOffsetAndVelocity(INIT_LEFT_OFFSET, INIT_GROUND_OFFSET, DEFAULT_HORIZONTAL_VELOCITY, DEFAULT_VERTICAL_VELOCITY);
	}
}