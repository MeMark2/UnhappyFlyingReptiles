/*
File:		UFReptileLogic.h
Project:	Unhappy Flying Reptiles
Author(s):	Jorge Ramirez
Description:
	This header file contains the class definition for the UFReptileLogic class.
*/


#pragma once

#include <stdlib.h>
#include <time.h>

#define REPTILE_STATE_FALLING 0
#define REPTILE_STATE_FLYING 1


/*
Name: UFReptileLogic
Description:
	This class is designed to model the movement logic behind the flying reptile.
*/
class UFReptileLogic
{
private:
	int xOffset; // The offset from the left of the screen
	int yOffset; // The offset from the bottom of the screen

	int xVelocity; // The velocity in the x-axis. Positive values go to the right, negative to the left.
	int yVelocity; // The velocity in the y-axis. Positive values go towards the top of the screen, negative to the bottom.

	unsigned int friction; // The rate at which the x velocity tends towards 0.
	unsigned int gravity; // The rate at which the y velocity decreases until the y offset is 0.

	unsigned int minFlightThreshold;
	unsigned int maxFlightThreshold;

	int reptileState; 

	void FlyTick();
	void FallTick();

	void FlapWings();
	static int CalcTicksToNextFlap();
	int ticksToNextFlap;

	int reptileRotation;

	void RotateClockwise(int degrees);
	void RotateCounterClockwise(int degrees);

	static int CalcTicksToNextXVel();
	int ticksToNextXVel;
public:
	UFReptileLogic(int leftOffset, int bottomOffset);
	UFReptileLogic(int leftOffset, int bottomOffset, int horizontalVelocity, int verticalVelocity);

	int GetLeftOffset() { return xOffset; }
	void SetLeftOffset(int offset) { xOffset = offset; }
	int GetBottomOffset() { return yOffset; }
	void SetBottomOffset(unsigned int offset) { yOffset = offset; }

	int GetHorizontalVel() { return xVelocity; }
	int GetVerticaltalVel() { return yVelocity; }

	int GetReptileRotation() { return reptileRotation; }

	int GetReptileState() { return reptileState;  }
	void SetReptileState(int stateCode);

	void Tick();

	void SetOffsetAndVelocity(int leftOffset, int bottomOffset, int horizontalVelocity, int verticalVelocity);
	
	void SetRandHorVel();
};

