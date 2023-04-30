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

static const animation AnimStandardPunch1 =
{
	.Frames = (u32[]){10, 11},
	.Length = 2,
	.FrameDelay = 6,
	.ShouldLoop = false
};

static const animation AnimStandardPunch2 =
{
	.Frames = (u32[]){12, 13},
	.Length = 2,
	.FrameDelay = 6,
	.ShouldLoop = false
};

static const animation AnimStandardPunch3 =
{
	.Frames = (u32[]){17, 18, 19},
	.Length = 3,
	.FrameDelay = 8,
	.ShouldLoop = false
};

static const animation* PunchAnimations[MAX_PUNCH_COMBO] =
{
	&AnimStandardPunch1,
	&AnimStandardPunch2,
	&AnimStandardPunch1,
	&AnimStandardPunch3
};

static const animation AnimJumpAttack =
{
	.Frames = (u32[]){14, 15, 16},
	.Length = 3,
	.FrameDelay = 5,
	.ShouldLoop = false
};

static const animation AnimTakingDamage =
{
	.Frames = (u32[]){48, 47, 46},
	.Length = 3,
	.FrameDelay = 15,
	.ShouldLoop = false
};

static const animation AnimBloodSplatter =
{
	.Frames = (u32[]){0, 1, 2, 3, 4, 5},
	.Length = 6,
	.FrameDelay = 5,
	.ShouldLoop = false
};

static constexpr u32 NUM_PARTICLES = 10;

static entity* Humans[2];
static entity Particles[NUM_PARTICLES];

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
		b32 result = Entity->AnimationFrameIndex >= Animation->Length - 1 && Entity->AnimationTimer <= 0;
		return result;
	}
	return false;
}


static void AnimateEntity(entity* Entity)
{
	const animation* Animation = Entity->PlayingAnimation;
	Entity->AnimationTimer--;
	if (Entity->AnimationTimer <= 0)
	{
		if (Animation->ShouldLoop)
		{
			Entity->AnimationFrameIndex++;
			Entity->AnimationTimer = Animation->FrameDelay;
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
				Entity->AnimationTimer = Animation->FrameDelay;
			}
		}
	}
}

static void SpawnParticle(v2 SpawnPos)
{
	for (u32 i = 0; i < ArrayCount(Particles); i++)
	{
		entity* Particle = &Particles[i];
		if (!Particle->IsActive)
		{
			Particle->IsActive = true;
			Particle->WorldPos = SpawnPos;
			SetAnimation(Particle, &AnimBloodSplatter);
			return;
		}
	}
	Assert(false);
}

static void TakeDamage(entity* Entity, u32 Amount)
{
	Entity->Health -= Amount;
	Entity->State |= State_TakingDamage;
	Entity->FlinchTimer = 0;
}

static b32 IsEntityFacingLeft(entity* Entity)
{
	return Entity->Sprite.attr1 & ATTR1_HFLIP;
}

const u32 EPSILON = 50;

// Move entity in world & screen space based on its velocity & jump state
static void MoveEntity(entity* Entity, const bg_map* Map, entity* Player)
{
	if (!Entity->IsActive)
	{
		return;
	}

	if (!(Entity->State & State_Immobile))
	{
		Entity->WorldPos += Entity->Velocity;
	}

	if (Entity->State & State_TakingDamage)
	{
		Entity->FlinchTimer++;
		if (Entity->FlinchTimer >= FLINCH_DURATION)
		{
			Entity->State &= ~State_TakingDamage;
		}
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

	if (Entity->Type != EPlayer)
	{
		v2 OffsetFromPlayer = Entity->WorldPos - Player->WorldPos;
		Entity->ScreenPos = Player->GroundPosScreenSpace + OffsetFromPlayer;
	}

	if ((Entity->State & State_Attacking) && Entity->AnimationFrameIndex == Entity->PlayingAnimation->Length - 1)
	{
		for (u32 i = 0; i < ArrayCount(Humans); i++)
		{
			entity* Other = Humans[i];
			if (Other->IsActive && (Other->Type == EPlayer || Other->Type == EEnemy) && Other != Entity)
			{
				v2 HandPos = Entity->WorldPos;
				HandPos.Y += Entity->Height / 2;
				if (!IsEntityFacingLeft(Entity))
				{
					HandPos.X += Entity->Width;
				}
				v2 OtherPos = Other->WorldPos;
				OtherPos.X += Other->Width;
				OtherPos.Y += Other->Height;

				v2 ToOther = HandPos - OtherPos;
				if (ToOther.X < 0)
				{
					SetHFlip(&Other->Sprite, true);
				}
				else
				{
					SetHFlip(&Other->Sprite, false);
				}

				fixed SqDist = SqMagnitude(ToOther);
				if (SqDist < EPSILON * EPSILON)
				{
					TakeDamage(Other, Entity->State & State_Running ? 2 : 1);
					SpawnParticle(HandPos);
				}
			}
		}
	}

	//
	// Animation update
	//
	if (Entity->Type == EParticle)
	{
		if (HasAnimationEnded(Entity))
		{
			Entity->IsActive = false;
			Entity->PlayingAnimation = nullptr;
		}
		return;
	}

	if (Entity->State & State_TakingDamage)
	{
		SetAnimation(Entity, &AnimTakingDamage);
	}
	else if (Entity->State & State_StartedJump)
	{
		SetAnimation(Entity, &AnimStartJump);
	}
	else if (Entity->State & State_Airborne)
	{
		if (Entity->State & State_Attacking)
		{
			SetAnimation(Entity, &AnimJumpAttack);
		}
		else if (FAbs(Entity->Velocity.X) >= RUN_SPEED)
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
		if (Entity->State & State_Running)
		{
			if (Entity->State & State_Attacking)
			{
				SetAnimation(Entity, &AnimStandardPunch3);
				if (HasAnimationEnded(Entity))
				{
					Entity->State &= ~(State_Running | State_Attacking);
				}
			}
			else
			{
				SetAnimation(Entity, &AnimRun);
			}

		}
		else if (Entity->State & State_Attacking)
		{
			SetAnimation(Entity, PunchAnimations[Entity->ComboCount]);
			if (HasAnimationEnded(Entity))
			{
				if (Entity->FramesSinceLastPunch > COMBO_INTERVAL ||
					Entity->ComboCount >= MAX_PUNCH_COMBO - 1)
				{
					Entity->ComboCount = 0;
					Entity->State &= ~State_Attacking;
				}
				else
				{
					Entity->ComboCount++;
				}
			}
		}
		else
		{
			if (SqMagnitude(Entity->Velocity) > 0)
			{
				SetAnimation(Entity, &AnimWalk);
			}
			else
			{
				SetAnimation(Entity, &AnimIdle);
			}
		}
	}

}