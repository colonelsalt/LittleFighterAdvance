#pragma once

struct animation
{
	u32* Frames;
	u32 Length;
	u32 FrameDelay;
	b32 ShouldLoop;
};

const static fixed WALK_SPEED = 1;

struct entity
{
	// Position of bottom centre of sprite
	v2 WorldPos;
	u16 Width;
	u16 Height;

	v2 Velocity;
	v2 LastMovementInput;


	u32 AnimationTimer;
	u32 AnimationFrameIndex;
	const animation* PlayingAnimation;

	OBJ_ATTR* Sprite;
};

struct bg_map
{
	u16 Width;
	u16 Height;

	u16 MinOffsetX;
	u16 MinOffsetY;
};