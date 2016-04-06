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

#define REPTILE_SCALE 0.1
#define SPRITES_FILEPATH TEXT("ReptileSprites\\")
#define REPTILE_SPRITE_EXT TEXT(".png")
#define REPTILE_DEAD_SPRITE_PREFIX TEXT("RD")
#define REPTILE_FLYING_SPRITE_PREFIX TEXT("RF")

#define WEIGHT 20
#define FORCE_GIVEN 0.6

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
	minXSpeed = MIN_RAND_X_SPEED;
	maxXSpeed = MAX_RAND_X_SPEED;
	
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

	// Calculate the scaled size of the reptile
	scaledWidth = GetSpriteWidth() * REPTILE_SCALE;
	scaledHeight = GetSpriteHeight() * REPTILE_SCALE;
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
	minXSpeed = MIN_RAND_X_SPEED;
	maxXSpeed = MAX_RAND_X_SPEED;

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

	// Calculate the scaled size of the reptile
	scaledWidth = GetSpriteWidth() * REPTILE_SCALE;
	scaledHeight = GetSpriteHeight() * REPTILE_SCALE;
}



/*
Name:	~UFReptileLogic()
Params: None
Description:
The destructor for the UFReptileLogic class.
Reptile sprites are deallocated here.
*/
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



/*
Name:	SetReptileState()
Params:
int stateCode - The value representing the state to set the reptile to.
Description:
This method sets the state of the reptile.
*/
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



/*
Name:	FlyTick()
Params: None
Description:
This method updates the information of the reptile when it is in flight.
*/
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



/*
Name:	FallTick()
Params: None
Description:
This method updates the information of the reptile when it is falling.
*/
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
This method selects a random number of ticks that must pass before the reptile changes its own x velocity.
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
This method selects a random velocity for the reptile based on a minimum and maximum horizontal speed.
*/
void UFReptileLogic::SetRandHorVel()
{
	// Get random value between -NumberOfAllowedSpeeds and (NumberOfAllowedSpeeds - 1)
	int velBuff = (rand() % ((maxXSpeed - minXSpeed) * 2)) - minXSpeed;

	// Adjust based on going left or right
	if (velBuff >= 0)
	{
		xVelocity = velBuff + minXSpeed;
	}
	else
	{
		xVelocity = velBuff - (minXSpeed - 1);
	}
}



/*
Name:	DetectCollision()
Params: 
Crate* crate - The crate to check the collision of the reptile against.
Return: void
Description:
This method calls the HandleCollision method if it detects that the reptile has collided with a crate.
The collision is detected by checking the relative position of the reptile to the crate and
comparing it to the size of both.
*/
void UFReptileLogic::DetectCollision(Crate* crate)
{
	int reptileHalfWidth = scaledWidth / 2;
	int reptileHalfHeight = scaledHeight / 2;
	int crateHalfWidth = crate->GetWidth() / 2;
	int crateHalfHeight = crate->GetHeight() / 2;
	int deltaXCenters = (xOffset + reptileHalfWidth) - (crate->GetLeftOffset() + crateHalfWidth);
	int deltaYCenters = (yOffset + reptileHalfHeight) - (crate->GetBottomOffset() + crateHalfHeight);

	// If the distance between the centers of the reptile and crate is smaller in magnitude than the distances from the center
	// of each shape to their corresponding edges added together, the objects are in collision
	if (abs(deltaXCenters) <= reptileHalfWidth + crateHalfWidth && 
		abs(deltaYCenters) <= reptileHalfHeight + crateHalfHeight)
	{
		HandleCollision(crate);
	}
}



