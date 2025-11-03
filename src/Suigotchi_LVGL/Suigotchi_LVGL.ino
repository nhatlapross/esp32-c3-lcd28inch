/*
 * Suigotchi with LVGL for ESP32-2424S012C
 * Displays your converted suigotchi image with touch button
 */

#include <lvgl.h>
#include <TFT_eSPI.h>

TFT_eSPI tft = TFT_eSPI();

// LVGL display buffer
static lv_disp_draw_buf_t draw_buf;
static lv_color_t buf1[240 * 10]; // 10 lines buffer

// Touch
#include "CST816S.h"
CST816S touch(4, 5, 1, 0);  // sda, scl, rst, irq

// Screens
lv_obj_t *screen_start;
lv_obj_t *screen_suigotchi;

// Forward declarations
void my_disp_flush(lv_disp_drv_t *disp, const lv_area_t *area, lv_color_t *color_p);
void my_touchpad_read(lv_indev_drv_t *indev_driver, lv_indev_data_t *data);
void create_start_screen();
void create_suigotchi_screen();
void start_button_clicked(lv_event_t *e);

// External image from converted.c
LV_IMG_DECLARE(suigotchi_resize);

void setup() {
  Serial.begin(115200);
  delay(1000);
  Serial.println("\n\n=== Suigotchi LVGL Starting ===");

  // Enable backlight FIRST
  pinMode(3, OUTPUT);
  digitalWrite(3, HIGH);
  Serial.println("Backlight ON");

  // Initialize display
  Serial.println("Initializing TFT...");
  tft.begin();
  tft.setRotation(0);
  tft.fillScreen(TFT_RED);  // Test - should see red!
  delay(1000);
  Serial.println("TFT initialized - you should see RED screen!");

  // Initialize touch
  Serial.println("Initializing touch...");
  touch.begin(FALLING);
  Serial.println("Touch initialized");

  // Initialize LVGL
  lv_init();

  // Setup display buffer
  lv_disp_draw_buf_init(&draw_buf, buf1, NULL, 240 * 10);

  // Display driver
  static lv_disp_drv_t disp_drv;
  lv_disp_drv_init(&disp_drv);
  disp_drv.hor_res = 240;
  disp_drv.ver_res = 240;
  disp_drv.flush_cb = my_disp_flush;
  disp_drv.draw_buf = &draw_buf;
  lv_disp_drv_register(&disp_drv);

  // Touch driver
  static lv_indev_drv_t indev_drv;
  lv_indev_drv_init(&indev_drv);
  indev_drv.type = LV_INDEV_TYPE_POINTER;
  indev_drv.read_cb = my_touchpad_read;
  lv_indev_drv_register(&indev_drv);

  Serial.println("LVGL initialized!");

  // Create UI screens
  create_start_screen();
  create_suigotchi_screen();

  // Load start screen
  lv_scr_load(screen_start);

  Serial.println("UI created! Touch START to begin");
}

void loop() {
  lv_timer_handler(); // LVGL task handler
  delay(5);
}

// ==================== Display Driver ====================
void my_disp_flush(lv_disp_drv_t *disp, const lv_area_t *area, lv_color_t *color_p) {
  uint32_t w = (area->x2 - area->x1 + 1);
  uint32_t h = (area->y2 - area->y1 + 1);

  tft.startWrite();
  tft.setAddrWindow(area->x1, area->y1, w, h);
  tft.pushColors((uint16_t *)&color_p->full, w * h, true);
  tft.endWrite();

  lv_disp_flush_ready(disp);
}

// ==================== Touch Driver ====================
void my_touchpad_read(lv_indev_drv_t *indev_driver, lv_indev_data_t *data) {
  if (touch.available()) {
    data->state = LV_INDEV_STATE_PR;
    data->point.x = touch.data.x;
    data->point.y = touch.data.y;
  } else {
    data->state = LV_INDEV_STATE_REL;
  }
}

// ==================== Start Screen ====================
void create_start_screen() {
  screen_start = lv_obj_create(NULL);
  lv_obj_set_style_bg_color(screen_start, lv_color_black(), 0);

  // Title "Suigotchi"
  lv_obj_t *label_title = lv_label_create(screen_start);
  lv_label_set_text(label_title, "Suigotchi");
  lv_obj_set_style_text_color(label_title, lv_color_hex(0x07FF), 0); // Cyan
  lv_obj_set_style_text_font(label_title, &lv_font_montserrat_14, 0);
  lv_obj_align(label_title, LV_ALIGN_TOP_MID, 0, 40);

  // Subtitle
  lv_obj_t *label_sub = lv_label_create(screen_start);
  lv_label_set_text(label_sub, "on Sui Blockchain");
  lv_obj_set_style_text_color(label_sub, lv_color_hex(0x7BEF), 0); // Gray
  lv_obj_align(label_sub, LV_ALIGN_TOP_MID, 0, 80);

  // START button
  lv_obj_t *btn = lv_btn_create(screen_start);
  lv_obj_set_size(btn, 120, 50);
  lv_obj_align(btn, LV_ALIGN_CENTER, 0, 40);
  lv_obj_set_style_bg_color(btn, lv_color_hex(0x07FF), 0); // Cyan
  lv_obj_add_event_cb(btn, start_button_clicked, LV_EVENT_CLICKED, NULL);

  lv_obj_t *label_btn = lv_label_create(btn);
  lv_label_set_text(label_btn, "START");
  lv_obj_set_style_text_color(label_btn, lv_color_black(), 0);
  lv_obj_center(label_btn);

  // Instructions
  lv_obj_t *label_inst = lv_label_create(screen_start);
  lv_label_set_text(label_inst, "Touch to start");
  lv_obj_set_style_text_color(label_inst, lv_color_hex(0x39E7), 0);
  lv_obj_align(label_inst, LV_ALIGN_BOTTOM_MID, 0, -20);
}

// ==================== Suigotchi Screen ====================
void create_suigotchi_screen() {
  screen_suigotchi = lv_obj_create(NULL);
  lv_obj_set_style_bg_color(screen_suigotchi, lv_color_black(), 0);

  // Display the converted suigotchi image!
  lv_obj_t *img = lv_img_create(screen_suigotchi);
  lv_img_set_src(img, &suigotchi_resize);
  lv_obj_center(img);

  // Back button (invisible, covers whole screen)
  lv_obj_t *btn_back = lv_btn_create(screen_suigotchi);
  lv_obj_set_size(btn_back, 240, 240);
  lv_obj_set_style_bg_opa(btn_back, LV_OPA_TRANSP, 0);
  lv_obj_set_style_border_width(btn_back, 0, 0);
  lv_obj_align(btn_back, LV_ALIGN_CENTER, 0, 0);
  lv_obj_add_event_cb(btn_back, [](lv_event_t *e) {
    lv_scr_load(screen_start);
    Serial.println("Back to start screen");
  }, LV_EVENT_CLICKED, NULL);
}

// ==================== Button Event ====================
void start_button_clicked(lv_event_t *e) {
  Serial.println("START button clicked!");
  lv_scr_load(screen_suigotchi);
}
