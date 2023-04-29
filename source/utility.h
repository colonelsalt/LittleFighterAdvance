#pragma once

inline void SetObjPos(OBJ_ATTR* Obj, v2 ScreenPos)
{
	obj_set_pos(Obj, ScreenPos.X.WholePart, ScreenPos.Y.WholePart);
}

#define Assert(X) { if (!(X)) *(int*)0; }