typedef unsigned int32 SkPMColor;
typedef unsigned int32 SkColor;
typedef unsigned int8 U8CPU;

extern "C" SkPMColor SkBlendARGB32(SkPMColor src, SkPMColor dst, U8CPU aa);
extern "C" uniform SkPMColor SkPreMultiplyColor(uniform SkColor c);

export void D32_A8_Color_ispc(uniform unsigned int32 device[], uniform int dstRB,
                         uniform unsigned int8 mask[], uniform int maskRB,
                         uniform unsigned int32 color,
                         uniform int width, uniform int height) {
    uniform unsigned int32 pmc = SkPreMultiplyColor(color);
    uniform int dstOffset = dstRB >> 2;

    for (uniform int i = 0; i < height; i++) {
        foreach (w = 0 ... width) {
            unsigned int8 aa = mask[dstOffset * i + w];
            unsigned int32 dd = device[maskRB * i + w];
            unsigned int32 x = SkBlendARGB32(pmc, dd, aa);
            device[w] = x;
        }
    }
}
