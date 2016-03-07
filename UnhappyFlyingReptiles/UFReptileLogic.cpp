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

#define MAX_TICKS_BETWEEN_FLAPS 6
#define MIN_TICKS_BETWEEN_FLAPS 3
#define MAX_FLAP_STRENGTH 8
#define MIN_FLAP_STRENGTH 4
#define DEFAULT_MAX_FLIGHT_THRESHOLD 300
#define DEFAULT_MIN_FLIGHT_THRESHOLD 120

#define ROTATION_DEGREES 360
#define DEATH_SPIN_DEGREES 5

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
	xOffset = leftOffset;
	yOffset = bottomOffset;
	xVelocity = DEFAULT_X_VELOCITY;
	yVelocity = DEFAULT_Y_VELOCITY;
	friction = DEFAULT_FRICTION;
	gravity = DEFAULT_GRAVITY;
	minFlightThreshold = DEFAULT_MIN_FLIGHT_THRESHOLD;
	maxFlightThreshold = DEFAULT_MAX_FLIGHT_THRESHOLD;
	
	reptileState = REPTILE_STATE_FLYING;
	srand(NULL);
	ticksToNextFlap = CalcTicksToNextFlap();

	reptileRotation = 0;
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
	xOffset = leftOffset;
	yOffset = bottomOffset;
	xVelocity = horizontalVelocity;
	yVelocity = verticalVelocity;
	friction = DEFAULT_FRICTION;
	gravity = DEFAULT_GRAVITY;
	minFlightThreshold = DEFAULT_MIN_FLIGHT_THRESHOLD;
	maxFlightThreshold = DEFAULT_MAX_FLIGHT_THRESHOLD;

	reptileState = REPTILE_STATE_FLYING;
	srand(NULL);
	ticksToNextFlap = CalcTicksToNextFlap();

	reptileRotation = 0;
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
	}
	else if (reptileState == REPTILE_STATE_FLYING)
	{
		FlyTick();
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


	// Apply gravity to vertival velocity if reptile is off the ground
	if (yOffset != 0)
	{
		yVelocity -= gravity;
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