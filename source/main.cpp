#include <tonc.h>

#include "maths.h"
#include "main.h"

#include "utility.h"

// This file is autogenerated from the file in the graphics folder
#include "freeze_0.h"
#include "bandit_0.h"

#include "forest_bg.h"
#include "blood.h"

#include "entity.cpp"
#include "player.cpp"

OBJ_ATTR g_ObjBuffer[128];
OBJ_AFFINE *g_AffineObjBuffer = (OBJ_AFFINE*)g_ObjBuffer;

static void LoadSprite(const u16* Palette, u32 PaletteSize,
                       u32 SpriteIndex, u32 PaletteIndex,
                       OBJ_ATTR* OutSprite)
{
	// Load palette
	memcpy32(pal_obj_bank + PaletteIndex, Palette, PaletteSize / 4);

	obj_set_attr(OutSprite,
	             ATTR0_SQUARE,  // Square, regular sprite
	             ATTR1_SIZE_64, // 64x64 pixels,
	             ATTR2_PALBANK(PaletteIndex) | SpriteIndex * 64);
}

static void LoadBackground(void)
{
	// Load palette
	memcpy32(pal_bg_mem, forest_bgPal, forest_bgPalLen / 4);
	// Load tiles into CBB 0
	memcpy32(&tile_mem[0][0], forest_bgTiles, forest_bgTilesLen / 4);
	// Load map into SBB 30
	memcpy32(&se_mem[30][0], forest_bgMap, forest_bgMapLen / 4);

	REG_BG0CNT = BG_CBB(0) | BG_SBB(30) | BG_4BPP | BG_REG_32x32;
}

static b32 g_CanDraw;
static entity Player;
static entity Enemy;

void OnVBlank()
{
	if (g_CanDraw)
	{
		g_CanDraw = false;

		u32 ObjIndex = 0;
		
		// Always draw particles in front of other sprites
		for (u32 i = 0; i < ArrayCount(Particles); i++)
		{
			entity* Particle = &Particles[i];
			if (Particle->IsActive)
			{
				u32 TileIndex = (2 * 64 + 16 * ObjIndex);
				obj_unhide(&Particle->Sprite, ATTR0_REG);
				Particle->Sprite.attr1 = ATTR1_SIZE_32;
				Particle->Sprite.attr2 = ATTR2_PALBANK(2) | TileIndex;
				SetObjPos(&Particle->Sprite, Particle->ScreenPos);

				g_ObjBuffer[ObjIndex++] = Particle->Sprite;
			}
		}

		// If Player is in front of enemy (higher Y), set it as the first OBJ, so it's drawn last
		if (Player.WorldPos.Y > Enemy.WorldPos.Y)
		{
			g_ObjBuffer[ObjIndex++] = Player.Sprite;
			g_ObjBuffer[ObjIndex++] = Enemy.Sprite;
		}
		else
		{
			// Otherwise, set enemy as first OBJ
			g_ObjBuffer[ObjIndex++] = Enemy.Sprite;
			g_ObjBuffer[ObjIndex++] = Player.Sprite;
		}

		u32 TileOffset = 0;

		u32 PlayerFrame = Player.PlayingAnimation->Frames[Player.AnimationFrameIndex];
		memcpy32(&tile_mem[4][TileOffset], Player.Tiles + PlayerFrame * PLAYER_FRAME_SIZE, PLAYER_FRAME_SIZE / 4);
		TileOffset += 64;

		u32 EnemyFrame = Enemy.PlayingAnimation->Frames[Enemy.AnimationFrameIndex];
		memcpy32(&tile_mem[4][TileOffset], Enemy.Tiles + EnemyFrame * PLAYER_FRAME_SIZE, PLAYER_FRAME_SIZE / 4);
		TileOffset += 64;

		for (u32 i = 0; i < ArrayCount(Particles); i++)
		{
			entity* Particle = &Particles[i];
			if (Particle->IsActive)
			{
				u32 ParticleFrame = Particle->PlayingAnimation->Frames[Particle->AnimationFrameIndex];
				memcpy32(&tile_mem[4][TileOffset], bloodTiles + PARTICLE_FRAME_SIZE * ParticleFrame, PARTICLE_FRAME_SIZE / 4);
				TileOffset += 16;
			}
		}

		for (u32 i = ObjIndex; i < ObjIndex + 10; i++)
		{
			obj_hide(&g_ObjBuffer[i]);
		}
		oam_copy(oam_mem, g_ObjBuffer, ObjIndex + 10);
	}
}

