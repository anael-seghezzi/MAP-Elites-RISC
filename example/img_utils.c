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

#define STB_IMAGE_WRITE_IMPLEMENTATION
#define STBIW_ASSERT(x)
#include "stb_image_write.h"


static void m_image_float_to_srgb(struct m_image *dest, const struct m_image *src)
{
   float *src_data;
   uint8_t *dest_data;
   int i;

   m_image_create(dest, M_UBYTE, src->width, src->height, src->comp);

   src_data = (float *)src->data;
   dest_data = (uint8_t *)dest->data;

   for (i = 0; i < src->size; i++) {

      float s, f = M_CLAMP(src_data[i], 0, 1);

      if (f < 0.0031308)
         s = (12.92 * f);
      else
         s = 1.055 * powf(f, 1.0/2.4) - 0.055;

      dest_data[i] = M_CLAMP(s * 255, 0, 255);
   }
}

int img_save(const struct m_image *src, const char *filename)
{
	if (src->type == M_FLOAT) {
		struct m_image tmp = M_IMAGE_IDENTITY();
		m_image_float_to_srgb(&tmp, src);
		stbi_write_tga(filename, tmp.width, tmp.height, tmp.comp, tmp.data);
		m_image_destroy(&tmp);
		return 1;
	}
	else if (src->type == M_UBYTE) {
		stbi_write_tga(filename, src->width, src->height, src->comp, src->data);
		return 1;
	}
	return 0;
}
