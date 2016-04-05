/*
File:		UFReptileLogic.cpp
Project:	Unhappy Flying Reptiles
Author(s):	Jorge Ramirez
Description:
	This file contains the method definitions for the UFReptileLogic class.
*/

#include "UFReptileLogic.h"

#define DEFAULT_X_OFFSET 5
#define DEFAULT_Y_OFFSET 5
#define DEFAULT_X_VELOCITY 7
#define DEFAULT_Y_VELOCITY 10
#define DEFAULT_FRICTION 1
#define DEFAULT_GRAVITY 1

#define MAX_TICKS_BETWEEN_FLAPS 10
#define MIN_TICKS_BETWEEN_FLAPS 5
#define MAX_FLAP_STRENGTH 12
#define MIN_FLAP_STRENGTH 6
#define MAX_TICKS_BETWEEN_XVEL 20
#define MIN_TICKS_BETWEEN_XVEL 10
#define MAX_RAND_X_SPEED 16
#define MIN_RAND_X_SPEED 8
#define DEFAULT_MAX_FLIGHT_THRESHOLD 300
#define DEFAULT_MIN_FLIGHT_THRESHOLD 120

#define ROTATION_DEGREES 360
#define DEATH_SPIN_DEGREES 5

#define SPRITES_FILEPATH TEXT("ReptileSprites\\")
#define REPTILE_SPRITE_EXT TEXT(".png")
#define REPTILE_DEAD_SPRITE_PREFIX TEXT("RD")
#define REPTILE_FLYING_SPRITE_PREFIX TEXT("RF")

/*
Name:	UFReptileLogic()
Params:
	int leftOffset - The initial xOffset for the reptile.
	int bottomOffset - The initial yOffset for the reptile.
Description:
	The constructor for the UFReptileLogic class.
	The base movement information is set here.
*/
UFReptileLogic::UFReptileLogic(int leftOffset, int bottomOffset)
{
	wchar_t buff[256] = { '\0' };

	xOffset = leftOffset;
	yOffset = bottomOffset;
	xVelocity = DEFAULT_X_VELOCITY;
	yVelocity = DEFAULT_Y_VELOCITY;
	friction = DEFAULT_FRICTION;
	gravity = DEFAULT_GRAVITY;
	minFlightThreshold = DEFAULT_MIN_FLIGHT_THRESHOLD;
	maxFlightThreshold = DEFAULT_MAX_FLIGHT_THRESHOLD;
	
	reptileState = REPTILE_STATE_FLYING;
	wasFlying = true;
	srand(time(NULL));
	ticksToNextFlap = CalcTicksToNextFlap();
	ticksToNextXVel = CalcTicksToNextXVel();

	reptileRotation = 0;
	if (xVelocity >= 0)
	{
		flyingLeft = false;
	}
	else
	{
		flyingLeft = true;
	}

	// Load reptile sprites
	for (int sprite = 0; sprite < REPTILE_FLYING_SPRITE_COUNT; sprite++)
	{
		swprintf(buff, TEXT("%s%s%d%s"), SPRITES_FILEPATH, REPTILE_FLYING_SPRITE_PREFIX, sprite, REPTILE_SPRITE_EXT);
		flyingSprites.push_back(new Bitmap(buff));
	}
	swprintf(buff, TEXT("%s%s%s"), SPRITES_FILEPATH, REPTILE_DEAD_SPRITE_PREFIX, REPTILE_SPRITE_EXT);
	deadSprite = new Bitmap(buff);

	// Set selected sprite
	flyingSpriteIndex = 0;
	selectedSprite = flyingSprites[flyingSpriteIndex];
}

/*
Name:	UFReptileLogic()
Params:
int leftOffset - The initial xOffset for the reptile.
int bottomOffset - The initial yOffset for the reptile.
int horizontalVelocity - The initial horizontal velocity for the reptile.
int verticalVelocity - The initial vertical velocity for the reptile.
Description:
The constructor for the UFReptileLogic class.
The base movement information is set here.
*/
UFReptileLogic::UFReptileLogic(int leftOffset, int bottomOffset, int horizontalVelocity, int verticalVelocity)
{
	wchar_t buff[256] = { '\0' };
	int buffer = 0;

	xOffset = leftOffset;
	yOffset = bottomOffset;
	xVelocity = horizontalVelocity;
	yVelocity = verticalVelocity;
	friction = DEFAULT_FRICTION;
	gravity = DEFAULT_GRAVITY;
	minFlightThreshold = DEFAULT_MIN_FLIGHT_THRESHOLD;
	maxFlightThreshold = DEFAULT_MAX_FLIGHT_THRESHOLD;

	reptileState = REPTILE_STATE_FLYING;
	wasFlying = true;
	srand(time(NULL));
	ticksToNextFlap = CalcTicksToNextFlap();
	ticksToNextXVel = CalcTicksToNextXVel();

	reptileRotation = 0;
	if (xVelocity >= 0)
	{
		flyingLeft = false;
	}
	else
	{
		flyingLeft = true;
	}

	// Load reptile sprites
	for (int sprite = 0; sprite < REPTILE_FLYING_SPRITE_COUNT; sprite++)
	{
		swprintf(buff, TEXT("%s%s%d%s"), SPRITES_FILEPATH, REPTILE_FLYING_SPRITE_PREFIX, sprite, REPTILE_SPRITE_EXT);
		flyingSprites.push_back(new Bitmap(buff));
	}
	swprintf(buff, TEXT("%s%s%s"), SPRITES_FILEPATH, REPTILE_DEAD_SPRITE_PREFIX, REPTILE_SPRITE_EXT);
	deadSprite = new Bitmap(buff);

	// Set selected sprite
	flyingSpriteIndex = 0;
	selectedSprite = flyingSprites[flyingSpriteIndex];
	buffer = selectedSprite->GetHeight();
}

