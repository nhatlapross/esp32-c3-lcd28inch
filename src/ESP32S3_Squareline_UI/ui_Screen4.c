// Screen4 - WiFi Configuration Screen
// Created for ESP32 Sui Wallet

#include "ui.h"

lv_obj_t * ui_Screen4 = NULL;
lv_obj_t * ui_LabelWiFiTitle = NULL;
lv_obj_t * ui_TextAreaSSID = NULL;
lv_obj_t * ui_TextAreaPassword = NULL;
lv_obj_t * ui_ButtonConnect = NULL;
lv_obj_t * ui_LabelConnect = NULL;
lv_obj_t * ui_ButtonBack4 = NULL;
lv_obj_t * ui_LabelBack4 = NULL;
lv_obj_t * ui_Keyboard = NULL;
lv_obj_t * ui_LabelSSID = NULL;
lv_obj_t * ui_LabelPassword = NULL;

// Current active text area
lv_obj_t * activeTextArea = NULL;

// External WiFi connect function
extern void reconnectWiFiWithCredentials(const char* ssid, const char* password);

// Event functions
void ui_event_TextAreaSSID(lv_event_t * e)
{
    lv_event_code_t event_code = lv_event_get_code(e);

    if(event_code == LV_EVENT_CLICKED || event_code == LV_EVENT_FOCUSED) {
        activeTextArea = ui_TextAreaSSID;
        lv_keyboard_set_textarea(ui_Keyboard, ui_TextAreaSSID);
    }
}

void ui_event_TextAreaPassword(lv_event_t * e)
{
    lv_event_code_t event_code = lv_event_get_code(e);

    if(event_code == LV_EVENT_CLICKED || event_code == LV_EVENT_FOCUSED) {
        activeTextArea = ui_TextAreaPassword;
        lv_keyboard_set_textarea(ui_Keyboard, ui_TextAreaPassword);
    }
}

void ui_event_ButtonConnect(lv_event_t * e)
{
    lv_event_code_t event_code = lv_event_get_code(e);

    if(event_code == LV_EVENT_CLICKED) {
        // Get SSID and Password from text areas
        const char* ssid = lv_textarea_get_text(ui_TextAreaSSID);
        const char* password = lv_textarea_get_text(ui_TextAreaPassword);

        // Call reconnect function
        reconnectWiFiWithCredentials(ssid, password);
    }
}

void ui_event_ButtonBack4(lv_event_t * e)
{
    lv_event_code_t event_code = lv_event_get_code(e);

    if(event_code == LV_EVENT_CLICKED) {
        _ui_screen_change(&ui_Screen1, LV_SCR_LOAD_ANIM_FADE_ON, 300, 0, &ui_Screen1_screen_init);
    }
}

void ui_event_Screen4(lv_event_t * e)
{
    lv_event_code_t event_code = lv_event_get_code(e);

    if(event_code == LV_EVENT_GESTURE &&  lv_indev_get_gesture_dir(lv_indev_get_act()) == LV_DIR_TOP) {
        lv_indev_wait_release(lv_indev_get_act());
        _ui_screen_change(&ui_Screen1, LV_SCR_LOAD_ANIM_MOVE_TOP, 100, 0, &ui_Screen1_screen_init);
    }
}

