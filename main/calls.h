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
extern void uart_main(void);
extern void http_fota(void);
extern void uart_write_string(const char *);
extern void uart_write_string_ln(const char *);
extern void RestartDevice(void);

#include "esp_http_client.h"
#include "esp_https_ota.h"
extern esp_err_t _http_event_handler(esp_http_client_event_t *);