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

int main() {
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
#if 1
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
    double minSerial = 1e30;
    for (int i = 0; i < 3; ++i) {
        reset_and_start_timer();
        D32_A8_Color((void*)buf, dstRB, (const void*)mask, maskRB, color1, width, height);
        double dt = get_elapsed_mcycles();
        minSerial = std::min(minSerial, dt);
    }

    printf("[D32_A8_Color serial]:\t\t[%.3f] millon cycles\n", minSerial);
///////////////////////////////////////////////////////////////////////////////

    printf("\t\t\t\t(%.2fx speedup from ISPC)\n", minSerial/minISPC);

    return 0;
}
