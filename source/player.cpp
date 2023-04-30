#include <tonc.h>

// Assign velocity and jump state to player based on input
static void HandlePlayerInput(entity* Player)
{
	if (Player->StartedJump)
	{
		Player->JumpTimer--;
		if (Player->JumpTimer <= 0)
		{
			Player->StartedJump = false;
			Player->VelocityZ = JUMP_SPEED;
			Player->IsAirborne = true;
		}
	}

	if (Player->IsAirborne)
	{
		Player->VelocityZ -= GRAVITY;
		Player->ZPos += Player->VelocityZ;
		if (Player->ZPos < 0)
		{
			Player->ZPos = 0;
			Player->IsAirborne = false;
		}
	}

	if (key_hit(KEY_LEFT))
	{
		SetHFlip(Player->Sprite, true);

		if (Player->LastInputX < 0 && Player->FramesSinceLastMovement <= DOUBLE_TAP_INTERVAL)
		{
			Player->IsRunning = true;
			Player->Velocity.X = -RUN_SPEED;
		}
		else
		{
			if (Player->Velocity.X >= RUN_SPEED)
			{
				Player->IsRunning = false;
				Player->Velocity.X = 0;
			}
		}
		Player->LastInputX = -1;
		Player->FramesSinceLastMovement = 0;
	}
	else if (key_hit(KEY_RIGHT))
	{
		SetHFlip(Player->Sprite, false);	

		if (Player->LastInputX > 0 && Player->FramesSinceLastMovement <= DOUBLE_TAP_INTERVAL)
		{
			Player->IsRunning = true;
			Player->Velocity.X = RUN_SPEED;
		}
		else
		{
			if (Player->Velocity.X <= -RUN_SPEED)
			{
				Player->IsRunning = false;
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

	if (key_hit(KEY_A) && !Player->IsAirborne)
	{
		Player->StartedJump = true;
		Player->JumpTimer = JUMP_DELAY;
	}


	if (!Player->IsRunning)
	{
		Player->Velocity.X = WALK_SPEED * key_tri_horz();
	}
	Player->Velocity.Y = WALK_SPEED * key_tri_vert();
}