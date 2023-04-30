#pragma once

static const fixed WALK_SPEED = 1;
static const fixed RUN_SPEED = 3;
static const fixed JUMP_SPEED = 7;
static const fixed GRAVITY = F_HALF;

static const u32 DOUBLE_TAP_INTERVAL = 20; // Max num. frames in between presses to count as a double-tap
static const u32 PLAYER_FRAME_SIZE = 8 * 8 * sizeof(TILE);
static const u32 JUMP_DELAY = 7;

struct bg_map
{
	u16 Width;
	u16 Height;

	u16 MinOffsetX;
	u16 MinOffsetY;
};