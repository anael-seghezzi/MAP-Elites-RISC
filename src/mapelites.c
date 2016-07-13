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

// generic MAP-Elites
// calling code defines me_dna, me_dna_copy, me_mutate, me_pos, me_eval


#ifndef ME_TMP_SIZE
#define ME_TMP_SIZE 1
#endif

#define ME_POP_COUNT (ME_SIZEX * ME_SIZEY)


struct me_info
{
	float fitness;
	float user1;
	float user2;
	int parent;
};

struct me_cell
{
	me_dna best;
	int state;
};

struct me_project
{
	me_dna tmp[ME_TMP_SIZE];
	struct me_info tmpi[ME_TMP_SIZE];
	
	float *map;
	struct me_cell *cell;

	int64_t generation;
	float best_score;
};


// prototypes
void me_dna_copy(me_dna *dest, me_dna *src);
void me_mutate(me_dna *dest, int init);
void me_pos(me_dna *src, struct me_info *info, int *x, int *y);
void me_eval(int threadi, struct me_project *project, me_dna *src, struct me_info *info);


void me_create_internal(struct me_project *project)
{
	project->map = (float *)calloc(ME_POP_COUNT, sizeof(float));
	project->cell = (struct me_cell *)calloc(ME_POP_COUNT, sizeof(struct me_cell));
	project->generation = 0;
	project->best_score = -1;
}

void me_destroy_internal(struct me_project *project)
{
	free(project->map);
	free(project->cell);
}

void me_restart(struct me_project *project)
{
	int i;

	memset(project->map, 0, ME_POP_COUNT * sizeof(float));
	for (i = 0; i < ME_POP_COUNT; i++)
		project->cell[i].state = 0;

	project->generation = 0;
	project->best_score = -1;
}

void me_update(struct me_project *project)
{
	int i;

	for (i = 0; i < ME_TMP_SIZE; i++) {

		struct me_cell *cell;
		int cellid;
		
		// selection
		cellid = m_rand() % ME_POP_COUNT;
		cell = &project->cell[cellid];

		if (cell->state > 0)
			me_dna_copy(&project->tmp[i], &cell->best);

		// mutation
		if (cell->state == 0) {
			me_mutate(&project->tmp[i], 1);
			project->tmpi[i].parent = -1;
		}
		else {
			me_mutate(&project->tmp[i], 0);
			project->tmpi[i].parent = cellid;
		}
	}

	// evaluate
#if ME_TMP_SIZE > 1
	#pragma omp parallel for
#endif
	for (i = 0; i < ME_TMP_SIZE; i++)
		me_eval(i, project, &project->tmp[i], &project->tmpi[i]);

	for (i = 0; i < ME_TMP_SIZE; i++) {
		float fit = project->tmpi[i].fitness;
		if (project->best_score < 0)
			project->best_score = fit;
		else if(fit > project->best_score)
			project->best_score = fit;
	}

	// update map
	for (i = 0; i < ME_TMP_SIZE; i++) {

		int x, y;

		if (project->tmpi[i].fitness == 0)
			continue;

		// get position in the map
		me_pos(&project->tmp[i], &project->tmpi[i], &x, &y);

		// update cell
		if (x >= 0 && y >= 0 && x < ME_SIZEX && y < ME_SIZEY) {

			struct me_cell *cell;
			int mid;

			mid = y * ME_SIZEX + x;
			cell = &project->cell[mid];
			
			if (project->tmpi[i].fitness >= project->map[mid]) {
				
				if (cell->state == 0)
					cell->state = 1;

				project->map[mid] = project->tmpi[i].fitness;
				me_dna_copy(&cell->best, &project->tmp[i]);
			}
		}
	}

	project->generation++;
}
