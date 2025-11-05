// Screen4 - WiFi Configuration Screen
// Created for ESP32 Sui Wallet

#ifndef UI_SCREEN4_H
#define UI_SCREEN4_H

#ifdef __cplusplus
extern "C" {
#endif

// SCREEN: ui_Screen4
extern void ui_Screen4_screen_init(void);
extern void ui_Screen4_screen_destroy(void);
extern lv_obj_t * ui_Screen4;
extern lv_obj_t * ui_LabelWiFiTitle;
extern lv_obj_t * ui_TextAreaSSID;
extern lv_obj_t * ui_TextAreaPassword;
extern lv_obj_t * ui_ButtonConnect;
extern lv_obj_t * ui_LabelConnect;
extern lv_obj_t * ui_ButtonBack4;
extern lv_obj_t * ui_LabelBack4;
extern lv_obj_t * ui_Keyboard;
extern lv_obj_t * ui_LabelSSID;
extern lv_obj_t * ui_LabelPassword;

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif
