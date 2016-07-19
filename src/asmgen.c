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

// simplified MIPS-like instruction set
// see https://en.wikipedia.org/wiki/MIPS_instruction_set


#ifndef ASMGEN_REG
#define ASMGEN_REG 8
#endif

#define ASMGEN_NOTHING     0

#define ASMGEN_LOAD        1
#define ASMGEN_STORE       2
#define ASMGEN_ADD         3
#define ASMGEN_ADDI        4
#define ASMGEN_SUB         5
#define ASMGEN_SHIFTL      6
#define ASMGEN_SHIFTR      7
#define ASMGEN_AND         8
#define ASMGEN_OR          9
#define ASMGEN_XOR         10
#define ASMGEN_LESS        11
#define ASMGEN_BRANCH      12
#define ASMGEN_NBRANCH     13
#define ASMGEN_GOTO        14

#define ASMGEN_LOADF       15
#define ASMGEN_STOREF      16
#define ASMGEN_ADDF        17
#define ASMGEN_SUBF        18
#define ASMGEN_MULF        19
#define ASMGEN_DIVF        20
#define ASMGEN_LESSF       21
#define ASMGEN_MINF        22
#define ASMGEN_MAXF        23
#define ASMGEN_SINF        24
#define ASMGEN_COSF        25
#define ASMGEN_SQRTF       26

const char *amsgen_i_name[26] = {
   "load",
   "store",
   "add",
   "addi",
   "sub",
   "shiftl",
   "shiftr",
   "and",
   "or",
   "xor",
   "less",
   "branch",
   "nbranch",
   "goto",
   "loadf",
   "storef",
   "addf",
   "subf",
   "mulf",
   "divf",
   "lessf",
   "minf",
   "maxf",
   "sinf",
   "cosf",
   "sqrtf"
};


struct asmgeni
{
   int o;
   char i, a, b, c;
};

struct asmgen
{
   float regf[ASMGEN_REG];
   int regi[ASMGEN_REG];

   int *mem;
   int mem_size;
   int mem_readonly;

   struct asmgeni *code;
   int code_size;
   int start;
};


void asmgen_create(struct asmgen *dest, int mem_size, int code_size, int readonly)
{
   memset(dest->regf, 0, sizeof(dest->regf));
   memset(dest->regi, 0, sizeof(dest->regi));
   dest->mem = (int *)calloc(mem_size, sizeof(int));
   dest->code = (struct asmgeni *)calloc(code_size, sizeof(struct asmgeni));
   dest->mem_size = mem_size;
   dest->mem_readonly = readonly;
   dest->code_size = code_size;
   dest->start = 0;
}

void asmgen_destroy(struct asmgen *dest)
{
   free(dest->mem);
   free(dest->code);
}

void asmgen_copy(struct asmgen *dest, struct asmgen *src)
{
   if (src->mem_readonly > 0)
      memcpy(dest->mem, src->mem, src->mem_readonly * sizeof(int));
   memcpy(dest->code, src->code, dest->code_size * sizeof(struct asmgeni));
}

void asmgen_clear_reg(struct asmgen *dest)
{
   memset(dest->regf, 0, sizeof(dest->regf));
   memset(dest->regi, 0, sizeof(dest->regi));
}

void asmgen_clear_mem(struct asmgen *dest)
{
   memset(dest->mem + dest->mem_readonly, 0, (dest->mem_size - dest->mem_readonly) * sizeof(int));
}

int asmgen_line_count(struct asmgen *src)
{
   int i, count = 0;
   
   for (i = 0; i < src->code_size; i++) {
      if (src->code[i].i != ASMGEN_NOTHING)
         count++;
   }

   return count;
}

