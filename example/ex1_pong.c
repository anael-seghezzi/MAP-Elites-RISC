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

// ex1: evolve a program playing pong
// the best possible score after 1024 frames is 20


#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <memory.h>
#include <math.h>
#include <limits.h>

#define M_MATH_IMPLEMENTATION
#define M_IMAGE_IMPLEMENTATION
#include "m_math.h"
#include "m_image.h"
#include "img_utils.c"


// settings
#define DIM 32
#define MAX_CALL (DIM * 2)
#define FRAME_COUNT 1024
#define EXPORT_GAME_FRAMES


// MAP-Elites
#define ME_X
#define ME_SIZEX DIM
#define ME_SIZEY MAX_CALL

#include "../src/mapelites_dna_float.c"
#include "../src/asmgen.c"
#include "../game/pong.c"


// project
struct me_project project;
struct pong game;


void me_mutate(me_dna *dest, int init)
{
	int i;

	if (init) {
		
		float pr = m_randf();

		// randomize
		for (i = 0; i < dest->size; i++)
			dest->genome[i] = m_randf();

		// mask with pr probability
		for (i = 5; i < (DIM * 6); i+=6) {
			if (m_randf() < pr)
				dest->genome[i] = m_randf() * 0.5 + 0.5;
			else
				dest->genome[i] = m_randf() * 0.5;
		}
	}
	else {

		float pr = 6.0 / dest->size;

		// mutate with pr probability
		for (i = 0; i <  dest->size; i++) {
			if (m_randf() < pr) {
				float v = dest->genome[i] + (m_randf() - 0.5);
				if (v >= 0.0 && v <= 1.0)
					dest->genome[i] = v;
			}
		}
	}
}

void me_pos(me_dna *src, struct me_info *info, int *x, int *y)
{
	*y = info->user1 * (ME_SIZEY - 1);
	*x = info->user2 * (ME_SIZEX - 1);	
}

void to_asmgen(struct asmgen *dest, me_dna *src)
{
	float *phenotype = src->genome;
	int i;
	
	asmgen_create(dest, 4, DIM, 3);

	for (i = 0; i < DIM; i++) {
		if (phenotype[5] > 0.5) {
			
			dest->code[i].i = (int)(phenotype[0] * 65536) % ASMGEN_LOADF; // USE INTEGER LOGIC ONLY
			dest->code[i].a = (int)(phenotype[1] * 65536) % (ASMGEN_REG - 1) + 1;
			dest->code[i].b = (int)(phenotype[2] * 65536) % ASMGEN_REG;
			dest->code[i].c = (int)(phenotype[3] * 65536) % ASMGEN_REG;
			dest->code[i].o = (phenotype[4] * 2.0 - 1.0) * 32768;

			switch(dest->code[i].i) {
			case ASMGEN_LOAD:
			case ASMGEN_STORE:
			case ASMGEN_LOADF:
			case ASMGEN_STOREF:
				dest->code[i].o = dest->code[i].o % dest->mem_size; // stay in memory range
				break;
			case ASMGEN_BRANCH:
			case ASMGEN_NBRANCH:
			case ASMGEN_GOTO:
				dest->code[i].o = dest->code[i].o % dest->code_size; // stay in code range
				break;
			}
		}
		else {
			dest->code[i].i = 0;
		}
		phenotype += 6;
	}
}

void me_eval(int threadi, struct me_project *project, me_dna *src, struct me_info *info)
{
	struct asmgen program;
	int64_t instr_count = 0;
	int i;

	to_asmgen(&program, src);
	pong_clear(&game);

	for (i = 0; i < FRAME_COUNT; i++) {

		// set read-only memory
		program.mem[0] = (int)game.ball_pos.x;
		program.mem[1] = (int)game.ball_pos.y;
		program.mem[2] = (int)game.bar_x;

		// run and record
		instr_count += asmgen_run(&program, MAX_CALL);

		// update game
		game.bar_d = (float)M_CLAMP(program.mem[3], -1, 1);
		pong_update(&game);
	}

	info->fitness = game.score;
	info->user1 = instr_count / (double)(MAX_CALL * FRAME_COUNT);
	info->user2 = asmgen_line_count(&program) / (double)program.code_size;
	asmgen_destroy(&program);
}

int get_best(float *data, int count)
{
	int best = 0, i;

	for (i = 1; i < count; i++) {
		if (data[i] > data[best])
			best = i;
	}
	
	return best;
}


int main(int argc, char **argv)
{
	printf("<mapelite pong>\n");

	pong_create(&game);
	me_create(&project, DIM * 6);

	while (project.generation < 500000) {

		me_update(&project);

		if ((project.generation % 1000) == 0) {

			// save
			struct asmgen program;
			int i = get_best(project.map, ME_POP_COUNT);
			to_asmgen(&program, &project.cell[i].best);
			asmgen_save(&program, "pong_best.cgen");
			asmgen_save_text(&program, "pong_best.cgen.txt");
			asmgen_destroy(&program);

			// print current best score
			printf("%.4d: %.10f\n", project.generation, project.best_score);
		}
	}

	// play the best game and save frames to disc
	#ifdef EXPORT_GAME_FRAMES
	{
		char filename[256];
		struct asmgen program;
		int i;

		i = get_best(project.map, ME_POP_COUNT);
		to_asmgen(&program, &project.cell[i].best);

		pong_clear(&game);

		for (i = 0; i < FRAME_COUNT; i++) {

			// set read-only memory
			program.mem[0] = (int)game.ball_pos.x;
			program.mem[1] = (int)game.ball_pos.y;
			program.mem[2] = (int)game.bar_x;

			// run and record
			asmgen_run(&program, MAX_CALL);

			// update game
			game.bar_d = (float)M_CLAMP(program.mem[3], -1, 1);
			pong_update(&game);
			pong_draw(&game);

			sprintf(filename, "pong_frame%.4d.tga", i);
			img_save(&game.image, filename);
		}

		asmgen_destroy(&program);
	}
	#endif

	me_destroy(&project);
	pong_destroy(&game);
	return EXIT_SUCCESS;
}
