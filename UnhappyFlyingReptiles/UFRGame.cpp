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

#define SOUND_CHANNELS 16


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
	// load game images
	background = new Bitmap(TEXT(".\\Background.bmp"));
	midground = new Bitmap(TEXT(".\\Midground.bmp"));
	foreground = new Bitmap(TEXT(".\\Foreground.bmp"));
	slingshot1 = new Bitmap(TEXT(".\\slingshot1.png"));
	slingshot2 = new Bitmap(TEXT(".\\slingshot2.png"));
	reptile = new Bitmap(TEXT(".\\reptile.png"));

	// Create and init fmod system
	FMOD::System_Create(&fmodSystem);
	fmodSystem->init(SOUND_CHANNELS, FMOD_INIT_NORMAL, NULL);

	// Init game sounds
	fmodSystem->createSound("shoot.wav", FMOD_HARDWARE, 0, &shootSound);
	fmodSystem->createSound("punch.wav", FMOD_HARDWARE, 0, &punchSound);
	fmodSystem->createSound("falling.wav", FMOD_HARDWARE, 0, &fallSound);
	fmodSystem->createSound("thud.wav", FMOD_HARDWARE, 0, &thudSound);

	// Create image buffer
	buffer = background->Clone(0, 0, background->GetWidth(), background->GetHeight(), PixelFormat32bppARGB);
	bufferCanvas = Graphics::FromImage(buffer);

	// Remove green from background and midground images
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
	floorHit = false;
	reptileFliesLeft = false;

	// Initiate mouse position
	mouseX = 0;
	mouseY = 0;
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

	// release game sounds
	shootSound->release();
	punchSound->release();
	fallSound->release();
	thudSound->release();

	// close and release fmod
	fmodSystem->close();
	fmodSystem->release();
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

	// Calc scaled mouse position
	int scaledMouseX = (mouseX / (float)windowWidth) * imageWidth;
	int scaledMouseY = (mouseY / (float)windowHeight) * imageHeight;

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

	// Set up tranformations for reptile:
	// center, rotate, and return to original offset
	bufferCanvas->TranslateTransform(-(reptileLogic->GetLeftOffset() + scaleRptlWidth / 2), 
		-(imageHeight - reptileLogic->GetBottomOffset() - scaleRptlHeight / 2) );
	bufferCanvas->RotateTransform(reptileLogic->GetReptileRotation(), MatrixOrderAppend);
	bufferCanvas->TranslateTransform((reptileLogic->GetLeftOffset() + scaleRptlWidth / 2), 
		(imageHeight - reptileLogic->GetBottomOffset() - scaleRptlHeight / 2), MatrixOrderAppend);

	// Draw reptile with transformations
	bufferCanvas->DrawImage(reptile, reptileLogic->GetLeftOffset(), 
		imageHeight - scaleRptlHeight - reptileLogic->GetBottomOffset(),
		scaleRptlWidth, scaleRptlHeight);

	// Clear transformations
	bufferCanvas->ResetTransform();

	// Draw slingshot to buffer at mouse postition 
	// (the center of the slingshot firing area is adjusted to the mouse position)
	bufferCanvas->DrawImage(slingshot1, scaledMouseX - (scaleSlngWidth / 2), 
		scaledMouseY - 15, scaleSlngWidth, scaleSlngHeight);
	bufferCanvas->DrawImage(slingshot2, scaledMouseX - (scaleSlngWidth / 2), 
		scaledMouseY - 15, scaleSlngWidth, scaleSlngHeight);

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

	// Play a thud sound when the reptile first hits the ground
	if (deadTicks == 0 && reptileLogic->GetVerticaltalVel() == 0 && reptileLogic->GetBottomOffset() == 0 && !floorHit)
	{
		fmodSystem->playSound(FMOD_CHANNEL_FREE, thudSound, false, 0);
		floorHit = true;
	}

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
		
		// Reset dead ticks and floor hit
		deadTicks = 0;
		floorHit = false;
	}
	else if (reptileLogic->GetHorizontalVel() == 0 && reptileLogic->GetVerticaltalVel() == 0)
	{
		deadTicks++;
	}
}



/*
Name:	Click()
Params: 
int windowX - The x coordinate of the mouse click based on the window size.
int windowY - The y coordinate of the mouse click based on the window size.
CRect* windowDimensions - The dimensions of the window.
Return: void
Description:
	This method executes game logic that heppens at the click of the mouse.
	Sounds play and the reptile is checked for clicks.
*/
void UFRGame::Click(int windowX, int windowY, CRect* windowDimensions)
{
	// Calculate the mouse click in relation to the image resolution
	int x = windowX * (imageWidth / (float) windowDimensions->Width());
	int y = windowY * (imageHeight / (float) windowDimensions->Height());

	fmodSystem->playSound(FMOD_CHANNEL_FREE, shootSound, false, 0);

	// If reptile is clicked, change to falling state
	if (x > reptileLogic->GetLeftOffset() && x < reptileLogic->GetLeftOffset() + scaleRptlWidth &&
		y > imageHeight - (reptileLogic->GetBottomOffset() + scaleRptlHeight) && y < imageHeight - reptileLogic->GetBottomOffset())
	{
		reptileLogic->SetReptileState(REPTILE_STATE_FALLING);
		fmodSystem->playSound(FMOD_CHANNEL_FREE, punchSound, false, 0);
		fmodSystem->playSound(FMOD_CHANNEL_FREE, fallSound, false, 0);
	}

	// FMOD maintanence
	fmodSystem->update();
}