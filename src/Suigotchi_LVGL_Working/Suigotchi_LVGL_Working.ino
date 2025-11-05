/*
 * Suigotchi with LVGL using LCD_1in28 driver (the working one!)
 */

#include <Arduino.h>
#include "LCD_Test.h"
#include <lvgl.h>

UWORD Imagesize = LCD_1IN28_HEIGHT * LCD_1IN28_WIDTH * 2;
UWORD *BlackImage;
CST816S touch(4, 5, 1, 0);

// LVGL buffer
static lv_disp_draw_buf_t draw_buf;
static lv_color_t lvgl_buf[240 * 10];

lv_obj_t *screen_start;
lv_obj_t *screen_suigotchi;

LV_IMG_DECLARE(suigotchi_resize);

void my_disp_flush(lv_disp_drv_t *disp, const lv_area_t *area, lv_color_t *color_p);
void my_touchpad_read(lv_indev_drv_t *indev_driver, lv_indev_data_t *data);
void create_start_screen();
void create_suigotchi_screen();

void setup()
{
    Serial.begin(115200);
    delay(1000);
    Serial.println("\n=== Suigotchi LVGL ===");

    touch.begin(FALLING);

    if ((BlackImage = (UWORD *)malloc(Imagesize)) == NULL){
        Serial.println("Memory fail!");
        while(1);
    }

    if (DEV_Module_Init() != 0) {
        Serial.println("Init fail!");
        while(1);
    }

    LCD_1IN28_Init(HORIZONTAL);
    LCD_1IN28_Clear(BLACK);

    // Initialize Paint library for the buffer
    Paint_NewImage((UBYTE *)BlackImage, LCD_1IN28.WIDTH, LCD_1IN28.HEIGHT, 0, BLACK);
    Paint_SetScale(65);
    Paint_SetRotate(ROTATE_0);
    Paint_Clear(BLACK);

    delay(500);
    Serial.println("LCD works!");

    // Init LVGL
    lv_init();
    lv_disp_draw_buf_init(&draw_buf, lvgl_buf, NULL, 240 * 10);

    static lv_disp_drv_t disp_drv;
    lv_disp_drv_init(&disp_drv);
    disp_drv.hor_res = 240;
    disp_drv.ver_res = 240;
    disp_drv.flush_cb = my_disp_flush;
    disp_drv.draw_buf = &draw_buf;
    lv_disp_drv_register(&disp_drv);

    static lv_indev_drv_t indev_drv;
    lv_indev_drv_init(&indev_drv);
    indev_drv.type = LV_INDEV_TYPE_POINTER;
    indev_drv.read_cb = my_touchpad_read;
    lv_indev_drv_register(&indev_drv);

    create_start_screen();
    create_suigotchi_screen();
    lv_scr_load(screen_start);

    Serial.println("Ready!");
}

void loop()
{
    lv_timer_handler();
    delay(5);
}

void my_disp_flush(lv_disp_drv_t *disp, const lv_area_t *area, lv_color_t *color_p) {
    uint32_t w = (area->x2 - area->x1 + 1);
    uint32_t h = (area->y2 - area->y1 + 1);

    // Copy LVGL pixels directly to BlackImage buffer
    lv_color_t *src = color_p;

    for(uint32_t y = 0; y < h; y++) {
        uint32_t destIndex = ((area->y1 + y) * 240) + area->x1;
        for(uint32_t x = 0; x < w; x++) {
            // LV_COLOR_16_SWAP might be needed depending on endianness
            BlackImage[destIndex + x] = src->full;
            src++;
        }
    }

    // Only update the changed region
    LCD_1IN28_DisplayWindows(area->x1, area->y1, area->x2 + 1, area->y2 + 1, BlackImage);

    lv_disp_flush_ready(disp);
}

void my_touchpad_read(lv_indev_drv_t *indev_driver, lv_indev_data_t *data) {
    static bool was_touched = false;

    if (touch.available()) {
        data->state = LV_INDEV_STATE_PR;
        data->point.x = touch.data.x;
        data->point.y = touch.data.y;
        was_touched = true;
        Serial.printf("Touch: %d, %d\n", touch.data.x, touch.data.y);
    } else {
        data->state = LV_INDEV_STATE_REL;
        if (was_touched) {
            Serial.println("Touch released");
            was_touched = false;
        }
    }
}

void create_start_screen() {
    screen_start = lv_obj_create(NULL);
    lv_obj_set_style_bg_color(screen_start, lv_color_black(), 0);

    // Title - baby blue color
    lv_obj_t *label = lv_label_create(screen_start);
    lv_label_set_text(label, "Suigotchi");
    lv_obj_set_style_text_color(label, lv_color_hex(0xADFF), 0); // Light blue
    lv_obj_align(label, LV_ALIGN_TOP_MID, 0, 60);

    // START button - baby blue
    lv_obj_t *btn = lv_btn_create(screen_start);
    lv_obj_set_size(btn, 120, 50);
    lv_obj_align(btn, LV_ALIGN_CENTER, 0, 20);
    lv_obj_set_style_bg_color(btn, lv_color_hex(0x87CE), 0); // Baby blue
    lv_obj_set_style_radius(btn, 8, 0);
    lv_obj_clear_flag(btn, LV_OBJ_FLAG_CLICK_FOCUSABLE); // Remove focus outline

    lv_obj_add_event_cb(btn, [](lv_event_t *e) {
        Serial.println("=== START BUTTON CLICKED! ===");
        lv_scr_load(screen_suigotchi);
    }, LV_EVENT_CLICKED, NULL);

    lv_obj_t *label_btn = lv_label_create(btn);
    lv_label_set_text(label_btn, "START");
    lv_obj_set_style_text_color(label_btn, lv_color_black(), 0);
    lv_obj_center(label_btn);

    Serial.println("Start screen created");
}

void create_suigotchi_screen() {
    screen_suigotchi = lv_obj_create(NULL);
    lv_obj_set_style_bg_color(screen_suigotchi, lv_color_black(), 0);

    lv_obj_t *img = lv_img_create(screen_suigotchi);
    lv_img_set_src(img, &suigotchi_resize);
    lv_obj_center(img);

    lv_obj_add_event_cb(screen_suigotchi, [](lv_event_t *e) {
        lv_scr_load(screen_start);
    }, LV_EVENT_CLICKED, NULL);
}