int asmgen_run(struct asmgen *src, int max_call)
{
   struct asmgeni *code;
   int b;
   int cc = 0;
   int i = src->start;

   while (i >= 0 && i < src->code_size) {

      code = &src->code[i];

      switch (code->i) {

      default:
      case ASMGEN_NOTHING:
         i++;
         break;

      case ASMGEN_LOAD:
         b = src->regi[code->b] + code->o;
         if (b >= 0 && b < src->mem_size)
            src->regi[code->a] = src->mem[b];
         i++;
         cc++;
         break;

      case ASMGEN_STORE:
         b = src->regi[code->b] + code->o;
         if (b >= src->mem_readonly && b < src->mem_size)
            src->mem[b] = src->regi[code->c];
         i++;
         cc++;
         break;

      case ASMGEN_ADD:
         src->regi[code->a] = src->regi[code->b] + src->regi[code->c];
         i++;
         cc++;
         break;

      case ASMGEN_ADDI:
         src->regi[code->a] = src->regi[code->b] + code->o;
         i++;
         cc++;
         break;

      case ASMGEN_SUB:
         src->regi[code->a] = src->regi[code->b] - src->regi[code->c];
         i++;
       cc++;
         break;

      case ASMGEN_SHIFTL:
         src->regi[code->a] = src->regi[code->b] << 1;
         i++;
         cc++;
         break;

      case ASMGEN_SHIFTR:
         src->regi[code->a] = src->regi[code->b] >> 1;
         i++;
         cc++;
         break;

      case ASMGEN_AND:
         src->regi[code->a] = (src->regi[code->b] & src->regi[code->c]);
         i++;
         cc++;
         break;

      case ASMGEN_OR:
         src->regi[code->a] = (src->regi[code->b] | src->regi[code->c]);
         i++;
         cc++;
         break;

      case ASMGEN_XOR:
         src->regi[code->a] = (src->regi[code->b] ^ src->regi[code->c]);
         i++;
         cc++;
         break;

      case ASMGEN_LESS:
         src->regi[code->a] = (src->regi[code->b] < src->regi[code->c]);
         i++;
         cc++;
         break;

      case ASMGEN_BRANCH:
         if (src->regi[code->b] == src->regi[code->c])
            i = i + 1 + code->o;
         else
            i++;
         cc++;
         break;

      case ASMGEN_NBRANCH:
         if (src->regi[code->b] != src->regi[code->c])
            i = i + 1 + code->o;
         else
            i++;
         cc++;
         break;

      case ASMGEN_GOTO:
         i = code->o;
         cc++;
         break;

      case ASMGEN_LOADF:
         b = src->regi[code->b] + code->o;
         if (b >= 0 && b < src->mem_size)
            src->regf[code->a] = *((float *)&src->mem[b]);
         i++;
         cc++;
         break;

      case ASMGEN_STOREF:
         b = src->regi[code->b] + code->o;
         if (b >= src->mem_readonly && b < src->mem_size)
            *((float *)&src->mem[b]) = src->regf[code->c];
         i++;
         cc++;
         break;

      case ASMGEN_ADDF:
         src->regf[code->a] = src->regf[code->b] + src->regf[code->c];
         i++;
         cc++;
         break;

      case ASMGEN_SUBF:
         src->regf[code->a] = src->regf[code->b] - src->regf[code->c];
         i++;
         cc++;
         break;

      case ASMGEN_MULF:
         src->regf[code->a] = src->regf[code->b] * src->regf[code->c];
         i++;
         cc++;
         break;

      case ASMGEN_DIVF:
         if (src->regf[code->c] != 0)
            src->regf[code->a] = src->regf[code->b] / src->regf[code->c];
         i++;
         cc++;
         break;

      case ASMGEN_MINF:
         src->regf[code->a] = src->regf[code->b] < src->regf[code->c] ? src->regf[code->b] : src->regf[code->c];
         i++;
         cc++;
         break;

      case ASMGEN_MAXF:
         src->regf[code->a] = src->regf[code->b] > src->regf[code->c] ? src->regf[code->b] : src->regf[code->c];
         i++;
         cc++;
         break;

      case ASMGEN_LESSF:
         src->regi[code->a] = (src->regf[code->b] < src->regf[code->c]);
         i++;
         cc++;
         break;

      case ASMGEN_SINF:
         src->regf[code->a] = sinf(src->regf[code->b]);
         i++;
         cc++;
         break;

      case ASMGEN_COSF:
         src->regf[code->a] = cosf(src->regf[code->b]);
         i++;
         cc++;
         break;

      case ASMGEN_SQRTF:
       src->regf[code->a] = sqrtf(src->regf[code->b] < 0 ? -src->regf[code->b] : src->regf[code->b]);
         i++;
         cc++;
         break;
      }

     if (cc == max_call)
        return max_call;
   }

   return cc;
}

