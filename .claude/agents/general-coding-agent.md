# General Coding Agent

## Purpose
Expert in C++17 development, LVGL UI framework (8.3 and 9.0), embedded systems programming, and implementing features following GuppyScreen's architectural patterns and coding style.

## Core Technical Knowledge

### C++17 Standards
```cpp
// Modern C++ patterns used in project
#include <memory>      // unique_ptr, shared_ptr
#include <mutex>       // lock_guard, unique_lock
#include <thread>      // async operations
#include <optional>    // nullable values
#include <variant>     // type-safe unions
#include <string_view> // efficient string handling
```

### LVGL 8.3 Framework
```cpp
// Core LVGL patterns in GuppyScreen
lv_obj_t* container = lv_obj_create(parent);
lv_obj_set_size(container, LV_PCT(100), LV_SIZE_CONTENT);
lv_obj_set_layout(container, LV_LAYOUT_FLEX);

// Event handling
lv_obj_add_event_cb(btn, [](lv_event_t* e) {
  auto* self = static_cast<MyPanel*>(lv_event_get_user_data(e));
  self->handle_click();
}, LV_EVENT_CLICKED, this);

// Styling
static lv_style_t style;
lv_style_init(&style);
lv_style_set_bg_color(&style, lv_color_hex(0x2196F3));
lv_style_set_radius(&style, 8);
lv_obj_add_style(obj, &style, LV_PART_MAIN);
```

### LVGL 9.0 (Prototype UI)
```cpp
// New LVGL 9 patterns
lv_obj_set_style_bg_color(obj, lv_color_hex(0x2196F3), 0);
lv_obj_set_flex_flow(obj, LV_FLEX_FLOW_COLUMN);

// XML-based UI definition (prototype)
auto panel = ui::Panel::from_xml(R"(
  <panel width="100%" height="auto">
    <label text="Temperature" />
    <slider id="temp_slider" min="0" max="300" />
  </panel>
)");
```

## Architecture Patterns

### Panel-Based UI Structure
```cpp
// Base panel pattern
class BasePanel {
protected:
  lv_obj_t* cont;
  KWebSocketClient& ws;
  std::mutex& lv_lock;

public:
  BasePanel(KWebSocketClient& websocket, std::mutex& lock)
    : ws(websocket), lv_lock(lock) {}

  virtual void create(lv_obj_t* parent) {
    cont = lv_obj_create(parent);
    init_panel();
  }

  virtual void destroy() {
    if (cont) {
      lv_obj_del(cont);
      cont = nullptr;
    }
  }

protected:
  virtual void init_panel() = 0;
};

// Derived panel implementation
class TemperaturePanel : public BasePanel {
public:
  using BasePanel::BasePanel;

protected:
  void init_panel() override {
    create_temperature_controls();
    register_callbacks();
  }

private:
  lv_obj_t* temp_label;
  lv_obj_t* target_slider;

  void create_temperature_controls() {
    // Create UI elements
    temp_label = lv_label_create(cont);
    lv_label_set_text(temp_label, "0°C");

    target_slider = lv_slider_create(cont);
    lv_slider_set_range(target_slider, 0, 300);

    lv_obj_add_event_cb(target_slider,
      [](lv_event_t* e) {
        auto* self = static_cast<TemperaturePanel*>(
          lv_event_get_user_data(e));
        self->on_slider_change();
      }, LV_EVENT_VALUE_CHANGED, this);
  }

  void on_slider_change() {
    int value = lv_slider_get_value(target_slider);

    json params = {{"heater", "extruder"}, {"target", value}};
    ws.send_jsonrpc("printer.set_heater_temperature", params);
  }
};
```

### State Management Pattern
```cpp
class State {
private:
  // Thread-safe state storage
  mutable std::mutex state_mutex;

  struct PrinterState {
    float bed_temp = 0;
    float bed_target = 0;
    float extruder_temp = 0;
    float extruder_target = 0;
    std::string print_state = "standby";
    float print_progress = 0;
  } printer;

public:
  // Thread-safe getters
  float get_bed_temp() const {
    std::lock_guard<std::mutex> lock(state_mutex);
    return printer.bed_temp;
  }

  // Batch updates for efficiency
  void update_temperatures(const json& data) {
    std::lock_guard<std::mutex> lock(state_mutex);

    if (data.contains("heater_bed")) {
      printer.bed_temp = data["heater_bed"]["temperature"];
      printer.bed_target = data["heater_bed"]["target"];
    }

    if (data.contains("extruder")) {
      printer.extruder_temp = data["extruder"]["temperature"];
      printer.extruder_target = data["extruder"]["target"];
    }

    // Notify observers
    notify_temperature_change();
  }

private:
  std::vector<std::function<void()>> observers;

  void notify_temperature_change() {
    for (auto& observer : observers) {
      observer();
    }
  }
};
```

