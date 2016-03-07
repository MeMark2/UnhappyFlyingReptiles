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
#define SLINGSHOT_SCALE 0.7
#define REPTILE_SCALE 0.7

#define INIT_LEFT_OFFSET 0
#define INIT_GROUND_OFFSET 300
#define DEFAULT_HORIZONTAL_VELOCITY 10
#define DEFAULT_VERTICAL_VELOCITY 0

#define REPTILE_RESET_TICKS 15



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

	// The natural size of the background image
	imageWidth = background->GetWidth();
	imageHeight = background->GetHeight();

	// Calculate the scaled size of the reptile
	scaleRptlWidth = reptile->GetWidth() * REPTILE_SCALE;
	scaleRptlHeight = reptile->GetHeight() * REPTILE_SCALE;

	// Start the reptile off the screen
	reptileLogic = new UFReptileLogic(0 - scaleRptlWidth, INIT_GROUND_OFFSET, DEFAULT_HORIZONTAL_VELOCITY, DEFAULT_VERTICAL_VELOCITY);
	deadTicks = 0; 
	reptileFliesLeft = false;
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

	// Calculate the scaled size of the slingshot
	int scaleSlngWidth = slingshot1->GetWidth() * SLINGSHOT_SCALE;
	int scaleSlngHeight = slingshot1->GetHeight() * SLINGSHOT_SCALE;

	// If the reptile is out of bounds of the screen, draw it on the other side
	if (reptileLogic->GetLeftOffset() > imageWidth)
	{
		reptileLogic->SetLeftOffset(-scaleRptlWidth);
	}
	else if (reptileLogic->GetLeftOffset() < -scaleRptlWidth)
	{
		reptileLogic->SetLeftOffset(imageWidth);
	}

	// Draw Backdrop to buffer
	bufferCanvas->DrawImage(background, 0, 0);
	bufferCanvas->DrawImage(midground, 0, 0);
	bufferCanvas->DrawImage(foreground, 0, 0);

	// Draw further part of slingshot to buffer
	bufferCanvas->DrawImage(slingshot1, 0, imageHeight - scaleSlngHeight, scaleSlngWidth, scaleSlngHeight);

	// Set up tranformations for reptile:
	// center, rotate, and return to original offset
	bufferCanvas->TranslateTransform(-(reptileLogic->GetLeftOffset() + scaleRptlWidth / 2), -(imageHeight - reptileLogic->GetBottomOffset() - scaleRptlHeight / 2) );
	bufferCanvas->RotateTransform(reptileLogic->GetReptileRotation(), MatrixOrderAppend);
	bufferCanvas->TranslateTransform((reptileLogic->GetLeftOffset() + scaleRptlWidth / 2), (imageHeight - reptileLogic->GetBottomOffset() - scaleRptlHeight / 2), MatrixOrderAppend);

	// Draw reptile with transformations
	bufferCanvas->DrawImage(reptile, reptileLogic->GetLeftOffset(), imageHeight - scaleRptlHeight - reptileLogic->GetBottomOffset(),
		scaleRptlWidth, scaleRptlHeight);

	// Clear transformations
	bufferCanvas->ResetTransform();

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
	int newHorizontalVelocity = DEFAULT_HORIZONTAL_VELOCITY;

	// Calculate new reptile location.
	reptileLogic->Tick();

	// If the reptile has been dead for enough ticks, reset its velocity and starting location.
	// Else, if it is dead, add to the deadTicks count.
	if (deadTicks >= REPTILE_RESET_TICKS)
	{
		// Swap reptile flight direction
		reptileFliesLeft = !reptileFliesLeft;
		if (reptileFliesLeft)
		{
			newHorizontalVelocity = -newHorizontalVelocity;
		}

		reptileLogic->SetOffsetAndVelocity(0 - scaleRptlWidth, INIT_GROUND_OFFSET, newHorizontalVelocity, DEFAULT_VERTICAL_VELOCITY);
		reptileLogic->SetReptileState(REPTILE_STATE_FLYING);
		
		// Reset dead ticks
		deadTicks = 0;
	}
	else if (reptileLogic->GetHorizontalVel() == 0 && reptileLogic->GetVerticaltalVel() == 0)
	{
		deadTicks++;
	}
}


void UFRGame::Click(int windowX, int windowY, CRect* windowDimensions)
{
	// Calculate the mouse click in relation to the image resolution
	int x = windowX * (imageWidth / (float) windowDimensions->Width());
	int y = windowY * (imageHeight / (float) windowDimensions->Height());


	// If reptile is clicked, change to falling state
	if (x > reptileLogic->GetLeftOffset() && x < reptileLogic->GetLeftOffset() + scaleRptlWidth &&
		y > imageHeight - (reptileLogic->GetBottomOffset() + scaleRptlHeight) && y < imageHeight - reptileLogic->GetBottomOffset())
	{
		reptileLogic->SetReptileState(REPTILE_STATE_FALLING);
	}
}