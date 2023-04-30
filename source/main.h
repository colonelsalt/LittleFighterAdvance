#pragma once

struct animation
{
	u32* Frames;
	u32 Length;
	u32 FrameDelay;
	b32 ShouldLoop;
};

static const fixed WALK_SPEED = 1;
static const fixed RUN_SPEED = 3;

static const u32 DOUBLE_TAP_INTERVAL = 20; // Max num. frames in between presses to count as a double-tap

struct entity
{
	// Position of bottom centre of sprite
	v2 WorldPos;
	u16 Width;
	u16 Height;

	b32 IsRunning;
	v2 Velocity;
	s32 LastInputX;
	u32 FramesSinceLastMovement;

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