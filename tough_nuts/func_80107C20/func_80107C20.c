#include "common.h"

typedef struct {
    s16 vscale[4];
    s16 vtrans[4];
} Vp;

void func_80107C20(Vp *vp, f32 ulx, f32 uly, f32 lrx, f32 lry) {
    f32 cx;
    f32 cy;

    cx = (ulx + lrx) * 0.5f;
    cy = (uly + lry) * 0.5f;

    vp->vscale[0] = (s16) ((lrx - cx) * 4.0f);
    vp->vtrans[2] = 0x1FF;
    vp->vscale[2] = vp->vtrans[2];
    vp->vscale[1] = (s16) ((lry - cy) * 4.0f);
    vp->vtrans[0] = (s16) (cx * 4.0f);
    vp->vtrans[1] = (s16) (cy * 4.0f);
}
