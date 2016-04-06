#pragma once

#include <stdlib.h>
#include "afxwin.h"
#include <gdiplus.h>

using namespace Gdiplus;

class Crate
{
private:
	int xOffset; // The offset from the left of the screen
	int yOffset; // The offset from the bottom of the screen

	int xVelocity; // The velocity in the x-axis. Positive values go to the right, negative to the left.
	int yVelocity; // The velocity in the y-axis. Positive values go towards the top of the screen, negative to the bottom.

	unsigned int friction; // The rate at which the x velocity tends towards 0.
	unsigned int gravity; // The rate at which the y velocity decreases until the y offset is 0.

	int crateWeight;
	float crateForceGiven;
	float crateScale;

	int scaledHeight;
	int scaledWidth;

	Bitmap* sprite;

public:
	Crate(int leftOffset, int bottomOffset);
	Crate(int leftOffset, int bottomOffset, unsigned int weight, float forceGiven, float scale);
	~Crate();

	int GetLeftOffset() { return xOffset; }
	void SetLeftOffset(int offset) { xOffset = offset; }
	int GetBottomOffset() { return yOffset; }
	void SetBottomOffset(unsigned int offset) { yOffset = offset; }

	int GetHeight() { return scaledHeight; }
	int GetWidth() { return scaledWidth; }

	void Tick();

	int GetHorizontalVel() { return xVelocity; }
	int GetVerticalVel() { return yVelocity; }
	void SetHorizontalVel(int horizontalVel) { xVelocity = horizontalVel; }
	void SetVerticalVel(int verticalVel) { yVelocity = verticalVel; }

	Bitmap* GetSprite() { return sprite; }
	int GetSpriteHeight() { return sprite->GetHeight(); }
	int GetSpriteWidth() { return sprite->GetWidth(); }

	void DetectCollision(Crate* otherCrate);
	void HandleCollision(Crate* otherCrate);

	void ApplyHorizontalForce(int force);
	void ApplyVerticalForce(int force);
	int CalcHorizontalForce();
	int CalcVerticalForce();
};

