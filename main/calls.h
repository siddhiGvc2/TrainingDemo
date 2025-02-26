#ifndef EXAMPLE_LVGL_DEMO_H
#define EXAMPLE_LVGL_DEMO_H

#include "lvgl.h"  // Ensure LVGL types are available

#ifdef __cplusplus
extern "C" {
#endif

// Function declaration
void example_lvgl_demo_ui(lv_disp_t *disp);

#ifdef __cplusplus
}
#endif

#endif // EXAMPLE_LVGL_DEMO_H

extern void wifi_main(void);
extern void lvgl_start(void);