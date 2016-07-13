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


typedef struct me_dna
{
	float *genome;
	int size;
} me_dna;


#include "mapelites.c"


void me_dna_create(me_dna *dest, int size)
{
	dest->genome = (float *)calloc(size, sizeof(float));
	dest->size = size;
}

void me_dna_destroy(me_dna *src)
{
	free(src->genome);
}

void me_dna_copy(me_dna *dest, me_dna *src)
{
	memcpy(dest->genome, src->genome, sizeof(float) * src->size);
	dest->size = src->size;
}

void me_create(struct me_project *project, int size)
{
	int i;

	me_create_internal(project);
	
	for (i = 0; i < ME_POP_COUNT; i++)
		me_dna_create(&project->cell[i].best, size);

	for (i = 0; i < ME_TMP_SIZE; i++)
		me_dna_create(&project->tmp[i], size);
}

void me_destroy(struct me_project *project)
{
	int i;

	for (i = 0; i < ME_POP_COUNT; i++)
		me_dna_destroy(&project->cell[i].best);

	for (i = 0; i < ME_TMP_SIZE; i++)
		me_dna_destroy(&project->tmp[i]);

	me_destroy_internal(project);
}