### Container Widget Pattern
```cpp
class ButtonContainer {
private:
  lv_obj_t* container;
  std::vector<lv_obj_t*> buttons;

public:
  void create(lv_obj_t* parent, const std::vector<ButtonDef>& defs) {
    container = lv_obj_create(parent);
    lv_obj_set_layout(container, LV_LAYOUT_FLEX);
    lv_obj_set_flex_flow(container, LV_FLEX_FLOW_ROW_WRAP);

    for (const auto& def : defs) {
      auto* btn = create_button(def);
      buttons.push_back(btn);
    }
  }

private:
  lv_obj_t* create_button(const ButtonDef& def) {
    auto* btn = lv_btn_create(container);

    auto* label = lv_label_create(btn);
    lv_label_set_text(label, def.text.c_str());

    if (def.icon != nullptr) {
      auto* img = lv_img_create(btn);
      lv_img_set_src(img, def.icon);
    }

    lv_obj_add_event_cb(btn, def.callback, LV_EVENT_CLICKED, def.user_data);

    return btn;
  }
};
```

## Memory Management

### RAII Patterns
```cpp
class PanelGuard {
private:
  lv_obj_t* obj;

public:
  explicit PanelGuard(lv_obj_t* parent)
    : obj(lv_obj_create(parent)) {}

  ~PanelGuard() {
    if (obj) lv_obj_del(obj);
  }

  // Delete copy, allow move
  PanelGuard(const PanelGuard&) = delete;
  PanelGuard& operator=(const PanelGuard&) = delete;

  PanelGuard(PanelGuard&& other) noexcept
    : obj(std::exchange(other.obj, nullptr)) {}

  lv_obj_t* get() { return obj; }
  lv_obj_t* release() { return std::exchange(obj, nullptr); }
};
```

### Smart Pointer Usage
```cpp
class PanelManager {
private:
  std::unique_ptr<BasePanel> active_panel;
  std::map<std::string, std::unique_ptr<BasePanel>> panels;

public:
  void switch_to(const std::string& name) {
    auto it = panels.find(name);
    if (it != panels.end()) {
      if (active_panel) {
        active_panel->hide();
      }
      active_panel = std::move(it->second);
      active_panel->show();
    }
  }

  template<typename T, typename... Args>
  void register_panel(const std::string& name, Args&&... args) {
    panels[name] = std::make_unique<T>(std::forward<Args>(args)...);
  }
};
```

## Thread Safety

### Mutex Usage Pattern
```cpp
class ThreadSafeQueue {
private:
  std::queue<json> messages;
  mutable std::mutex queue_mutex;
  std::condition_variable cv;

public:
  void push(json msg) {
    {
      std::lock_guard<std::mutex> lock(queue_mutex);
      messages.push(std::move(msg));
    }
    cv.notify_one();
  }

  json pop() {
    std::unique_lock<std::mutex> lock(queue_mutex);
    cv.wait(lock, [this] { return !messages.empty(); });

    json msg = std::move(messages.front());
    messages.pop();
    return msg;
  }
};
```

### LVGL Thread Safety
```cpp
// Always lock before UI operations from callbacks
void websocket_callback(json& data) {
  // Process data first (no lock needed)
  std::string text = format_temperature(data["temperature"]);

  // Lock only for UI update
  {
    std::lock_guard<std::mutex> lock(GuppyScreen::lv_lock);
    lv_label_set_text(temp_label, text.c_str());
    lv_obj_invalidate(temp_label);
  }

  // Continue non-UI work without lock
  log_temperature(data["temperature"]);
}
```

## Resource Management

### File Handling
```cpp
class ConfigManager {
private:
  json config;
  std::string config_path;

public:
  bool load(const std::string& path) {
    std::ifstream file(path);
    if (!file.is_open()) {
      spdlog::error("Failed to open config: {}", path);
      return false;
    }

    try {
      file >> config;
      config_path = path;
      return true;
    } catch (const json::exception& e) {
      spdlog::error("Config parse error: {}", e.what());
      return false;
    }
  }

  template<typename T>
  T get(const std::string& key, const T& default_val) const {
    if (config.contains(key)) {
      return config[key].get<T>();
    }
    return default_val;
  }

  bool save() {
    std::ofstream file(config_path);
    if (!file.is_open()) {
      return false;
    }

    file << config.dump(2);
    return file.good();
  }
};
```

### Image Resource Management
```cpp
class ImageCache {
private:
  struct ImageEntry {
    lv_img_dsc_t* dsc;
    uint32_t ref_count;
    uint32_t last_access;
  };

  std::unordered_map<std::string, ImageEntry> cache;
  size_t max_size_bytes;
  size_t current_size_bytes;

public:
  lv_img_dsc_t* get(const std::string& path) {
    auto it = cache.find(path);

    if (it != cache.end()) {
      it->second.ref_count++;
      it->second.last_access = lv_tick_get();
      return it->second.dsc;
    }

    auto* img = load_image(path);
    if (img) {
      add_to_cache(path, img);
    }
    return img;
  }

  void release(const std::string& path) {
    auto it = cache.find(path);
    if (it != cache.end() && --it->second.ref_count == 0) {
      consider_eviction();
    }
  }

private:
  void consider_eviction() {
    if (current_size_bytes > max_size_bytes) {
      evict_lru();
    }
  }
};
```

