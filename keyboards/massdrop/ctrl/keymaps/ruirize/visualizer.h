RGB_MATRIX_EFFECT(VISUALIZER)

#ifdef RGB_MATRIX_CUSTOM_EFFECT_IMPLS


static uint8_t some_other_global_state;
static void VISUALIZER_INIT(effect_params_t* params) {
  some_other_global_state = 1;
}

static bool VISUALIZER_RUN(effect_params_t* params) {
  RGB_MATRIX_USE_LIMITS(led_min, led_max);

  for (uint8_t i = led_min; i < led_max; i++) {
    rgb_matrix_set_color(i, some_other_global_state % 0xff + 0x88, some_other_global_state % 0x22, some_other_global_state % + 0xFF);
    some_other_global_state++;
  }

  return led_max < DRIVER_LED_TOTAL;
}

static bool VISUALIZER(effect_params_t* params) {
  if (params->init)
      VISUALIZER_INIT(params);

  return VISUALIZER_RUN(params);
}

#endif // RGB_MATRIX_CUSTOM_EFFECT_IMPLS
