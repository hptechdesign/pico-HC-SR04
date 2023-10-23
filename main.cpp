
#include <string.h>
#include <math.h>
#include <vector>
#include <cstdlib>

#include "libraries/pico_display_2/pico_display_2.hpp"
#include "drivers/st7789/st7789.hpp"
#include "libraries/pico_graphics/pico_graphics.hpp"
#include "rgbled.hpp"
#include "button.hpp"

#include "distance_sensor.h"
#include "sensor.h"
#include "pico/stdlib.h"
#include "stdio.h"

using namespace pimoroni;

#define rowHeight 20
#define valOffset 250

ST7789 st7789(320, 240, ROTATE_0, false, get_spi_pins(BG_SPI_FRONT));
PicoGraphics_PenRGB332 graphics(st7789.width, st7789.height, nullptr);

RGBLED led(PicoDisplay2::LED_R, PicoDisplay2::LED_G, PicoDisplay2::LED_B);
void toggle_rVal(void);
void print_sensorData(uint32_t distance);

Button button_a(PicoDisplay2::A);
Button button_b(PicoDisplay2::B);
Button button_x(PicoDisplay2::X);
Button button_y(PicoDisplay2::Y);

static char printBuf[256];

bool toggle_led = false;
static uint8_t rVal = 0;
static Point text_location(0, 0);

int main()
{

  // Create an instance of the sensor
  // specify the pio, state machine, and gpio pin connected to trigger
  // echo pin must be on gpio pin trigger + 1.
  DistanceSensor hcsr04{pio0, 0, 2};

  stdio_init_all();
  static repeating_timer_t timer;

  st7789.set_backlight(255);

  Pen BG = graphics.create_pen(0, 0, 0);
  Pen WHITE = graphics.create_pen(255, 255, 255);

  printf("\nBegin main loop");

  led.set_brightness(64);

  while (true)
  {
    if (button_a.raw())
      text_location.x -= 1;
    if (button_b.raw())
      text_location.x += 1;

    if (button_x.raw())
      text_location.y -= 1;
    if (button_y.raw())
      text_location.y += 1;

    graphics.set_pen(BG);
    graphics.clear();

    graphics.set_pen(WHITE);

    // Trigger background sense
    hcsr04.TriggerRead();

    // wait for sensor to get a result
    while (hcsr04.is_sensing)
    {
      sleep_us(100);
    }

    print_sensorData(hcsr04.distance);
    toggle_rVal();
    led.set_rgb(0, rVal, 0);
    sleep_ms(10);

    // update screen
    st7789.update(&graphics);
  }

  return 0;
}

void print_sensorData(uint32_t distance)
{
  text_location.x += 20;
  graphics.text("-HC-SR04 Ultrasonic Sensor-", text_location, 320, 2);
  text_location.x -= 20;

  // Read result
  text_location.x += 10;
  text_location.y += rowHeight * 2;
  snprintf(printBuf, 256, "Distance: %d cm\n", distance);
  graphics.text(printBuf, text_location, 320);
  text_location.y -= rowHeight * 2;
  text_location.x -= 10;
}

void toggle_rVal(void)
{
  if (toggle_led)
  {
    rVal = 255;
    toggle_led = false;
  }
  else
  {
    rVal = 0;
    toggle_led = true;
  }
}
