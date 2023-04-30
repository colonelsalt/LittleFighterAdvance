#include <tonc.h>

// Assign velocity and jump state to player based on input
static void HandlePlayerInput(entity* Player)
{
	if (key_hit(KEY_A) && !(Player->State & State_Airborne))
	{
		Player->State |= State_StartedJump;
		Player->JumpTimer = JUMP_DELAY;
	}
	if (key_hit(KEY_B))
	{
		Player->State |= State_Attacking;
		Player->FramesSinceLastPunch = 0;
	}
	else
	{
		Player->FramesSinceLastPunch++;
	}

	if (Player->State & State_StartedJump)
	{
		Player->JumpTimer--;
		if (Player->JumpTimer <= 0)
		{
			Player->State &= ~State_StartedJump;
			Player->State |= State_Airborne;
			Player->VelocityZ = JUMP_SPEED;
		}
	}

	if (Player->State & State_Airborne)
	{
		Player->VelocityZ -= GRAVITY;
		Player->ZPos += Player->VelocityZ;
		if (Player->ZPos < 0)
		{
			Player->ZPos = 0;
			Player->State &= ~(State_Airborne | State_Attacking);
		}
	}

	if (key_hit(KEY_LEFT))
	{
		SetHFlip(&Player->Sprite, true);

		if (Player->LastInputX < 0 && Player->FramesSinceLastMovement <= DOUBLE_TAP_INTERVAL)
		{
			Player->State |= State_Running;
			Player->Velocity.X = -RUN_SPEED;
		}
		else
		{
			if (Player->Velocity.X >= RUN_SPEED)
			{
				Player->State &= ~State_Running;
				Player->Velocity.X = 0;
			}
		}
		Player->LastInputX = -1;
		Player->FramesSinceLastMovement = 0;
	}
	else if (key_hit(KEY_RIGHT))
	{
		SetHFlip(&Player->Sprite, false);	

		if (Player->LastInputX > 0 && Player->FramesSinceLastMovement <= DOUBLE_TAP_INTERVAL)
		{
			Player->State |= State_Running;
			Player->Velocity.X = RUN_SPEED;
		}
		else
		{
			if (Player->Velocity.X <= -RUN_SPEED)
			{
				Player->State &= ~State_Running;
				Player->Velocity.X = 0;
			}
		}
		Player->LastInputX = 1;
		Player->FramesSinceLastMovement = 0;
	}
	else
	{
		Player->FramesSinceLastMovement++;
	}

	if (!(Player->State & State_Running))
	{
		Player->Velocity.X = WALK_SPEED * key_tri_horz();
	}
	Player->Velocity.Y = WALK_SPEED * key_tri_vert();
}