void asmgen_write(struct asmgen *src, FILE *file)
{
   fwrite(src->code, sizeof(struct asmgeni), src->code_size, file);
   if (src->mem_readonly > 0)
      fwrite(src->mem, sizeof(int), src->mem_readonly, file);
   fwrite(&src->start, sizeof(int), 1, file);
}

void asmgen_read(struct asmgen *dest, FILE *file)
{
   fread(dest->code, sizeof(struct asmgeni), dest->code_size, file);
   if (dest->mem_readonly > 0)
      fread(dest->mem, sizeof(int), dest->mem_readonly, file);
   fread(&dest->start, sizeof(int), 1, file);
}

int asmgen_save(struct asmgen *src, const char *filename)
{
   FILE *file = fopen(filename, "wb");
   if(! file) {
      printf("ASMGEN ERROR: can't write file %s\n", filename);
      return 0;
   }

   fwrite(&src->code_size, sizeof(int), 1, file);
   fwrite(&src->mem_size, sizeof(int), 1, file);
   fwrite(&src->mem_readonly, sizeof(int), 1, file);

   asmgen_write(src, file);

   fclose(file);
   return 1;
}

int asmgen_load(struct asmgen *dest, const char *filename)
{
   FILE *file = fopen(filename, "rb");
   if(! file) {
      printf("ASMGEN ERROR: can't read file %s\n", filename);
      return 0;
   }

   fread(&dest->code_size, sizeof(int), 1, file);
   fread(&dest->mem_size, sizeof(int), 1, file);
   fread(&dest->mem_readonly, sizeof(int), 1, file);

   dest->mem = (int *)calloc(dest->mem_size, sizeof(int));
   dest->code = (struct asmgeni *)calloc(dest->code_size, sizeof(struct asmgeni));

   asmgen_read(dest, file);

   fclose(file);
   return 1;
}

int asmgen_save_text(struct asmgen *src, const char *filename)
{
#define PAB  fprintf(file, "($%d, $%d)", src->code[i].a, src->code[i].b);
#define PABC fprintf(file, "($%d, $%d, $%d)", src->code[i].a, src->code[i].b, src->code[i].c);
#define PABI fprintf(file, "($%d, $%d, %d)", src->code[i].a, src->code[i].b, src->code[i].o);
#define PL   fprintf(file, "($%d, m[$%d + %d])", src->code[i].a, src->code[i].b, src->code[i].o);
#define PS   fprintf(file, "(m[$%d + %d], $%d)", src->code[i].b, src->code[i].o, src->code[i].c);
#define PBR  fprintf(file, "($%d, $%d, %d)", src->code[i].b, src->code[i].c, src->code[i].o);
#define PJ   fprintf(file, "(%d)", src->code[i].o);

   FILE *file;
   int i;

   file = fopen(filename, "wt");
   if(! file) {
      printf("ASMGEN ERROR: can't write file %s\n", filename);
      return 0;
   }

   fprintf(file, "start = %d\n\n", src->start);
   
   for (i = 0; i < src->code_size; i++) {

      if (src->code[i].i > 0) {

         fprintf(file, "%d: %s ", i, amsgen_i_name[src->code[i].i - 1]);

         switch (src->code[i].i)
         {
         case ASMGEN_LOAD:
         case ASMGEN_LOADF:
            PL break;
         case ASMGEN_STORE:
         case ASMGEN_STOREF:
            PS break;
         case ASMGEN_BRANCH:
         case ASMGEN_NBRANCH:
            PBR break;
         case ASMGEN_GOTO:
            PJ break;
         case ASMGEN_ADDI:
            PABI break;
         case ASMGEN_SHIFTL:
         case ASMGEN_SHIFTR:
         case ASMGEN_SINF:
         case ASMGEN_COSF:
         case ASMGEN_SQRTF:
            PAB break;
         default:
            PABC break;
         }

         fprintf(file, "\n");
      }
   }

   fclose(file);
   return 1;
}