/*
Name:	HandleCollision()
Params:
Crate* crate - The crate to handle the collision of the reptile.
Return: void
Description:
This method assumes that the reptile and the crate have collided.
The relative positions of the crate and the reptile are calculated as well as the velocities and direction of impact.
Using this information, forces from the impact are passed to the respective objects.
*/
void UFReptileLogic::HandleCollision(Crate* crate)
{
	int forceOfReptile = 0;
	int forceOfCrate = 0;
	int deltaXCenters = (xOffset + scaledWidth / 2) - (crate->GetLeftOffset() + crate->GetWidth() / 2);
	int deltaYCenters = (yOffset + scaledHeight / 2) - (crate->GetBottomOffset() + crate->GetHeight() / 2);

	if (abs(deltaXCenters) >= abs(deltaYCenters)) // Collision happened from the side
	{
		if (deltaXCenters > 0) // The reptile is to the right and the crate is to the left
		{
			xOffset = crate->GetLeftOffset() + crate->GetWidth();

			// If the reptile was moving towards the crate
			if (xVelocity < 0)
			{
				// Calculate the collision force of the reptile
				forceOfReptile = CalcHorizontalForce();
			}

			// If the crate was moving towards the reptile
			if (crate->GetHorizontalVel() > 0)
			{
				// Calculate the collision force of the crate
				forceOfCrate = crate->CalcHorizontalForce();
			}

			// The reptile gives up its force and gives it to the crate
			ApplyHorizontalForce(-forceOfReptile);
			crate->ApplyHorizontalForce(forceOfReptile);

			// The crate gives up its force and gives it to the reptile
			crate->ApplyHorizontalForce(-forceOfCrate);
			ApplyHorizontalForce(forceOfCrate);
		}
		else // The reptile is to the left and the crate is to the right
		{
			xOffset = crate->GetLeftOffset() - scaledWidth;

			// If the reptile was moving towards the crate
			if (xVelocity > 0)
			{
				// Calculate the collision force of the reptile
				forceOfReptile = CalcHorizontalForce();
			}

			// If the crate was moving towards the reptile
			if (crate->GetHorizontalVel() < 0)
			{
				// Calculate the collision force of the crate
				forceOfCrate = crate->CalcHorizontalForce();
			}

			// The reptile gives up its force and gives it to the crate
			ApplyHorizontalForce(-forceOfReptile);
			crate->ApplyHorizontalForce(forceOfReptile);

			// The crate gives up its force and gives it to the reptile
			crate->ApplyHorizontalForce(-forceOfCrate);
			ApplyHorizontalForce(forceOfCrate);
		}
	}
	else // The collision happened from the top or bottom
	{
		if (deltaYCenters > 0) // The reptile is above the crate
		{
			yOffset = crate->GetBottomOffset() + crate->GetHeight();

			// If the reptile was moving towards the crate
			if (yVelocity < 0)
			{
				// Calculate the collision force of the reptile
				forceOfReptile = CalcVerticalForce();
			}

			// If the crate was moving towards the reptile
			if (crate->GetVerticalVel() > 0)
			{
				// Calculate the collision force of the crate
				forceOfCrate = crate->CalcVerticalForce();
			}

			// The reptile gives up its force and gives it to the crate
			ApplyVerticalForce(-forceOfReptile);
			crate->ApplyVerticalForce(forceOfReptile);

			// The crate gives up its force and gives it to the reptile
			crate->ApplyVerticalForce(-forceOfCrate);
			ApplyVerticalForce(forceOfCrate);
		}
		else // The reptile is bellow the other crate
		{
			crate->SetBottomOffset(yOffset + scaledHeight);

			// If the reptile was moving towards the crate
			if (yVelocity > 0)
			{
				// Calculate the collision force of the reptile
				forceOfReptile = CalcVerticalForce();
			}

			// If the crate was moving towards the reptile
			if (crate->GetVerticalVel() < 0)
			{
				// Calculate the collision force of the crate
				forceOfCrate = crate->CalcVerticalForce();
			}

			// The reptile gives up its force and gives it to the crate
			ApplyVerticalForce(-forceOfReptile);
			crate->ApplyVerticalForce(forceOfReptile);

			// The crate gives up its force and gives it to the reptile
			crate->ApplyVerticalForce(-forceOfCrate);
			ApplyVerticalForce(forceOfCrate);
		}
	}
}



/*
Name:	ApplyHorizontalForce()
Params:
int force - The amount and directionality of force to be applied. Positive converts into motion to the right, negative to the left.
Return: void
Description:
This method takes horizontal force applied to the reptile and converts it into horizontal velocity based on the weight of the reptile.
*/
void UFReptileLogic::ApplyHorizontalForce(int force)
{
	xVelocity += force / WEIGHT;
}



/*
Name:	ApplyVerticalForce()
Params:
int force - The amount and directionality of force to be applied. Positive converts into upward motion, negative to downward.
Return: void
Description:
This method takes vertical force applied to the reptile and converts it into vertical velocity based on the weight of the reptile.
*/
void UFReptileLogic::ApplyVerticalForce(int force)
{
	yVelocity += force / WEIGHT;
}



/*
Name:	CalcHorizontalForce()
Params: None
Return: int - The amount of force to give.
Description:
This method calculates the amount of force that the reptile can give up on horizontal impact.
The calculation is based on the current horizontal velocity, the weight of the reptile and the percentage of its force
the reptile gives up on impact.
*/
int UFReptileLogic::CalcHorizontalForce()
{
	return (xVelocity * WEIGHT) * (1 - FORCE_GIVEN);
}



/*
Name:	CalcVerticalForce()
Params: None
Return: int - The amount of force to give.
Description:
This method calculates the amount of force that the reptile can give up on vertical impact.
The calculation is based on the current vertical velocity, the weight of the reptile and the percentage of its force
the reptile gives up on impact.
*/
int UFReptileLogic::CalcVerticalForce()
{
	return (yVelocity * WEIGHT) * (1 - FORCE_GIVEN);
}



/*
Name:	SetMinHorSpeed()
Params: unsigned int horizontalSpeed - The minimum horizontal speed.
Return: void
Description:
This method sets the minimum speed that the reptile can travel at in the air.
*/
void UFReptileLogic::SetMinHorSpeed(unsigned int horizontalSpeed)
{
	if (horizontalSpeed < maxXSpeed)
	{
		minXSpeed = horizontalSpeed;
	}
}



/*
Name:	SetMaxHorSpeed()
Params: unsigned int horizontalSpeed - The maximum horizontal speed.
Return: void
Description:
This method sets the maximum speed that the reptile can travel at in the air.
*/
void UFReptileLogic::SetMaxHorSpeed(unsigned int horizontalSpeed)
{
	if (horizontalSpeed > minXSpeed)
	{
		maxXSpeed = horizontalSpeed;
	}
}