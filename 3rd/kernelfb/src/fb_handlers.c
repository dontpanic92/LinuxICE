
#include "kernelfb.h"

extern struct kfb_ops kfb_ops_vga16fb, kfb_ops_rgb888_32;

struct kfb_ops* kfb_ops_var[KFB_OPS_MAX] = {&kfb_ops_vga16fb, &kfb_ops_rgb888_32};
