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


static constexpr u16 State_Idle = 0;

static constexpr u16 State_Walking = 1;
static constexpr u16 State_Running = 1 << 1;

static constexpr u16 State_StartedJump = 1 << 2;
static constexpr u16 State_Airborne = 1 << 3;

static constexpr u16 State_Attacking = 1 << 4;

struct entity
{
	entity_type Type;
	u16 State;

	// Position of bottom centre of sprite
	v2 WorldPos;
	fixed ZPos;
	v2 ScreenPos;
	v2 GroundPosScreenSpace;

	u16 Width;
	u16 Height;

	u32 JumpTimer;

	v2 Velocity;
	fixed VelocityZ;
	s32 LastInputX;
	u32 FramesSinceLastMovement;
	u32 FramesSinceLastPunch;
	u32 ComboCount;

	s32 AnimationTimer;
	u32 AnimationFrameIndex;
	const animation* PlayingAnimation;

	OBJ_ATTR* Sprite;
	const u8* Tiles;
	u32 PaletteIndex;
};