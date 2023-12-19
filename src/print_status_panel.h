#ifndef __PRINT_STATUS_PANEL_H__
#define __PRINT_STATUS_PANEL_H__

#include "websocket_client.h"
#include "notify_consumer.h"
#include "button_container.h"
#include "image_label.h"
#include "finetune_panel.h"
#include "lvgl/lvgl.h"

#include <mutex>
#include <ctime>
#include <map>

class PrintStatusPanel : public NotifyConsumer {
 public:
  /* PrintStatusPanel(KWebSocketClient &ws, std::mutex &lock, json &j); */
  PrintStatusPanel(KWebSocketClient &ws, std::mutex &lock);
  ~PrintStatusPanel();

  void init(json &fans);
  void reset();
  void populate();
  void foreground();
  void background();

  void handle_metadata(const std::string &gcode_file, json &j);
  void handle_callback(lv_event_t *event);
  
  static void _handle_callback(lv_event_t *event) {
    PrintStatusPanel *panel = (PrintStatusPanel*)event->user_data;
    panel->handle_callback(event);
  };

  void consume(json &j);
  void update_time_progress(uint32_t time_passed);
  void update_flow_rate(double filament_used);
  void update_layers(json &info);
  
  FineTunePanel &get_finetune_panel();

 private:
  KWebSocketClient &ws;
  FineTunePanel finetune_panel;
  lv_obj_t *status_cont;
  lv_obj_t *buttons_cont;
  ButtonContainer finetune_btn;
  ButtonContainer pause_btn;
  ButtonContainer resume_btn;
  ButtonContainer cancel_btn;
  ButtonContainer emergency_btn;
  ButtonContainer back_btn;
  lv_obj_t *thumbnail_cont;
  lv_obj_t *thumbnail;
  lv_obj_t *progress_bar;
  lv_obj_t *progress_label;
  lv_obj_t *detail_cont;

  ImageLabel extruder_temp;
  ImageLabel bed_temp;
  ImageLabel print_speed;
  ImageLabel z_offset;
  ImageLabel flow_rate;
  ImageLabel layers;
  ImageLabel fan0;
  ImageLabel elapsed;
  /* ImageLabel fan1; */
  ImageLabel time_left;
  /* ImageLabel fan2; */

  /* json &metadata; */
  uint32_t estimated_time_s;

  // flow rate
  std::time_t flow_ts;
  double last_filament_used;
  double filament_diameter;
  double flow;
  int extruder_target;
  int heater_bed_target;
  int cur_layer;
  int total_layer;

  std::map<std::string, int> fan_speeds;
};

#endif // __PRINT_STATUS_PANEL_H__