UFReptileLogic::~UFReptileLogic()
{
	// Deallocate reptile sprites
	for (int sprite = 0; sprite < flyingSprites.size(); sprite++)
	{
		delete flyingSprites[sprite];
	}
	delete deadSprite;
}


/*
Name:	Tick()
Params: void
Return: void
Description:
	This method calculates the changes in offset and velocity of the reptile
	after one interval of time.
*/
void UFReptileLogic::Tick()
{
	if (reptileState == REPTILE_STATE_FALLING)
	{
		FallTick();
		wasFlying = false;
	}
	else if (reptileState == REPTILE_STATE_FLYING)
	{
		FlyTick();
		wasFlying = true;
	}
}


void UFReptileLogic::SetReptileState(int stateCode)
{
	switch (stateCode)
	{
	case REPTILE_STATE_FLYING:
		reptileState = REPTILE_STATE_FLYING;
		reptileRotation = 0;
		break;
	case REPTILE_STATE_FALLING:
		reptileState = REPTILE_STATE_FALLING;
		break;
	default:
		break;
	}
}



void UFReptileLogic::FlyTick()
{
	// Calculate movement
	xOffset += xVelocity;
	yOffset += yVelocity;

	// Reptile can't ever be below 0 height
	if (yOffset < 0)
	{
		yOffset = 0;
	}

	// If Reptile is on the ground, stop the vertical movement and flap wings
	if (yOffset == 0)
	{
		yVelocity = 0;

		// Flap wings and set ticks until next flap
		FlapWings();
		ticksToNextFlap = CalcTicksToNextFlap();
	}

	// If reptile is bellow the min flight threshold, or if it is time for the next flap, and the reptile is below the max
	// flight threshold, flap.
	// Else, continue counting down to next flap.
	if ((ticksToNextFlap <= 0 || yOffset < minFlightThreshold) && yOffset < maxFlightThreshold)
	{
		// Flap wings and set ticks until next flap
		FlapWings();
		ticksToNextFlap = CalcTicksToNextFlap();
	}
	else
	{
		ticksToNextFlap--;
	}

	// If it's time to change xVelocity, change it.
	// Else, continue counting down to next xVelocity change.
	if (ticksToNextXVel <= 0)
	{
		SetRandHorVel();
		ticksToNextXVel = CalcTicksToNextXVel();
	}
	else
	{
		ticksToNextXVel--;
	}

	// Apply gravity to vertival velocity if reptile is off the ground
	if (yOffset != 0)
	{
		yVelocity -= gravity;
	}

	// Update sprite to select
	flyingSpriteIndex++;
	if (flyingSpriteIndex == flyingSprites.size())
	{
		flyingSpriteIndex = 0;
	}

	// Revert flipped sprite if it was flying left previously
	if (flyingLeft)
	{
		selectedSprite->RotateFlip(RotateNoneFlipX);
	}

	// Select new sprite
	selectedSprite = flyingSprites[flyingSpriteIndex];

	// Update flight direction
	if (xVelocity >= 0)
	{
		flyingLeft = false;
	}
	else
	{
		flyingLeft = true;
	}

	// Flip sprite if its flying left this time
	if (flyingLeft)
	{
		selectedSprite->RotateFlip(RotateNoneFlipX);
	}
}