static const bg_map ForestMap =
{ 
	500, 256,
	0, 0
};

int main(void)
{
	irq_init(nullptr);
	irq_enable(II_VBLANK);
	irq_add(II_VBLANK, OnVBlank);

	oam_init(g_ObjBuffer, 128);

	LoadSprite(freeze_0Pal, freeze_0PalLen, 0, 0, &Player.Sprite);
	Player.Tiles = freeze_0Tiles;
	Player.Type = EPlayer;
	Player.IsActive = true;

	LoadSprite(bandit_0Pal, bandit_0PalLen, 1, 1, &Enemy.Sprite);
	Enemy.Tiles = bandit_0Tiles;
	Enemy.Type = EEnemy;
	Enemy.IsActive = true;

	Enemy.WorldPos.X = 300;
	Enemy.WorldPos.Y = 50;

	v2 PlayerStartPos = {};
	PlayerStartPos.X = 0;
	PlayerStartPos.Y = 90;
	
	Player.WorldPos = {};
	Player.WorldPos.X = Player.Width / 2;
	Player.WorldPos.Y = 0;

	for (u32 i = 0; i < ArrayCount(Particles); i++)
	{
		Particles[i].Type = EParticle;
		LoadSprite(bloodPal, bloodPalLen, 0, 2, &Particles[i].Sprite);
	}

	Humans[0] = &Player;
	Humans[1] = &Enemy;

	SetAnimation(&Player, &AnimIdle);
	SetAnimation(&Enemy, &AnimIdle);

	Player.Width = 64;
	Player.Height = 64;

	LoadBackground();

	REG_DISPCNT = DCNT_MODE0 | DCNT_BG0 | DCNT_OBJ | DCNT_OBJ_1D;

	v2 CameraPos = {};

	while (true)
	{
		key_poll();

		HandlePlayerInput(&Player);
		MoveEntity(&Player, &ForestMap, &Player);
		MoveEntity(&Enemy, &ForestMap, &Player);
		for (u32 i = 0; i < ArrayCount(Particles); i++)
		{
			MoveEntity(&Particles[i], &ForestMap, &Player);
		}


		v2 Cutoff = {};
		Cutoff.X = ForestMap.Width - SCREEN_WIDTH;
		Cutoff.Y = ForestMap.Height - SCREEN_HEIGHT;

		Player.ScreenPos = PlayerStartPos;
		if (Player.WorldPos.X >= Cutoff.X)
		{
			// If there's less than a screen's width left of the map, move the player across the screen
			Player.ScreenPos.X += Player.WorldPos.X - Cutoff.X;
		}
		else
		{
			// Otherwise, just move the camera to follow the player
			CameraPos.X = Player.WorldPos.X - Player.Width / 2;
		}

		if (Player.WorldPos.Y >= Cutoff.Y)
		{
			Player.ScreenPos.Y += Player.WorldPos.Y - Cutoff.Y;
		}
		else
		{
			CameraPos.Y = Player.WorldPos.Y;
		}
		Player.GroundPosScreenSpace = Player.ScreenPos;
		Player.ScreenPos.Y -= Player.ZPos;

		REG_BG0HOFS = CameraPos.X.WholePart;
		REG_BG0VOFS = CameraPos.Y.WholePart;

		AnimateEntity(&Player);
		AnimateEntity(&Enemy);
		for (u32 i = 0; i < ArrayCount(Particles); i++)
		{
			entity* Particle = &Particles[i];
			if (Particle->IsActive)
			{
				AnimateEntity(Particle);
				SetObjPos(&Particle->Sprite, Particle->ScreenPos);
			}
		}

		SetObjPos(&Player.Sprite, Player.ScreenPos);
		SetObjPos(&Enemy.Sprite, Enemy.ScreenPos);

		if (key_hit(KEY_START))
		{
			Enemy.IsActive = true;
			Enemy.State = State_Idle;
		}

		g_CanDraw = true;
		VBlankIntrWait();
	}
}
