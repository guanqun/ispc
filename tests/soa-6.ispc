
struct Point { float x, y, z; };

export uniform int width() { return programCount; }

export void f_fu(uniform float RET[], uniform float aFOO[], uniform float b) {
    float a = aFOO[programIndex]; 

    soa<8> Point pts[8];
    foreach (i = 0 ... 64) {
        pts[i].x = -42;
        pts[i].y = 0;
        pts[i].z = 0;
    }

    Point pv = { a, b, -a };
    pts[8+programIndex] = pv;

    RET[programIndex] = pts[6+programIndex].x;
}

export void result(uniform float RET[]) {
    RET[programIndex] = (1 + programIndex - 2);
    RET[0] = RET[1] = -42;
}
