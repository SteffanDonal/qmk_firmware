RGB_MATRIX_EFFECT(VISUALIZER)

#ifdef RGB_MATRIX_CUSTOM_EFFECT_IMPLS

typedef struct Point {
    uint8_t x;
    uint8_t y;
} Point;

#define VISUALIZER_LED_RANGE_X 224.0
#define VISUALIZER_LED_RANGE_Y 64.0

#define VISUALIZER_FREQ_LENGTH 32
#define VISUALIZER_FREQ_BYTE_RANGE_DIVISOR 8

static uint8_t visualizer_scroll_pos;
static int8_t visualizer_velocity;
static HSV visualizer_background;
static HSV visualizer_foreground;

static float visualizer_freq[VISUALIZER_FREQ_LENGTH] = { 0 };

static HSV VISUALIZER_MATH(HSV hsv, uint8_t i, uint8_t time) {
    const uint8_t segment_length = 256/4;

    Point led_position = {
        g_led_config.point[i].x / VISUALIZER_LED_RANGE_X * 255.0,
        g_led_config.point[i].y / VISUALIZER_LED_RANGE_Y * 255.0
    };

    uint16_t global_pos = visualizer_scroll_pos + led_position.x;
    uint8_t segment_pos = global_pos % segment_length;
    bool is_odd = (global_pos / segment_length) % 2 != 0;

    // Assign visualiser colors
    hsv.h = is_odd ? visualizer_foreground.h + 128 : visualizer_foreground.h;
    hsv.s = visualizer_foreground.s;
    hsv.v = visualizer_foreground.v;

    // Slight x/y hue drift over the whole keyboard
    hsv.h += (led_position.y / 5 + led_position.x / 5) / 2;

    // Make segments segmented
    if (segment_pos < segment_length * 0.4)
        hsv = (HSV){ 0, 0, 0 };

    // Underglow is "background"
    if (HAS_ANY_FLAGS(g_led_config.flags[i], LED_FLAG_UNDERGLOW))
        hsv = visualizer_background;

    // Use frequency intensity to set value
    hsv.v = visualizer_freq[led_position.x / VISUALIZER_FREQ_BYTE_RANGE_DIVISOR] * 255;

    return hsv;
}

static bool VISUALIZER(effect_params_t* params) { return effect_runner_i(params, &VISUALIZER_MATH); }


#include "raw_hid.h"

void raw_hid_receive(uint8_t *data, uint8_t length) {
    visualizer_scroll_pos = data[0];
    visualizer_velocity = data[1] - 128;
    visualizer_background.h = data[2];
    visualizer_background.s = data[3];
    visualizer_background.v = data[4];
    visualizer_foreground.h = data[5];
    visualizer_foreground.s = data[6];
    visualizer_foreground.v = data[7];

    uint8_t freq_start = 8;
    for (uint8_t i = 0; i < VISUALIZER_FREQ_LENGTH; i++) {
        visualizer_freq[i] = data[freq_start + i] / 255.0f;
    }
}

#endif // RGB_MATRIX_CUSTOM_EFFECT_IMPLS