// Build functions
void ui_Screen4_screen_init(void)
{
    ui_Screen4 = lv_obj_create(NULL);
    lv_obj_clear_flag(ui_Screen4, LV_OBJ_FLAG_SCROLLABLE);

    // Title
    ui_LabelWiFiTitle = lv_label_create(ui_Screen4);
    lv_obj_set_width(ui_LabelWiFiTitle, LV_SIZE_CONTENT);
    lv_obj_set_height(ui_LabelWiFiTitle, LV_SIZE_CONTENT);
    lv_obj_set_x(ui_LabelWiFiTitle, 0);
    lv_obj_set_y(ui_LabelWiFiTitle, -100);
    lv_obj_set_align(ui_LabelWiFiTitle, LV_ALIGN_CENTER);
    lv_label_set_text(ui_LabelWiFiTitle, "WiFi Settings");
    lv_obj_set_style_text_font(ui_LabelWiFiTitle, &lv_font_montserrat_18, LV_PART_MAIN | LV_STATE_DEFAULT);

    // SSID Label
    ui_LabelSSID = lv_label_create(ui_Screen4);
    lv_obj_set_width(ui_LabelSSID, LV_SIZE_CONTENT);
    lv_obj_set_height(ui_LabelSSID, LV_SIZE_CONTENT);
    lv_obj_set_x(ui_LabelSSID, -70);
    lv_obj_set_y(ui_LabelSSID, -70);
    lv_obj_set_align(ui_LabelSSID, LV_ALIGN_CENTER);
    lv_label_set_text(ui_LabelSSID, "SSID:");
    lv_obj_set_style_text_font(ui_LabelSSID, &lv_font_montserrat_12, LV_PART_MAIN | LV_STATE_DEFAULT);

    // SSID Text Area
    ui_TextAreaSSID = lv_textarea_create(ui_Screen4);
    lv_obj_set_width(ui_TextAreaSSID, 150);
    lv_obj_set_height(ui_TextAreaSSID, 30);
    lv_obj_set_x(ui_TextAreaSSID, 20);
    lv_obj_set_y(ui_TextAreaSSID, -70);
    lv_obj_set_align(ui_TextAreaSSID, LV_ALIGN_CENTER);
    lv_textarea_set_placeholder_text(ui_TextAreaSSID, "WiFi Name");
    lv_textarea_set_one_line(ui_TextAreaSSID, true);
    lv_textarea_set_max_length(ui_TextAreaSSID, 32);

    // Password Label
    ui_LabelPassword = lv_label_create(ui_Screen4);
    lv_obj_set_width(ui_LabelPassword, LV_SIZE_CONTENT);
    lv_obj_set_height(ui_LabelPassword, LV_SIZE_CONTENT);
    lv_obj_set_x(ui_LabelPassword, -60);
    lv_obj_set_y(ui_LabelPassword, -35);
    lv_obj_set_align(ui_LabelPassword, LV_ALIGN_CENTER);
    lv_label_set_text(ui_LabelPassword, "Pass:");
    lv_obj_set_style_text_font(ui_LabelPassword, &lv_font_montserrat_12, LV_PART_MAIN | LV_STATE_DEFAULT);

    // Password Text Area
    ui_TextAreaPassword = lv_textarea_create(ui_Screen4);
    lv_obj_set_width(ui_TextAreaPassword, 150);
    lv_obj_set_height(ui_TextAreaPassword, 30);
    lv_obj_set_x(ui_TextAreaPassword, 20);
    lv_obj_set_y(ui_TextAreaPassword, -35);
    lv_obj_set_align(ui_TextAreaPassword, LV_ALIGN_CENTER);
    lv_textarea_set_placeholder_text(ui_TextAreaPassword, "Password");
    lv_textarea_set_one_line(ui_TextAreaPassword, true);
    lv_textarea_set_password_mode(ui_TextAreaPassword, true);
    lv_textarea_set_max_length(ui_TextAreaPassword, 64);

    // Keyboard
    ui_Keyboard = lv_keyboard_create(ui_Screen4);
    lv_obj_set_size(ui_Keyboard, 240, 120);
    lv_obj_set_x(ui_Keyboard, 0);
    lv_obj_set_y(ui_Keyboard, 60);
    lv_obj_set_align(ui_Keyboard, LV_ALIGN_CENTER);
    lv_keyboard_set_mode(ui_Keyboard, LV_KEYBOARD_MODE_TEXT_LOWER);

    // Connect Button
    ui_ButtonConnect = lv_btn_create(ui_Screen4);
    lv_obj_set_width(ui_ButtonConnect, 80);
    lv_obj_set_height(ui_ButtonConnect, 25);
    lv_obj_set_x(ui_ButtonConnect, -40);
    lv_obj_set_y(ui_ButtonConnect, -5);
    lv_obj_set_align(ui_ButtonConnect, LV_ALIGN_CENTER);
    lv_obj_set_style_bg_color(ui_ButtonConnect, lv_color_hex(0x00AA00), LV_PART_MAIN | LV_STATE_DEFAULT);

    ui_LabelConnect = lv_label_create(ui_ButtonConnect);
    lv_obj_set_align(ui_LabelConnect, LV_ALIGN_CENTER);
    lv_label_set_text(ui_LabelConnect, "Connect");

    // Back Button
    ui_ButtonBack4 = lv_btn_create(ui_Screen4);
    lv_obj_set_width(ui_ButtonBack4, 70);
    lv_obj_set_height(ui_ButtonBack4, 25);
    lv_obj_set_x(ui_ButtonBack4, 45);
    lv_obj_set_y(ui_ButtonBack4, -5);
    lv_obj_set_align(ui_ButtonBack4, LV_ALIGN_CENTER);

    ui_LabelBack4 = lv_label_create(ui_ButtonBack4);
    lv_obj_set_align(ui_LabelBack4, LV_ALIGN_CENTER);
    lv_label_set_text(ui_LabelBack4, "Back");

    // Set default keyboard target to SSID
    lv_keyboard_set_textarea(ui_Keyboard, ui_TextAreaSSID);
    activeTextArea = ui_TextAreaSSID;

    // Add event callbacks
    lv_obj_add_event_cb(ui_Screen4, ui_event_Screen4, LV_EVENT_ALL, NULL);
    lv_obj_add_event_cb(ui_TextAreaSSID, ui_event_TextAreaSSID, LV_EVENT_ALL, NULL);
    lv_obj_add_event_cb(ui_TextAreaPassword, ui_event_TextAreaPassword, LV_EVENT_ALL, NULL);
    lv_obj_add_event_cb(ui_ButtonConnect, ui_event_ButtonConnect, LV_EVENT_ALL, NULL);
    lv_obj_add_event_cb(ui_ButtonBack4, ui_event_ButtonBack4, LV_EVENT_ALL, NULL);
}

void ui_Screen4_screen_destroy(void)
{
    if(ui_Screen4) lv_obj_del(ui_Screen4);

    // NULL screen variables
    ui_Screen4 = NULL;
    ui_LabelWiFiTitle = NULL;
    ui_TextAreaSSID = NULL;
    ui_TextAreaPassword = NULL;
    ui_ButtonConnect = NULL;
    ui_LabelConnect = NULL;
    ui_ButtonBack4 = NULL;
    ui_LabelBack4 = NULL;
    ui_Keyboard = NULL;
    ui_LabelSSID = NULL;
    ui_LabelPassword = NULL;
}
