typedef unsigned int32 SkPMColor;
typedef unsigned int32 SkColor;
typedef unsigned int8 U8CPU;

#define SkMulS16(x, y)  ((x) * (y))
#define SK_A32_SHIFT 24
#define SK_R32_SHIFT 16
#define SK_G32_SHIFT 8
#define SK_B32_SHIFT 0

static inline uniform U8CPU SkMulDiv255Round(uniform U8CPU a, uniform U8CPU b) {
    uniform unsigned int prod = SkMulS16(a, b) + 128;
    return (prod + (prod >> 8)) >> 8;
}       

static inline uniform SkPMColor SkPackARGB32(uniform U8CPU a, uniform U8CPU r, uniform U8CPU g, uniform U8CPU b) {
    return (a << SK_A32_SHIFT) | (r << SK_R32_SHIFT) |
           (g << SK_G32_SHIFT) | (b << SK_B32_SHIFT);
}

static inline uniform SkPMColor SkPremultiplyARGBInline(uniform U8CPU a, uniform U8CPU r, uniform U8CPU g, uniform U8CPU b) {
    if (a != 255) {
        r = SkMulDiv255Round(r, a);
        g = SkMulDiv255Round(g, a);
        b = SkMulDiv255Round(b, a);
    }
    return SkPackARGB32(a, r, g, b);
}       

/** return the alpha byte from a SkColor value */
#define SkColorGetA(color)      (((color) >> 24) & 0xFF)
/** return the red byte from a SkColor value */
#define SkColorGetR(color)      (((color) >> 16) & 0xFF)
/** return the green byte from a SkColor value */
#define SkColorGetG(color)      (((color) >>  8) & 0xFF)
/** return the blue byte from a SkColor value */
#define SkColorGetB(color)      (((color) >>  0) & 0xFF)

uniform SkPMColor SkPreMultiplyColor(uniform SkColor c) {
    return SkPremultiplyARGBInline(SkColorGetA(c), SkColorGetR(c),
                                   SkColorGetG(c), SkColorGetB(c));
}

#define SK_RESTRICT

static inline unsigned int8 SkAlpha255To256(U8CPU alpha) {
    // this one assues that blending on top of an opaque dst keeps it that way
    // even though it is less accurate than a+(a>>7) for non-opaque dsts
    return alpha + 1;
}

#define SkAlphaMul(value, alpha256)     (SkMulS16(value, alpha256) >> 8)

const uniform unsigned int32 gMask_00FF00FF = 0xFF00FF;

static inline unsigned int32 SkAlphaMulQ(unsigned int32 c, unsigned int8 scale) {
    unsigned int32 mask = gMask_00FF00FF;

    unsigned int32 rb = ((c & mask) * scale) >> 8;
    unsigned int32 ag = ((c >> 8) & mask) * scale;
    return (rb & mask) | (ag & ~mask);
}

#define SkGetPackedA32(packed)      ((unsigned int32)((packed) << (24 - SK_A32_SHIFT)) >> 24)
#define SkGetPackedR32(packed)      ((unsigned int32)((packed) << (24 - SK_R32_SHIFT)) >> 24)
#define SkGetPackedG32(packed)      ((unsigned int32)((packed) << (24 - SK_G32_SHIFT)) >> 24)
#define SkGetPackedB32(packed)      ((unsigned int32)((packed) << (24 - SK_B32_SHIFT)) >> 24)

static inline SkPMColor SkBlendARGB32(SkPMColor src, SkPMColor dst, U8CPU aa) {
    unsigned int8 src_scale = SkAlpha255To256(aa);
    unsigned int8 dst_scale = SkAlpha255To256(255 - SkAlphaMul(SkGetPackedA32(src), src_scale));
    
    return SkAlphaMulQ(src, src_scale) + SkAlphaMulQ(dst, dst_scale);
}       

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
