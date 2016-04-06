#include "Crate.h"

#define DEFAULT_CRATE_SCALE 0.4
#define SPRITE_FILEPATH TEXT("Crates\\")
#define CRATE_SPRITE TEXT("Crate.png")
#define HEAVY_CRATE_SPRITE TEXT("HeavyCrate.png")
#define LIGHT_CRATE_SPRITE TEXT("LightCrate.png")

#define DEFAULT_X_OFFSET 5
#define DEFAULT_Y_OFFSET 5
#define DEFAULT_X_VELOCITY 0
#define DEFAULT_Y_VELOCITY 0
#define DEFAULT_FRICTION 1
#define DEFAULT_GRAVITY 1
#define COLL_ADJUST_ACCEL 1

#define HEAVY_CRATE_THRESHOLD 12
#define LIGHT_CRATE_THRESHOLD 3
#define DEFAULT_WEIGHT 5
#define DEFAULT_FORCE_GIVEN 0.7


/*
Name:	Crate()
Params:
int leftOffset - The initial xOffset for the crate.
int bottomOffset - The initial yOffset for the crate.
Description:
The constructor for the Crate class.
The base movement information is set here.
*/
Crate::Crate(int leftOffset, int bottomOffset)
{
	// Load sprite
	wchar_t buff[355] = { '\0' };
	swprintf(buff, TEXT("%s%s"), SPRITE_FILEPATH, CRATE_SPRITE);
	sprite = new Bitmap(buff);

	// Set initial physics values
	xOffset = leftOffset;
	yOffset = bottomOffset;
	xVelocity = DEFAULT_X_VELOCITY;
	yVelocity = DEFAULT_Y_VELOCITY;
	friction = DEFAULT_FRICTION;
	gravity = DEFAULT_GRAVITY;
	crateWeight = DEFAULT_WEIGHT;
	crateForceGiven = DEFAULT_FORCE_GIVEN;
	crateScale = DEFAULT_CRATE_SCALE;

	scaledWidth = sprite->GetWidth() * crateScale;
	scaledHeight = sprite->GetHeight() * crateScale;
}



/*
Name:	Crate()
Params:
int leftOffset - The initial xOffset for the crate.
int bottomOffset - The initial yOffset for the crate.
unsigned int weight - The weight of the crate.
float forceGiven - The force that the crate gives up on impact.
float scale - The factor by which to scale the crate.
Description:
The constructor for the Crate class.
The base movement information is set here.
*/
Crate::Crate(int leftOffset, int bottomOffset, unsigned int weight, float forceGiven, float scale)
{
	// Load sprite
	wchar_t buff[355] = { '\0' };
	if (weight >= HEAVY_CRATE_THRESHOLD)
	{
		swprintf(buff, TEXT("%s%s"), SPRITE_FILEPATH, HEAVY_CRATE_SPRITE);
	}
	else if (weight <= LIGHT_CRATE_THRESHOLD)
	{
		swprintf(buff, TEXT("%s%s"), SPRITE_FILEPATH, LIGHT_CRATE_SPRITE);
	}
	else
	{
		swprintf(buff, TEXT("%s%s"), SPRITE_FILEPATH, CRATE_SPRITE);
	}
	sprite = new Bitmap(buff);

	// Set initial physics values
	xOffset = leftOffset;
	yOffset = bottomOffset;
	xVelocity = DEFAULT_X_VELOCITY;
	yVelocity = DEFAULT_Y_VELOCITY;
	friction = DEFAULT_FRICTION;
	gravity = DEFAULT_GRAVITY;
	crateWeight = weight;
	if (forceGiven < 0.0)
	{
		crateForceGiven = -forceGiven;
	}
	else
	{
		crateForceGiven = forceGiven;
	}
	if (scale < 0)
	{
		crateScale = -scale;
	}
	else
	{
		crateScale = scale;
	}

	scaledWidth = sprite->GetWidth() * crateScale;
	scaledHeight = sprite->GetHeight() * crateScale;
}



/*
Name:	~Crate()
Params: None
Description:
The destructor for the Crate class.
The crate sprite is deallocated here.
*/
Crate::~Crate()
{
	// Deallocate the sprite
	delete sprite;
}



