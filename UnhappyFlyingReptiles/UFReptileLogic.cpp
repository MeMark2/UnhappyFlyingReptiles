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
#define DEFAULT_X_VELOCITY 10
#define DEFAULT_Y_VELOCITY 25
#define DEFAULT_FRICTION 1
#define DEFAULT_GRAVITY 1


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
	// Calculate movement
	xOffset += xVelocity;
	yOffset += yVelocity;

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