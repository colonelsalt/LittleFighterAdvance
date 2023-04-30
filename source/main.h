#pragma once

static const fixed WALK_SPEED = 1;
static const fixed RUN_SPEED = 3;
static const fixed JUMP_SPEED = 7;
static const fixed GRAVITY = F_HALF;

static const u32 DOUBLE_TAP_INTERVAL = 18; // Max num. frames in between presses to count as a double-tap
static const u32 COMBO_INTERVAL = 10;
static const u32 PLAYER_FRAME_SIZE = 8 * 8 * sizeof(TILE);
static const u32 PARTICLE_FRAME_SIZE = 4 * 4 * sizeof(TILE);
static const u32 JUMP_DELAY = 7;
static const u32 MAX_PUNCH_COMBO = 4;
static const u32 FLINCH_DURATION = 30; // How many frames are you out for immediately after being hit
static const u32 KNOCKOUT_DURATION = 50; // How many frames you're out for when knocked to the ground

struct bg_map
{
	u16 Width;
	u16 Height;

	u16 MinOffsetX;
	u16 MinOffsetY;
};