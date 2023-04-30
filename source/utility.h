#pragma once

#define Assert(X) { if (!(X)) *(int*)0; }

inline void SetObjPos(OBJ_ATTR* Obj, v2 ScreenPos)
{
	obj_set_pos(Obj, ScreenPos.X.WholePart, ScreenPos.Y.WholePart);
}

inline void SetHFlip(OBJ_ATTR* Obj, b32 HFlip)
{
	if (HFlip)
	{
		Obj->attr1 |= ATTR1_HFLIP;
	}
	else
	{
		Obj->attr1 &= ~ATTR1_HFLIP;
	}
}