/*
Name:	Tick()
Params: void
Return: void
Description:
This method calculates the changes in offset and velocity of the crate
after one interval of time.
*/
void Crate::Tick()
{
	// Calculate movement
	xOffset += xVelocity;
	yOffset += yVelocity;

	// The crate can't ever be below 0 height
	if (yOffset < 0)
	{
		yOffset = 0;
	}

	// crate horizontal friction
	if (xVelocity > 0)
	{
		xVelocity -= friction;
	}
	else if (xVelocity < 0)
	{
		xVelocity += friction;
	}

	// Apply gravity to vertival velocity if the crate is off the ground
	if (yOffset != 0)
	{
		yVelocity -= gravity;
	}
}



/*
Name:	DetectCollision()
Params:
Crate* otherCrate - The other crate to check the collision of the crate against.
Return: void
Description:
This method calls the HandleCollision method if it detects that the crate has collided with another crate.
The collision is detected by checking the relative position of the crates and
comparing it to their size.
*/
void Crate::DetectCollision(Crate* otherCrate)
{
	int crateHalfWidth = scaledWidth / 2;
	int crateHalfHeight = scaledHeight / 2;
	int otherCrateHalfWidth = otherCrate->GetWidth() / 2;
	int otherCrateHalfHeight = otherCrate->GetHeight() / 2;
	int deltaXCenters = (xOffset + crateHalfWidth) - (otherCrate->GetLeftOffset() + otherCrateHalfWidth);
	int deltaYCenters = (yOffset + crateHalfHeight) - (otherCrate->GetBottomOffset() + otherCrateHalfHeight);

	// If the distance between the centers of the two crates is smaller in magnitude than the distances from the center
	// of each crate to their corresponding edges added together, the objects are in collision
	if (abs(deltaXCenters) <= crateHalfWidth + otherCrateHalfWidth &&
		abs(deltaYCenters) <= crateHalfHeight + otherCrateHalfHeight)
	{
		HandleCollision(otherCrate);
	}
}



