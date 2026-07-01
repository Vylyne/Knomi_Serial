#include "temp_page.h"
#include "ui/pages/page_helper.h"

namespace ui
{
  namespace temp_page
  {
    static lv_obj_t *_hotend_temp = nullptr;
    static lv_obj_t *_hotend_target = nullptr;
    static lv_obj_t *_hotend_label = nullptr;
    static lv_obj_t *_bed_temp = nullptr;
    static lv_obj_t *_bed_target = nullptr;
    static lv_obj_t *_bed_label = nullptr;
    static lv_obj_t *_chamber_temp = nullptr;
    static lv_obj_t *_chamber_target = nullptr;
    static lv_obj_t *_chamber_label = nullptr;
    static lv_obj_t *_mcu_temp = nullptr;
    static lv_obj_t *_mcu_target = nullptr;
    static lv_obj_t *_mcu_label = nullptr;

    // Declare Private Functions prototypes
    lv_obj_t *_init_temp(lv_obj_t *parent, int y, const char *name, lv_obj_t **label_out = nullptr);
    void _update_temp(lv_obj_t *temp, int value);
    void _update_target(lv_obj_t *target, lv_obj_t *temp, int value);
    void _set_row_visible(lv_obj_t *label, lv_obj_t *temp, lv_obj_t *target, bool visible);

    // Define Functions
    lv_obj_t *init(lv_obj_t *parent, const printer::State &state)
    {
      lv_obj_t *page = page_helper::create_page(parent, "TEMP");

      // Y positions here are placeholders (0) because printer_update handles them dynamically
      _hotend_temp   = _init_temp(page, 0, "HOTEND", &_hotend_label);
      _hotend_target = lv_label_create(page);
      _bed_temp      = _init_temp(page, 0, "BED", &_bed_label);
      _bed_target    = lv_label_create(page);
      _chamber_temp  = _init_temp(page, 0, "CHAMBER", &_chamber_label);
      _chamber_target = lv_label_create(page);
      _mcu_temp      = _init_temp(page, 0, "MCU", &_mcu_label);
      _mcu_target    = lv_label_create(page);

      return page;
    }

    lv_obj_t *_init_temp(lv_obj_t *parent, int y, const char *name, lv_obj_t **label_out)
    {
      lv_obj_t *temp = lv_label_create(parent);
      lv_obj_align(temp, LV_ALIGN_LEFT_MID, 125, y);
      lv_obj_set_style_text_font(temp, &lv_font_montserrat_24, LV_PART_MAIN);

      lv_obj_t *label = lv_label_create(parent);
      lv_obj_align(label, LV_ALIGN_RIGHT_MID, -130, y);
      lv_label_set_text(label, name);

      if (label_out)
        *label_out = label;
      return temp;
    }

    void printer_update(const printer::State &state)
    {
      // 1. Determine visibility by checking if the temperature sensor is active/present
      bool show_hotend  = (state.hotend_temp != 0);
      bool show_bed     = (state.bed_temp != 0);
      bool show_chamber = (state.chamber_temp != 0);
      bool show_mcu     = (state.mcu_temp != 0);

      _set_row_visible(_hotend_label, _hotend_temp, _hotend_target, show_hotend);
      _set_row_visible(_bed_label, _bed_temp, _bed_target, show_bed);
      _set_row_visible(_chamber_label, _chamber_temp, _chamber_target, show_chamber);
      _set_row_visible(_mcu_label, _mcu_temp, _mcu_target, show_mcu);

      // 2. Update raw temperature values first (keeps widths stable before positioning)
      _update_temp(_hotend_temp, state.hotend_temp);
      _update_temp(_bed_temp, state.bed_temp);
      _update_temp(_chamber_temp, state.chamber_temp);
      _update_temp(_mcu_temp, state.mcu_temp);

      // 3. Dynamically space active rows centered vertically on the screen
      int active_count = (show_hotend ? 1 : 0) + (show_bed ? 1 : 0) + 
                         (show_chamber ? 1 : 0) + (show_mcu ? 1 : 0);

      if (active_count > 0)
      {
        // Using an even gap size (like 40 or 44) guarantees perfect pixel symmetry
        int gap = 44; 
        int total_span = (active_count - 1) * gap;
        int current_y = -total_span / 2;

        struct RowLayout {
          bool visible;
          lv_obj_t *temp;
          lv_obj_t *label;
        };

        RowLayout layout[] = {
          {show_hotend,  _hotend_temp,  _hotend_label},
          {show_bed,     _bed_temp,     _bed_label},
          {show_chamber, _chamber_temp, _chamber_label},
          {show_mcu,     _mcu_temp,     _mcu_label}
        };

        for (const auto& row : layout)
        {
          if (row.visible)
          {
            lv_obj_align(row.temp, LV_ALIGN_LEFT_MID, 125, current_y);
            lv_obj_align(row.label, LV_ALIGN_RIGHT_MID, -130, current_y);
            current_y += gap;
          }
        }
      }

      // 4. Finally, update and anchor target readouts relative to the moved temp labels
      _update_target(_hotend_target, _hotend_temp, state.hotend_target);
      _update_target(_bed_target, _bed_temp, state.bed_target);
      _update_target(_chamber_target, _chamber_temp, state.chamber_target);
      _update_target(_mcu_target, _mcu_temp, state.mcu_target);
    }

    void _update_temp(lv_obj_t *temp, int value)
    {
      lv_label_set_text_fmt(temp, "%d", value);
    }

    void _update_target(lv_obj_t *target, lv_obj_t *temp, int value)
    {
      lv_label_set_text_fmt(target, "/ %d", value);
      lv_obj_align_to(target, temp, LV_ALIGN_OUT_RIGHT_BOTTOM, 5, -3);
      if (value == 0)
      {
        lv_obj_add_flag(target, LV_OBJ_FLAG_HIDDEN);
      }
      else
      {
        lv_obj_clear_flag(target, LV_OBJ_FLAG_HIDDEN);
      }
    }

    void _set_row_visible(lv_obj_t *label, lv_obj_t *temp, lv_obj_t *target, bool visible)
    {
      if (visible)
      {
        lv_obj_clear_flag(label, LV_OBJ_FLAG_HIDDEN);
        lv_obj_clear_flag(temp, LV_OBJ_FLAG_HIDDEN);
        lv_obj_clear_flag(target, LV_OBJ_FLAG_HIDDEN);
      }
      else
      {
        lv_obj_add_flag(label, LV_OBJ_FLAG_HIDDEN);
        lv_obj_add_flag(temp, LV_OBJ_FLAG_HIDDEN);
        lv_obj_add_flag(target, LV_OBJ_FLAG_HIDDEN);
      }
    }
  }
}