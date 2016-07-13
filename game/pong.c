/*======================================================================
 MAP-Elites RISC
 version 1.0
------------------------------------------------------------------------
 Copyright (c) 2016 Anael Seghezzi

 This software is provided 'as-is', without any express or implied
 warranty. In no event will the authors be held liable for any damages
 arising from the use of this software.

 Permission is granted to anyone to use this software for any purpose,
 including commercial applications, and to alter it and redistribute it
 freely, subject to the following restrictions:

 1. The origin of this software must not be misrepresented; you must not
    claim that you wrote the original software. If you use this software
    in a product, an acknowledgment in the product documentation would
    be appreciated but is not required.

 2. Altered source versions must be plainly marked as such, and must not
    be misrepresented as being the original software.

 3. This notice may not be removed or altered from any source
    distribution.

========================================================================*/

// solo PONG
// score increments each time the ball hit the top screen
// the bar movement is controlled by setting the value "bar_d" (-1 = left, 1 = right)

struct pong
{
	struct m_image image;
	float2 ball_pos;
	float2 ball_dir;
	float bar_x;
	float bar_d;
	int bar_r;
	int score;
};


static unsigned int _pong_rz = 362436069;
static unsigned int _pong_rw = 521288629;

unsigned int pong_rand(void)
{
   _pong_rz = 36969 * (_pong_rz & 65535) + (_pong_rz >> 16);
   _pong_rw = 18000 * (_pong_rw & 65535) + (_pong_rw >> 16);
   return (_pong_rz << 16) + _pong_rw;
}

float pong_randf(void)
{
   unsigned int u = pong_rand();
   return (u + 1.0) * 2.328306435454494e-10;
}

void pong_reset(struct pong *game)
{
	game->ball_pos.x = game->image.width / 2;
	game->ball_pos.y = 0;
	game->ball_dir.x = pong_randf() - 0.5;
	game->ball_dir.y = 1;
	M_NORMALIZE2(game->ball_dir, game->ball_dir);
}

void pong_clear(struct pong *game)
{
	_pong_rz = 362436069;
	_pong_rw = 521288629;
	pong_reset(game);
	game->score = 0;
	game->bar_x = game->image.width / 2;
	game->bar_r = 2;
}

void pong_create(struct pong *game)
{
	memset(&game->image, 0, sizeof(game->image));
	m_image_create(&game->image, M_FLOAT, 24, 24, 1);
	pong_clear(game);
}

void pong_destroy(struct pong *game)
{
	m_image_destroy(&game->image);
}

void pong_update(struct pong *game)
{
	float rand_factor = 0.25;
	int w0 = game->image.width - 1;
	int h0 = game->image.height - 1;
	int x, y, x1, x2;

	x = game->ball_pos.x;
	y = game->ball_pos.y;
	x1 = (int)game->bar_x - game->bar_r - 1;
	x2 = (int)game->bar_x + game->bar_r + 1;

	// update bar position
	game->bar_x += M_MIN(game->bar_d, 1);
	game->bar_x = M_CLAMP(game->bar_x, game->bar_r, w0 - game->bar_r);
	game->bar_d = 0;

	// update ball position
	game->ball_pos.x += game->ball_dir.x;
	game->ball_pos.y += game->ball_dir.y;

	// bounce walls
	if (game->ball_pos.x < 0) {
		game->ball_pos.x = 0;
		game->ball_dir.x = -game->ball_dir.x;
		game->ball_dir.x += (pong_randf() - 0.5) * rand_factor;
		M_NORMALIZE2(game->ball_dir, game->ball_dir);
	}
	if (game->ball_pos.y < 0) {
		game->ball_pos.y = 0;
		game->ball_dir.y = -game->ball_dir.y;
		game->ball_dir.x += (pong_randf() - 0.5) * rand_factor;
		M_NORMALIZE2(game->ball_dir, game->ball_dir);
		game->score++;
	}
	if (game->ball_pos.x > w0) {
		game->ball_pos.x = w0;
		game->ball_dir.x = -game->ball_dir.x;
		game->ball_dir.x += (pong_randf() - 0.5) * rand_factor;
		M_NORMALIZE2(game->ball_dir, game->ball_dir);
	}
	
	if (game->ball_pos.y > h0) {

		// bounce bar
		if (game->ball_pos.x > x1 && game->ball_pos.x < x2) {
			game->ball_pos.y = h0;
			game->ball_dir.y = -game->ball_dir.y;
			game->ball_dir.y += (pong_randf() - 0.5) * rand_factor;
			M_NORMALIZE2(game->ball_dir, game->ball_dir);
		}
		// reset
		else {
			pong_reset(game);
		}
	}
}

void pong_draw(struct pong *game)
{
	float *pixel;
	int x, y, x1, x2;

	memset(game->image.data, 0, game->image.size * sizeof(float));

	x = game->ball_pos.x;
	y = game->ball_pos.y;

	pixel = (float *)game->image.data + y * game->image.width + x;
	*pixel = 1;

	y = game->image.height - 1;
	x1 = (int)game->bar_x - game->bar_r;
	x2 = (int)game->bar_x + game->bar_r + 1;
	pixel = (float *)game->image.data + y * game->image.width + x1;
	
	for (x = x1; x < x2; x++) {
		*pixel = 1;
		pixel++;
	}
}