void UFReptileLogic::FallTick()
{
	// Calculate movement
	xOffset += xVelocity;
	yOffset += yVelocity;

	// Calculate rotation
	if (xVelocity > 0)
	{
		RotateClockwise(DEATH_SPIN_DEGREES);
	}
	else if (xVelocity < 0)
	{
		RotateCounterClockwise(DEATH_SPIN_DEGREES);
	}

	// Reptile can't ever be below 0 height
	if (yOffset < 0)
	{
		yOffset = 0;
	}

	// If Reptile is on the ground, stop the vertical movement and apply friction
	if (yOffset == 0)
	{
		yVelocity = 0;

		if (friction > abs(xVelocity) && abs(xVelocity) > 0)
		{
			// If friction would stop the xVelocity, set the xVelocity to 0
			xVelocity = 0;
		}
		else if (xVelocity > 0)
		{
			// If the xVelocity is positive, subtract the friction from it.
			xVelocity -= friction;
		}
		else if (xVelocity < 0)
		{
			// If the xVelocity is negative, add the friction to it.
			xVelocity += friction;
		}
	}

	// Apply gravity to vertival velocity if reptile is off the ground
	if (yOffset != 0)
	{
		yVelocity -= gravity;
	}

	// If the reptile was flying left just before this,
	if (wasFlying && flyingLeft)
	{
		// Reset the flip of the previous image
		selectedSprite->RotateFlip(RotateNoneFlipX);
	}

	// Set dead sprite
	selectedSprite = deadSprite;

	// If the reptile was flying left just before this,
	if (wasFlying && flyingLeft)
	{
		// Set the falling sprite to face left
		selectedSprite->RotateFlip(RotateNoneFlipX);
	}
}



/*
Name:	FlapWings()
Params: None
Return: void
Description:
This method adds a positive value to the yVelocity based on the strength of the reptile's falp.
The flap strength is a random number within a range.
*/
void UFReptileLogic::FlapWings()
{
	yVelocity += (rand() % (MAX_FLAP_STRENGTH - MIN_FLAP_STRENGTH)) + MIN_FLAP_STRENGTH;
}



/*
Name:	CalcTicksToNextFlap()
Params: None
Return: void
Description:
This method generates a random number of ticks (within a range) that represent the number of ticks it will take for the reptile
to flap its wings again.
*/
int UFReptileLogic::CalcTicksToNextFlap()
{
	return (rand() % (MAX_TICKS_BETWEEN_FLAPS - MIN_TICKS_BETWEEN_FLAPS)) + MIN_TICKS_BETWEEN_FLAPS;
}



/*
Name:	SetOffsetAndVelocity()
Params:
	int leftOffset - The new xOffset for the reptile.
	int bottomOffset - The new yOffset for the reptile.
	int horizontalVelocity - The new xVelocity for the reptile.
	int verticalVelocity - The new yVelocity for the reptile.
Return: void
Description:
	This is a setter for multiple attributes of the reptile at once.
*/
void UFReptileLogic::SetOffsetAndVelocity(int leftOffset, int bottomOffset, int horizontalVelocity, int verticalVelocity)
{
	xOffset = leftOffset;
	yOffset = bottomOffset;
	xVelocity = horizontalVelocity;
	yVelocity = verticalVelocity;
}



/*
Name:	RotateClockwise()
Params: 
int degrees - The number of degrees to rotate clockwise.
Return: void
Description:
This method updates the reptile's current rotation angle based on the passed in number of degrees.
The degrees passed in represent the number of degrees that the reptile rotates clockwise.
*/
void UFReptileLogic::RotateClockwise(int degrees)
{
	reptileRotation = (reptileRotation + degrees) % ROTATION_DEGREES;
}



/*
Name:	RotateCounterClockwise()
Params:
int degrees - The number of degrees to rotate counter clockwise.
Return: void
Description:
This method updates the reptile's current rotation angle based on the passed in number of degrees.
The degrees passed in represent the number of degrees that the reptile rotates counter clockwise.
*/
void UFReptileLogic::RotateCounterClockwise(int degrees)
{
	RotateClockwise(-degrees);
}



/*
Name:	CalcTicksToNextXVel()
Params: None
Return: void
Description:

*/
int UFReptileLogic::CalcTicksToNextXVel()
{
	return (rand() % (MAX_TICKS_BETWEEN_XVEL - MIN_TICKS_BETWEEN_XVEL)) + MIN_TICKS_BETWEEN_XVEL;
}



/*
Name:	SetRandHorVel()
Params: None
Return: void
Description:

*/
void UFReptileLogic::SetRandHorVel()
{
	// Get random value between -NumberOfAllowedSpeeds and (NumberOfAllowedSpeeds - 1)
	int velBuff = (rand() % ((MAX_RAND_X_SPEED - MIN_RAND_X_SPEED) * 2)) - MIN_RAND_X_SPEED;

	if (velBuff >= 0)
	{
		xVelocity = velBuff + MIN_RAND_X_SPEED;
	}
	else
	{
		xVelocity = velBuff - (MIN_RAND_X_SPEED - 1);
	}
}