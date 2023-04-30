#include "entity.h"

static const animation AnimIdle =
{
	.Frames = (u32[]){0, 1, 2, 3},
	.Length = 4,
	.FrameDelay = 10,
	.ShouldLoop = true
};

static const animation AnimWalk =
{
	.Frames = (u32[]){4, 5, 6, 7},
	.Length = 4,
	.FrameDelay = 7,
	.ShouldLoop = true
};

static const animation AnimRun =
{
	.Frames = (u32[]){20, 21, 22},
	.Length = 3,
	.FrameDelay = 5,
	.ShouldLoop = true
};

static const animation AnimStartJump =
{
	.Frames = (u32[]){60, 61},
	.Length = 2,
	.FrameDelay = 6,
	.ShouldLoop = false
};

static const animation AnimJumpLoop = 
{
	.Frames = (u32[]){62},
	.Length = 1,
	.FrameDelay = 1,
	.ShouldLoop = true
};

static const animation AnimJumpFastLoop = 
{
	.Frames = (u32[]){63},
	.Length = 1,
	.FrameDelay = 1,
	.ShouldLoop = true
};

inline void SetAnimation(entity* Entity, const animation* Animation)
{
	if (Entity->PlayingAnimation != Animation)
	{
		Entity->PlayingAnimation = Animation;
		Entity->AnimationTimer = Animation->FrameDelay;
		Entity->AnimationFrameIndex = 0;
	}
}

inline b32 HasAnimationEnded(entity* Entity)
{
	const animation* Animation = Entity->PlayingAnimation;
	if (!Animation->ShouldLoop)
	{
		return Entity->AnimationFrameIndex >= Animation->Length - 1;
	}
	return false;
}


static void AnimateEntity(entity* Entity)
{
	const animation* Animation = Entity->PlayingAnimation;
	Entity->AnimationTimer--;
	if (Entity->AnimationTimer <= 0)
	{
		Entity->AnimationTimer = Animation->FrameDelay;
		if (Animation->ShouldLoop)
		{
			Entity->AnimationFrameIndex++;
			if (Entity->AnimationFrameIndex >= Animation->Length)
			{
				Entity->AnimationFrameIndex = 0;
			}
		}
		else
		{
			if (Entity->AnimationFrameIndex < Animation->Length - 1)
			{
				Entity->AnimationFrameIndex++;
			}
		}
	}
}

// Move entity in world & screen space based on its velocity & jump state
static void MoveEntity(entity* Entity, const bg_map* Map, entity* Player)
{
	if (!Entity->StartedJump)
	{
		Entity->WorldPos += Entity->Velocity;
	}
		
	if (Entity->WorldPos.X < 0)
	{
		Entity->WorldPos.X = 0;
	}
	if (Entity->WorldPos.X > Map->Width)
	{
		Entity->WorldPos.X = Map->Width;
	}

	if (Entity->WorldPos.Y > 104)
	{
		Entity->WorldPos.Y = 104;
	}
	if (Entity->WorldPos.Y < 0)
	{
		Entity->WorldPos.Y = 0;
	}

	if (Entity->StartedJump)
	{
		SetAnimation(Entity, &AnimStartJump);
	}
	else if (Entity->IsAirborne)
	{
		if (Entity->Velocity.X <= -RUN_SPEED)
		{
			SetAnimation(Entity, &AnimJumpFastLoop);
		}
		else if (Entity->Velocity.X >= RUN_SPEED)
		{
			SetAnimation(Entity, &AnimJumpFastLoop);
		}
		else
		{
			SetAnimation(Entity, &AnimJumpLoop);
		}
	}
	else
	{
		if (Entity->IsRunning)
		{
			SetAnimation(Entity, &AnimRun);
		}
		else if (SqMagnitude(Entity->Velocity) > 0)
		{
			SetAnimation(Entity, &AnimWalk);
		}
		else
		{
			SetAnimation(Entity, &AnimIdle);
		}
	}

	if (Entity->Type == EEnemy)
	{
		v2 OffsetFromPlayer = Entity->WorldPos - Player->WorldPos;
		Entity->ScreenPos = Player->GroundPosScreenSpace + OffsetFromPlayer;
	}

}