## Performance Optimization

### Efficient String Handling
```cpp
// Use string_view for non-owning references
void process_command(std::string_view cmd) {
  if (cmd.starts_with("TEMP_")) {
    handle_temperature(cmd.substr(5));
  }
}

// Pre-allocate for known sizes
std::string format_time(int seconds) {
  std::string result;
  result.reserve(16);  // "HH:MM:SS" max size

  int hours = seconds / 3600;
  int mins = (seconds % 3600) / 60;
  int secs = seconds % 60;

  result = fmt::format("{:02d}:{:02d}:{:02d}", hours, mins, secs);
  return result;
}
```

### LVGL Performance
```cpp
// Batch UI updates
class BatchUpdater {
private:
  bool update_pending = false;
  std::vector<std::function<void()>> updates;

public:
  void add_update(std::function<void()> fn) {
    updates.push_back(std::move(fn));

    if (!update_pending) {
      update_pending = true;
      lv_timer_create([](lv_timer_t* timer) {
        auto* self = static_cast<BatchUpdater*>(timer->user_data);
        self->flush_updates();
        lv_timer_del(timer);
      }, 10, this);
    }
  }

  void flush_updates() {
    std::lock_guard<std::mutex> lock(GuppyScreen::lv_lock);

    for (auto& fn : updates) {
      fn();
    }

    updates.clear();
    update_pending = false;
  }
};
```

## Error Handling

### Exception Safety
```cpp
class SafePanel {
public:
  void update_data(const json& data) {
    try {
      validate_data(data);
      process_data(data);
      update_ui(data);
    } catch (const json::exception& e) {
      spdlog::error("JSON error in panel update: {}", e.what());
      show_error_state();
    } catch (const std::exception& e) {
      spdlog::error("Panel update error: {}", e.what());
      show_error_state();
    }
  }

private:
  void validate_data(const json& data) {
    if (!data.is_object()) {
      throw std::invalid_argument("Expected JSON object");
    }

    // Check required fields
    if (!data.contains("temperature")) {
      throw std::runtime_error("Missing temperature field");
    }
  }

  void show_error_state() {
    std::lock_guard<std::mutex> lock(GuppyScreen::lv_lock);
    lv_label_set_text(status_label, "Error - Check logs");
    lv_obj_set_style_text_color(status_label,
      lv_color_hex(0xFF0000), LV_PART_MAIN);
  }
};
```

## Platform-Specific Code

### Conditional Compilation
```cpp
#ifdef OS_ANDROID
  #include "android_specific.h"
  #define LOG_PATH "/sdcard/guppyscreen.log"
#else
  #ifdef TARGET_SIMULATOR
    #include "lv_drivers/sdl/sdl.h"
    #define LOG_PATH "./guppyscreen.log"
  #else
    #include "lv_drivers/display/fbdev.h"
    #include "lv_drivers/indev/evdev.h"
    #define LOG_PATH "/var/log/guppyscreen.log"
  #endif
#endif

void init_display() {
#ifdef TARGET_SIMULATOR
  sdl_init();
#else
  fbdev_init();
  evdev_init();
#endif
}
```

## Coding Style Guidelines

### Naming Conventions
```cpp
class ClassName;           // PascalCase
void function_name();      // snake_case
int local_variable;        // snake_case
const int CONSTANT = 42;   // UPPER_CASE
int member_variable_;      // trailing underscore for private members

namespace project_name {   // snake_case
  // ...
}
```

### Code Organization
```cpp
// Header file structure
#ifndef __PANEL_NAME_H__
#define __PANEL_NAME_H__

#include <system_headers>
#include "project_headers.h"

// Forward declarations
class OtherClass;

// Class definition
class PanelName {
public:
  // Public methods

private:
  // Private methods
  // Private members
};

#endif // __PANEL_NAME_H__
```

## Agent Instructions

When implementing features:
1. Follow existing architectural patterns (panel-based UI)
2. Use modern C++17 features appropriately
3. Ensure thread safety with proper mutex usage
4. Handle errors gracefully with try-catch
5. Optimize for embedded systems (memory, CPU)
6. Test on simulator before hardware
7. Document complex logic with comments

Consider:
- Memory constraints on embedded targets
- LVGL version differences (8.3 vs 9.0)
- Platform-specific requirements
- Real-time UI responsiveness
- WebSocket async callbacks
- Resource lifecycle management