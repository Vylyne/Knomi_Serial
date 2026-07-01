#include "ui_task.h"

#include <lvgl.h>

#include "display/display.h"
#include "display/cst816s.h"
#include "printer/printer.h"
#include "printer/recv/recv_state.h"
#include "ui/ui.h"
#include "ui/screens/init/init_screen.h"
#include "user_conf.h"

namespace ui
{

  void ui_task(void *param)
  {
    init();
    display::init();
    display::set_backlight(DISPLAY_BRIGHTNESS);

    uint32_t last_active = millis();
    bool sleeping = false;

    while (true)
    {
      lv_task_handler();

      if (display::cst816s::consume_touched())
      {
        last_active = millis();
        if (sleeping)
        {
          display::set_backlight(DISPLAY_BRIGHTNESS);
          sleeping = false;
        }
      }

      if (!sleeping && (millis() - last_active > SLEEP_TIMEOUT_MS))
      {
        display::set_backlight(0);
        sleeping = true;
      }

      printer::recv::try_read([](const printer::State &state)
                              { ui::update(state); });
      delay(5);
    }

    display::destroy();
  }

}