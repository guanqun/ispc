/*
  Copyright (c) 2010-2011, Intel Corporation
  All rights reserved.

  Redistribution and use in source and binary forms, with or without
  modification, are permitted provided that the following conditions are
  met:

    * Redistributions of source code must retain the above copyright
      notice, this list of conditions and the following disclaimer.

    * Redistributions in binary form must reproduce the above copyright
      notice, this list of conditions and the following disclaimer in the
      documentation and/or other materials provided with the distribution.

    * Neither the name of Intel Corporation nor the names of its
      contributors may be used to endorse or promote products derived from
      this software without specific prior written permission.


   THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS
   IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED
   TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A
   PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER
   OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
   EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
   PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
   PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
   LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
   NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
   SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.  
*/

#include <stdio.h>
#include <algorithm>
#include <stdlib.h>
#include <time.h>
#include "../timing.h"
#include "blitrow_ispc.h"
using namespace ispc;

extern void D32_A8_Color(void* dst, size_t dstRB,
                         const void* maskPtr, size_t maskRB,
                         uint32_t color, int width, int height);

extern void SkARGB32_A8_BlitMask_SSE2(void* dst, size_t dstRB,
                         const void* maskPtr, size_t maskRB,
                         uint32_t color, int width, int height);

extern void D32_A8_Opaque(void* dst, size_t dstRB,
                          const void* maskPtr, size_t maskRB,
                          uint32_t color, int width, int height);

typedef uint32_t SkPMColor;
typedef unsigned int U8CPU;

extern void S32A_Opaque_BlitRow32(uint32_t* dst,
                                  const uint32_t* src,
                                  int count, unsigned int alpha);

uint32_t getrand() {
    uint32_t result = 0;
    unsigned char x;

    x = rand() % 256;
    result |= x;
    x = rand() % 256;
    result |= (x << 8);
    x = rand() % 256;
    result |= (x << 16);
    x = 0xff;
    x = 0;
    x = rand() % 256;
    result |= (x << 24);

    return result;
}

extern SkPMColor SkPMSrcOver(SkPMColor src, SkPMColor dst);
extern "C" {
SkPMColor SkPMSrcOverExternal(SkPMColor src, SkPMColor dst);
}

void test_s32a() {
    //int count = 10000;
    int count = 1;
    SkPMColor *dst = new SkPMColor[count];
    SkPMColor *src = new SkPMColor[count];
    SkPMColor *dst2 = new SkPMColor[count];
    SkPMColor *back = new SkPMColor[count];

#if 0
    //set random data
    for (int i = 0; i < count; i++) {
        dst[i] = getrand();
        src[i] = getrand();

        dst2[i] = dst[i];

        back[i] = dst[i];
        //printf("[%d] 0x%x 0x%x 0x%x 0x%x\n", i, dst[i], dst2[i], src[i], SkPMSrcOver(src[i], dst[i]));
    }
#else
    back[0] = dst2[0] = dst[0] = 0x78fcd603;
    src[0] =  0xa8d051;

#endif

printf("-----------\n");
    S32A_Opaque_BlitRow32_ispc(dst2, src, count, 255);
    S32A_Opaque_BlitRow32(dst, src, count, 255);

    for (int i = 0; i < count; i++) {
        printf("[%d] 0x%x 0x%x\n", i, dst[i], dst2[i]);

        if (dst[i] != dst2[i]) {
            fprintf(stderr, "not correct!!! %d: 0x%0x 0x%0x\n", i, back[i], src[i]);
        }
    }

    fprintf(stderr, "done\n");
}

extern uint32_t SkAlphaMulQ(uint32_t c, unsigned scale);
int main() {
    srand(time(NULL));
    int dst_width = 400;
    int dst_height = 400;
    int width = 300;
    int height = 300;
    size_t dstRB = dst_width * 4;
    size_t maskRB = dst_width;
    
    uint32_t *buf = new uint32_t[dst_width * dst_height];
    uint8_t *mask = new uint8_t[dst_width * dst_height];

    //uint32_t color1 = 0x7755aa33;
    uint32_t color1 = 0xff55aa33;

///////////////////////////////////////////////////////////////////////////////
#if 0
    double minISPC = 1e30;
    for (int i = 0; i < 3; ++i) {
        reset_and_start_timer();
        D32_A8_Color_ispc((uint32_t*)buf, dstRB, (uint8_t*)mask, maskRB, color1, width, height);
        double dt = get_elapsed_mcycles();
        minISPC = std::min(minISPC, dt);
    }

    printf("[D32_A8_Color ispc]:\t\t[%.3f] million cycles\n", minISPC);
#endif
///////////////////////////////////////////////////////////////////////////////

#if 0
    double minISPC = 1e30;
    for (int i = 0; i < 3; ++i) {
        reset_and_start_timer();
        SkARGB32_A8_BlitMask_SSE2((void*)buf, dstRB, (const void*)mask, maskRB, color1, width, height);
        double dt = get_elapsed_mcycles();
        minISPC = std::min(minISPC, dt);
    }

    printf("[SkARGB32_A8_BlitMask_SSE2 SSE2]:\t\t[%.3f] million cycles\n", minISPC);
#endif

    // Clear out the buffer
    for (unsigned int i = 0; i < dst_width * dst_height; ++i)
        buf[i] = 0;

///////////////////////////////////////////////////////////////////////////////
#if 0
    double minSerial = 1e30;
    for (int i = 0; i < 3; ++i) {
        reset_and_start_timer();
        D32_A8_Color((void*)buf, dstRB, (const void*)mask, maskRB, color1, width, height);
        double dt = get_elapsed_mcycles();
        minSerial = std::min(minSerial, dt);
    }

    printf("[D32_A8_Color serial]:\t\t[%.3f] millon cycles\n", minSerial);
#endif
///////////////////////////////////////////////////////////////////////////////

    //printf("\t\t\t\t(%.2fx speedup from ISPC)\n", minSerial/minISPC);

    test_s32a();


    uint32_t x = 0x11111111;
    unsigned scale = 8;
    printf("xxxxx: %x\n", SkAlphaMulQ(x, 2));
    return 0;
}
