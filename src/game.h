#ifndef __GAME_H__
#define __GAME_H__

#include <SDL2/SDL.h>
#include "sdl_framework.h"

using namespace std;

class Game : public SDLFramework
{
public:
	using SDLFramework::SDLFramework;

	virtual bool OnCreate()
		{
			level += "################################################################################";
			level += "#..............................................................................#";
			level += "#..............................................................................#";
			level += "#..............................................................................#";
			level += "#..............................................................................#";
			level += "#..............................................................................#";
			level += "#..............................................................................#";
			level += "#..............................................................................#";
			level += "#..............................................................................#";
			level += "#..........####................................................................#";
			level += "#..............................................................................#";
			level += "#......####....####............................................................#";
			level += "#..............................................................................#";
			level += "#..####....####....####........................................................#";
			level += "#..............................................................................#";
			level += "################################################################################";

			player = new Sprite(this, "./man.bmp");
			wall = new Sprite(this, "./brick.bmp");
			sky = new Sprite(this, "./sky.bmp");

			return true;
		}

	virtual bool OnUpdate(float elapsed_time)
		{
			static float time = 0.0f;
			time += elapsed_time;
			
			if (GetKey(Keys::LEFT).bHeld)
			{
				player_vx = -10.0f;
			}
			else if (GetKey(Keys::RIGHT).bHeld)
			{
				player_vx = 10.0f;
			}
			else
			{
				player_vx = 0.0f;
			}

			if (GetKey(Keys::UP).bPressed && player_vy == 0.0f)
			{
				player_vy = -10.0f;
			}

			// Check for collisions
			float new_player_x = player_x + player_vx * elapsed_time;
			float new_player_y = player_y + player_vy * elapsed_time;
			char c0 = level[static_cast<int>(floor(new_player_y + 0.01f) * level_width + floor(new_player_x + 0.01f))];
			char c1 = level[static_cast<int>(floor(new_player_y + 0.01f) * level_width + floor(new_player_x + 0.99f))];
			char c2 = level[static_cast<int>(floor(new_player_y + 0.99f) * level_width + floor(new_player_x + 0.01f))];
			char c3 = level[static_cast<int>(floor(new_player_y + 0.99f) * level_width + floor(new_player_x + 0.99f))];

			if (c0 == '.' && c1 == '.' && c2 == '.' && c3 == '.')
			{
				player_x = new_player_x;
				player_y = new_player_y;
			}
			else
			{
				player_vy = 0.0f;
			}

			camera_x = player_x;
			camera_y = player_y;

			// Draw level
			float tiles_x = ScreenWidth() / mag / tile;
			float tiles_y = ScreenHeight() / mag / tile;

			float origin_x = camera_x - (tiles_x / 2.0);
			float origin_y = camera_y - (tiles_y / 2.0);

			origin_x = max(origin_x, 0.0f);
			origin_x = min(origin_x, level_width - tiles_x);
			origin_y = max(origin_y, 0.0f);
			origin_y = min(origin_y, level_height - tiles_y);

			for (float y = 0.0f; y <= tiles_y + 0.5f; y += 1.0f)
			{
				for (float x = 0.0f; x <= tiles_x + 0.5f; x += 1.0f)
				{
					float loc_x = floor(origin_x + x);
					float loc_y = floor(origin_y + y);

					char c = level[static_cast<int>(loc_y * level_width + loc_x)];

					int pos_x = static_cast<int>((loc_x - origin_x) * mag * tile);
					int pos_y = static_cast<int>((loc_y - origin_y) * mag * tile);

					switch(c)
					{
					case '#':
						wall->Draw(pos_x, pos_y, mag);
						break;
					case '.':
						sky->Draw(pos_x, pos_y, mag);
						break;
					}
				}
			}

			// Draw player
			player->Draw((player_x - origin_x) * mag * tile, (player_y - origin_y) * mag * tile, mag);

			// Apply gravity to player's vertical velocity
			c0 = level[static_cast<int>(floor(player_y + 1.01f) * level_width + floor(player_x + 0.01f))];
			c1 = level[static_cast<int>(floor(player_y + 1.01f) * level_width + floor(player_x + 0.99f))];
			if (player_vy != 0.0f || (c0 == '.' && c1 == '.'))
			{
				player_vy += 20.0f * elapsed_time;
			}

			return true;
		}

	virtual void OnDestroy()
	{
		delete player;
		delete wall;
		delete sky;
	}

private:
	std::string level;
	Sprite *player;
	Sprite *wall;
	Sprite *sky;

	float player_x = 40.0f;
	float player_y = 14.0f;
	float player_vx = 0.0f;
	float player_vy = 0.0f;

	float camera_x = 0.0f;
	float camera_y = 0.0f;

	const float level_width = 80.0;
	const float level_height = 16.0;
	const float mag = 4.0;
	const float tile = 16.0;
};

#endif
