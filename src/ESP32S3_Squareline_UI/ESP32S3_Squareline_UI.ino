#include <lvgl.h>
#include "lv_conf.h"
#include <demos/lv_demos.h>
#include "LCD_1in28.h"
#include "DEV_Config.h"
#include "CST816S.h"
#include "ui.h"  // SquareLine Studio UI

#define EXAMPLE_LVGL_TICK_PERIOD_MS    2

/*Change to your screen resolution*/
static const uint16_t screenWidth  = 240;
static const uint16_t screenHeight = 240;

static lv_disp_draw_buf_t draw_buf;
static lv_color_t buf[ screenWidth * screenHeight / 10 ];

CST816S touch(6, 7, 13, 5);  // sda, scl, rst, irq

// Buffer for custom LCD driver (allocated in PSRAM)
// This is required by LCD_1in28.cpp
UWORD *BlackImage = NULL;

#if LV_USE_LOG != 0
/* Serial debugging */
void my_print(const char * buf)
{
    Serial.printf(buf);
    Serial.flush();
}
#endif

/* Display flushing using custom LCD driver */
void my_disp_flush( lv_disp_drv_t *disp_drv, const lv_area_t *area, lv_color_t *color_p )
{
    uint32_t w = ( area->x2 - area->x1 + 1 );
    uint32_t h = ( area->y2 - area->y1 + 1 );

    // Copy LVGL buffer to BlackImage at correct position (full-screen buffer layout)
    // BlackImage is 240x240, copy the dirty region into it
    uint16_t *src = (uint16_t *)&color_p->full;

    for(uint32_t row = 0; row < h; row++) {
        uint32_t y = area->y1 + row;
        uint32_t dst_offset = area->x1 + y * 240;  // BlackImage is 240 width
        uint16_t *src_row = &src[row * w];

        // Copy and swap bytes for this row
        for(uint32_t col = 0; col < w; col++) {
            uint16_t color = src_row[col];
            BlackImage[dst_offset + col] = (color >> 8) | (color << 8);
        }
    }

    // Use original LCD_1IN28_DisplayWindows function
    // Note: LVGL area is inclusive (x2, y2 are last pixels)
    // But DisplayWindows expects exclusive end (so +1)
    LCD_1IN28_DisplayWindows(area->x1, area->y1, area->x2 + 1, area->y2 + 1, BlackImage);

    lv_disp_flush_ready( disp_drv );
}

void example_increase_lvgl_tick(void *arg)
{
    /* Tell LVGL how many milliseconds has elapsed */
    lv_tick_inc(EXAMPLE_LVGL_TICK_PERIOD_MS);
}

/*Read the touchpad - FIXED VERSION*/
void my_touchpad_read( lv_indev_drv_t * indev_drv, lv_indev_data_t * data )
{
    // Read touch input
    if( touch.available() )
    {
        data->state = LV_INDEV_STATE_PR;
        data->point.x = touch.data.x;
        data->point.y = touch.data.y;
    }
    else
    {
        data->state = LV_INDEV_STATE_REL;
    }
}

void setup()
{
    Serial.begin( 115200 ); /* prepare for possible serial debug */
    delay(1000);

    String LVGL_Arduino = "Hello Arduino! ";
    LVGL_Arduino += String('V') + lv_version_major() + "." + lv_version_minor() + "." + lv_version_patch();

    Serial.println( LVGL_Arduino );
    Serial.println( "I am LVGL_Arduino with Custom LCD Driver" );

    // Initialize PSRAM
    Serial.println("Initializing PSRAM...");
    if(psramInit()){
        Serial.println("PSRAM is correctly initialized");
    }else{
        Serial.println("PSRAM not available");
    }

    // Allocate frame buffer in PSRAM (240*240*2 = 115200 bytes)
    UDOUBLE Imagesize = LCD_1IN28_HEIGHT * LCD_1IN28_WIDTH * 2;
    if ((BlackImage = (UWORD *)ps_malloc(Imagesize)) == NULL){
        Serial.println("Failed to allocate BlackImage in PSRAM!");
        while(1);
    }
    Serial.print("BlackImage allocated in PSRAM: ");
    Serial.print(Imagesize);
    Serial.println(" bytes");

    // Initialize GPIO and LCD
    Serial.println("Initializing LCD hardware...");
    if (DEV_Module_Init() != 0){
        Serial.println("GPIO Init Fail!");
        while(1);
    }
    Serial.println("GPIO Init successful!");

    // Initialize LCD
    LCD_1IN28_Init(HORIZONTAL);
    Serial.println("LCD initialized");

    // Test LCD with colors
    Serial.println("Testing LCD - RED");
    LCD_1IN28_Clear(0xF800);  // RED
    delay(500);
    Serial.println("Testing LCD - GREEN");
    LCD_1IN28_Clear(0x07E0);  // GREEN
    delay(500);
    Serial.println("Testing LCD - BLUE");
    LCD_1IN28_Clear(0x001F);  // BLUE
    delay(500);
    Serial.println("Testing LCD - BLACK");
    LCD_1IN28_Clear(0x0000);  // BLACK
    Serial.println("LCD test completed!");

    Serial.println("Initializing LVGL...");
    lv_init();
#if LV_USE_LOG != 0
    lv_log_register_print_cb( my_print ); /* register print function for debugging */
#endif
    Serial.println("LVGL initialized");

    // Initialize touch controller
    Serial.println("Initializing touch controller...");
    touch.begin();
    Serial.println("Touch initialized");

    lv_disp_draw_buf_init( &draw_buf, buf, NULL, screenWidth * screenHeight / 10 );

    /*Initialize the display*/
    static lv_disp_drv_t disp_drv;
    lv_disp_drv_init( &disp_drv );
    /*Change the following line to your display resolution*/
    disp_drv.hor_res = screenWidth;
    disp_drv.ver_res = screenHeight;
    disp_drv.flush_cb = my_disp_flush;
    disp_drv.draw_buf = &draw_buf;
    lv_disp_drv_register( &disp_drv );

    /*Initialize the (dummy) input device driver*/
    static lv_indev_drv_t indev_drv;
    lv_indev_drv_init( &indev_drv );
    indev_drv.type = LV_INDEV_TYPE_POINTER;
    indev_drv.read_cb = my_touchpad_read;
    lv_indev_drv_register( &indev_drv );

    // Setup LVGL timer
    const esp_timer_create_args_t lvgl_tick_timer_args = {
      .callback = &example_increase_lvgl_tick,
      .name = "lvgl_tick"
    };

    esp_timer_handle_t lvgl_tick_timer = NULL;
    esp_timer_create(&lvgl_tick_timer_args, &lvgl_tick_timer);
    esp_timer_start_periodic(lvgl_tick_timer, EXAMPLE_LVGL_TICK_PERIOD_MS * 1000);

    // Load SquareLine Studio UI
    Serial.println("Loading SquareLine Studio UI...");
    ui_init();
    Serial.println("UI loaded!");

    Serial.println( "Setup done" );
}

void loop()
{
    lv_timer_handler(); /* let the GUI do its work */
    delay( 5 );
}