/*
Name:	HandleCollision()
Params:
Crate* otherCrate - The other crate to handle the collision of the crate.
Return: void
Description:
This method assumes that the two crates have collided.
The relative positions of the crates are calculated as well as their velocities and direction of impact.
Using this information, forces from the impact are passed to the respective crates.
*/
void Crate::HandleCollision(Crate* otherCrate)
{
	int forceOfCrate = 0;
	int forceOfOtherCrate = 0;
	int deltaXCenters = (xOffset + scaledWidth / 2) - (otherCrate->GetLeftOffset() + otherCrate->GetWidth() / 2);
	int deltaYCenters = (yOffset + scaledHeight / 2) - (otherCrate->GetBottomOffset() + otherCrate->GetHeight() / 2);

	if (abs(deltaXCenters) >= abs(deltaYCenters)) // Collision happened from the side
	{
		if (deltaXCenters > 0) // This crate is to the right and the other crate is to the left
		{
			xOffset = otherCrate->GetLeftOffset() + otherCrate->GetWidth();

			// If this crate was moving towards the other crate
			if (xVelocity < 0)
			{
				// Calculate the collision force of this crate
				forceOfCrate = CalcHorizontalForce();
			}

			// If the other crate was moving towards this crate
			if (otherCrate->GetHorizontalVel() > 0)
			{
				// Calculate the collision force of the other crate
				forceOfOtherCrate = otherCrate->CalcHorizontalForce();
			}

			// This crate gives up its force and gives it to the other crate
			ApplyHorizontalForce(-forceOfCrate);
			otherCrate->ApplyHorizontalForce(forceOfCrate);

			// The other crate gives up its force and gives it to this crate
			otherCrate->ApplyHorizontalForce(-forceOfOtherCrate);
			ApplyHorizontalForce(forceOfOtherCrate);
		}
		else // This crate is to the left and the other crate is to the right
		{
			xOffset = otherCrate->GetLeftOffset() - scaledWidth;

			// If this crate was moving towards the other crate
			if (xVelocity > 0)
			{
				// Calculate the collision force of this crate
				forceOfCrate = CalcHorizontalForce();
			}

			// If the other crate was moving towards this crate
			if (otherCrate->GetHorizontalVel() < 0)
			{
				// Calculate the collision force of the other crate
				forceOfOtherCrate = otherCrate->CalcHorizontalForce();
			}

			// This crate gives up its force and gives it to the other crate
			ApplyHorizontalForce(-forceOfCrate);
			otherCrate->ApplyHorizontalForce(forceOfCrate);

			// The other crate gives up its force and gives it to this crate
			otherCrate->ApplyHorizontalForce(-forceOfOtherCrate);
			ApplyHorizontalForce(forceOfOtherCrate);
		}
	}
	else // The collision happened from the top or bottom
	{
		if (deltaYCenters > 0) // This crate is above the other crate
		{
			yOffset = otherCrate->GetBottomOffset() + otherCrate->GetHeight();

			// If this crate was moving towards the other crate
			if (yVelocity < 0)
			{
				// Calculate the collision force of this crate
				forceOfCrate = CalcVerticalForce();
			}

			// If the other crate was moving towards this crate
			if (otherCrate->GetVerticalVel() > 0)
			{
				// Calculate the collision force of the other crate
				forceOfOtherCrate = otherCrate->CalcVerticalForce();
			}

			// This crate gives up its force and gives it to the other crate
			ApplyVerticalForce(-forceOfCrate);
			otherCrate->ApplyVerticalForce(forceOfCrate);

			// The other crate gives up its force and gives it to this crate
			otherCrate->ApplyVerticalForce(-forceOfOtherCrate);
			ApplyVerticalForce(forceOfOtherCrate);
		}
		else // This crate is bellow the other crate
		{
			otherCrate->SetBottomOffset(yOffset + scaledHeight);

			// If this crate was moving towards the other crate
			if (yVelocity > 0)
			{
				// Calculate the collision force of this crate
				forceOfCrate = CalcVerticalForce();
			}

			// If the other crate was moving towards this crate
			if (otherCrate->GetVerticalVel() < 0)
			{
				// Calculate the collision force of the other crate
				forceOfOtherCrate = otherCrate->CalcVerticalForce();
			}

			// This crate gives up its force and gives it to the other crate
			ApplyVerticalForce(-forceOfCrate);
			otherCrate->ApplyVerticalForce(forceOfCrate);

			// The other crate gives up its force and gives it to this crate
			otherCrate->ApplyVerticalForce(-forceOfOtherCrate);
			ApplyVerticalForce(forceOfOtherCrate);
		}
	}
}



/*
Name:	ApplyHorizontalForce()
Params:
int force - The amount and directionality of force to be applied. Positive converts into motion to the right, negative to the left.
Return: void
Description:
This method takes horizontal force applied to the crate and converts it into horizontal velocity based on the weight of the crate.
*/
void Crate::ApplyHorizontalForce(int force)
{
	xVelocity += force / crateWeight;
}



/*
Name:	ApplyVerticalForce()
Params:
int force - The amount and directionality of force to be applied. Positive converts into upward motion, negative to downward.
Return: void
Description:
This method takes vertical force applied to the crate and converts it into vertical velocity based on the weight of the crate.
*/
void Crate::ApplyVerticalForce(int force)
{
	yVelocity += force / crateWeight;
}



/*
Name:	CalcHorizontalForce()
Params: None
Return: int - The amount of force to give.
Description:
This method calculates the amount of force that the crate can give up on horizontal impact.
The calculation is based on the current horizontal velocity, the weight of the crate and the percentage of its force
the crate gives up on impact.
*/
int Crate::CalcHorizontalForce()
{
	return (xVelocity * crateWeight) * (1 - crateForceGiven);
}



/*
Name:	CalcVerticalForce()
Params: None
Return: int - The amount of force to give.
Description:
This method calculates the amount of force that the crate can give up on vertical impact.
The calculation is based on the current vertical velocity, the weight of the crate and the percentage of its force
the crate gives up on impact.
*/
int Crate::CalcVerticalForce()
{
	return (yVelocity * crateWeight) * (1 - crateForceGiven);
}