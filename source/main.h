#pragma once

struct player
{
	v2 WorldPos;
	u16 Width;
	u16 Height;

	OBJ_ATTR* Sprite;
};

struct bg_map
{
	u16 Width;
	u16 Height;

	u16 MinOffsetX;
	u16 MinOffsetY;
};