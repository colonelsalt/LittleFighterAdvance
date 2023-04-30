#pragma once

#include <tonc.h>

struct animation
{
	u32* Frames;
	u32 Length;
	u32 FrameDelay;
	b32 ShouldLoop;
};

enum entity_type
{
	EPlayer,
	EEnemy
};

struct entity
{
	entity_type Type;

	// Position of bottom centre of sprite
	v2 WorldPos;
	fixed ZPos;
	v2 ScreenPos;
	v2 GroundPosScreenSpace;

	u16 Width;
	u16 Height;

	b32 IsRunning;
	b32 IsAirborne;
	b32 StartedJump;
	u32 JumpTimer;

	v2 Velocity;
	fixed VelocityZ;
	s32 LastInputX;
	u32 FramesSinceLastMovement;

	u32 AnimationTimer;
	u32 AnimationFrameIndex;
	const animation* PlayingAnimation;

	OBJ_ATTR* Sprite;
	const u8* Tiles;
	u32 PaletteIndex;
};