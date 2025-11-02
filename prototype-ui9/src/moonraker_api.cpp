/*
 * Copyright (C) 2025 356C LLC
 * Author: Preston Brown <pbrown@brown-house.net>
 *
 * This file is part of HelixScreen.
 *
 * HelixScreen is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * HelixScreen is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with HelixScreen. If not, see <https://www.gnu.org/licenses/>.
 */

#include "moonraker_api.h"
#include "spdlog/spdlog.h"
#include <sstream>
#include <iomanip>

MoonrakerAPI::MoonrakerAPI(MoonrakerClient& client, PrinterState& state)
    : client_(client)
    , state_(state) {
}

// ============================================================================
// File Management Operations
// ============================================================================

void MoonrakerAPI::list_files(const std::string& root,
                              const std::string& path,
                              bool recursive,
                              FileListCallback on_success,
                              ErrorCallback on_error) {
    json params = {
        {"root", root}
    };

    if (!path.empty()) {
        params["path"] = path;
    }

    if (recursive) {
        params["extended"] = true;
    }

    spdlog::debug("Listing files in {}/{}", root, path);

    client_.send_jsonrpc("server.files.list", params,
        [this, on_success](json& response) {
            try {
                std::vector<FileInfo> files = parse_file_list(response);
                spdlog::debug("Found {} files", files.size());
                on_success(files);
            } catch (const std::exception& e) {
                spdlog::error("Failed to parse file list: {}", e.what());
                on_success(std::vector<FileInfo>{});  // Return empty list on parse error
            }
        },
        on_error
    );
}

void MoonrakerAPI::get_file_metadata(const std::string& filename,
                                     FileMetadataCallback on_success,
                                     ErrorCallback on_error) {
    json params = {
        {"filename", filename}
    };

    spdlog::debug("Getting metadata for file: {}", filename);

    client_.send_jsonrpc("server.files.metadata", params,
        [this, on_success](json& response) {
            try {
                FileMetadata metadata = parse_file_metadata(response);
                on_success(metadata);
            } catch (const std::exception& e) {
                spdlog::error("Failed to parse file metadata: {}", e.what());
                FileMetadata empty;
                on_success(empty);
            }
        },
        on_error
    );
}

void MoonrakerAPI::delete_file(const std::string& filename,
                               SuccessCallback on_success,
                               ErrorCallback on_error) {
    json params = {
        {"path", filename}
    };

    spdlog::info("Deleting file: {}", filename);

    client_.send_jsonrpc("server.files.delete_file", params,
        [on_success](json& response) {
            spdlog::info("File deleted successfully");
            on_success();
        },
        on_error
    );
}

void MoonrakerAPI::move_file(const std::string& source,
                             const std::string& dest,
                             SuccessCallback on_success,
                             ErrorCallback on_error) {
    spdlog::info("Moving file from {} to {}", source, dest);

    json params = {
        {"source", source},
        {"dest", dest}
    };

    client_.send_jsonrpc("server.files.move", params,
        [on_success](json& response) {
            spdlog::info("File moved successfully");
            on_success();
        },
        on_error
    );
}

void MoonrakerAPI::copy_file(const std::string& source,
                             const std::string& dest,
                             SuccessCallback on_success,
                             ErrorCallback on_error) {
    spdlog::info("Copying file from {} to {}", source, dest);

    json params = {
        {"source", source},
        {"dest", dest}
    };

    client_.send_jsonrpc("server.files.copy", params,
        [on_success](json& response) {
            spdlog::info("File copied successfully");
            on_success();
        },
        on_error
    );
}

void MoonrakerAPI::create_directory(const std::string& path,
                                    SuccessCallback on_success,
                                    ErrorCallback on_error) {
    spdlog::info("Creating directory: {}", path);

    json params = {
        {"path", path}
    };

    client_.send_jsonrpc("server.files.post_directory", params,
        [on_success](json& response) {
            spdlog::info("Directory created successfully");
            on_success();
        },
        on_error
    );
}

void MoonrakerAPI::delete_directory(const std::string& path,
                                    bool force,
                                    SuccessCallback on_success,
                                    ErrorCallback on_error) {
    spdlog::info("Deleting directory: {} (force: {})", path, force);

    json params = {
        {"path", path},
        {"force", force}
    };

    client_.send_jsonrpc("server.files.delete_directory", params,
        [on_success](json& response) {
            spdlog::info("Directory deleted successfully");
            on_success();
        },
        on_error
    );
}

// ============================================================================
// Job Control Operations
// ============================================================================

void MoonrakerAPI::start_print(const std::string& filename,
                               SuccessCallback on_success,
                               ErrorCallback on_error) {
    json params = {
        {"filename", filename}
    };

    spdlog::info("Starting print: {}", filename);

    client_.send_jsonrpc("printer.print.start", params,
        [on_success](json& response) {
            spdlog::info("Print started successfully");
            on_success();
        },
        on_error
    );
}

void MoonrakerAPI::pause_print(SuccessCallback on_success,
                               ErrorCallback on_error) {
    spdlog::info("Pausing print");

    client_.send_jsonrpc("printer.print.pause", json::object(),
        [on_success](json& response) {
            spdlog::info("Print paused successfully");
            on_success();
        },
        on_error
    );
}

void MoonrakerAPI::resume_print(SuccessCallback on_success,
                                ErrorCallback on_error) {
    spdlog::info("Resuming print");

    client_.send_jsonrpc("printer.print.resume", json::object(),
        [on_success](json& response) {
            spdlog::info("Print resumed successfully");
            on_success();
        },
        on_error
    );
}

void MoonrakerAPI::cancel_print(SuccessCallback on_success,
                                ErrorCallback on_error) {
    spdlog::info("Canceling print");

    client_.send_jsonrpc("printer.print.cancel", json::object(),
        [on_success](json& response) {
            spdlog::info("Print canceled successfully");
            on_success();
        },
        on_error
    );
}

// ============================================================================
// Motion Control Operations
// ============================================================================

void MoonrakerAPI::home_axes(const std::string& axes,
                             SuccessCallback on_success,
                             ErrorCallback on_error) {
    std::string gcode = generate_home_gcode(axes);
    spdlog::info("Homing axes: {} (G-code: {})", axes.empty() ? "all" : axes, gcode);

    execute_gcode(gcode, on_success, on_error);
}

void MoonrakerAPI::move_axis(char axis,
                             double distance,
                             double feedrate,
                             SuccessCallback on_success,
                             ErrorCallback on_error) {
    std::string gcode = generate_move_gcode(axis, distance, feedrate);
    spdlog::info("Moving axis {} by {}mm (G-code: {})", axis, distance, gcode);

    execute_gcode(gcode, on_success, on_error);
}

void MoonrakerAPI::move_to_position(char axis,
                                    double position,
                                    double feedrate,
                                    SuccessCallback on_success,
                                    ErrorCallback on_error) {
    std::string gcode = generate_absolute_move_gcode(axis, position, feedrate);
    spdlog::info("Moving axis {} to {}mm (G-code: {})", axis, position, gcode);

    execute_gcode(gcode, on_success, on_error);
}

// ============================================================================
// Temperature Control Operations
// ============================================================================

void MoonrakerAPI::set_temperature(const std::string& heater,
                                   double temperature,
                                   SuccessCallback on_success,
                                   ErrorCallback on_error) {
    std::ostringstream gcode;
    gcode << "SET_HEATER_TEMPERATURE HEATER=" << heater << " TARGET=" << temperature;

    spdlog::info("Setting {} temperature to {}°C", heater, temperature);

    execute_gcode(gcode.str(), on_success, on_error);
}

void MoonrakerAPI::set_fan_speed(const std::string& fan,
                                 double speed,
                                 SuccessCallback on_success,
                                 ErrorCallback on_error) {
    // Convert percentage to 0-255 range for M106 command
    int fan_value = static_cast<int>(speed * 255.0 / 100.0);

    std::ostringstream gcode;
    if (fan == "fan") {
        // Part cooling fan uses M106
        gcode << "M106 S" << fan_value;
    } else {
        // Generic fans use SET_FAN_SPEED
        gcode << "SET_FAN_SPEED FAN=" << fan << " SPEED=" << (speed / 100.0);
    }

    spdlog::info("Setting {} speed to {}%", fan, speed);

    execute_gcode(gcode.str(), on_success, on_error);
}

// ============================================================================
// System Control Operations
// ============================================================================

void MoonrakerAPI::execute_gcode(const std::string& gcode,
                                 SuccessCallback on_success,
                                 ErrorCallback on_error) {
    json params = {
        {"script", gcode}
    };

    spdlog::debug("Executing G-code: {}", gcode);

    client_.send_jsonrpc("printer.gcode.script", params,
        [on_success](json& response) {
            on_success();
        },
        on_error
    );
}

void MoonrakerAPI::emergency_stop(SuccessCallback on_success,
                                  ErrorCallback on_error) {
    spdlog::warn("Emergency stop requested!");

    client_.send_jsonrpc("printer.emergency_stop", json::object(),
        [on_success](json& response) {
            spdlog::info("Emergency stop executed");
            on_success();
        },
        on_error
    );
}

void MoonrakerAPI::restart_firmware(SuccessCallback on_success,
                                    ErrorCallback on_error) {
    spdlog::info("Restarting firmware");

    client_.send_jsonrpc("printer.firmware_restart", json::object(),
        [on_success](json& response) {
            spdlog::info("Firmware restart initiated");
            on_success();
        },
        on_error
    );
}

void MoonrakerAPI::restart_klipper(SuccessCallback on_success,
                                   ErrorCallback on_error) {
    spdlog::info("Restarting Klipper");

    client_.send_jsonrpc("printer.restart", json::object(),
        [on_success](json& response) {
            spdlog::info("Klipper restart initiated");
            on_success();
        },
        on_error
    );
}

// ============================================================================
// Query Operations
// ============================================================================

void MoonrakerAPI::is_printer_ready(BoolCallback on_result,
                                    ErrorCallback on_error) {
    client_.send_jsonrpc("printer.info", json::object(),
        [on_result](json& response) {
            bool ready = false;
            if (response.contains("result") && response["result"].contains("state")) {
                std::string state = response["result"]["state"].get<std::string>();
                ready = (state == "ready");
            }
            on_result(ready);
        },
        on_error
    );
}

void MoonrakerAPI::get_print_state(StringCallback on_result,
                                   ErrorCallback on_error) {
    json params = {
        {"objects", json::object({
            {"print_stats", nullptr}
        })}
    };

    client_.send_jsonrpc("printer.objects.query", params,
        [on_result](json& response) {
            std::string state = "unknown";
            if (response.contains("result") &&
                response["result"].contains("status") &&
                response["result"]["status"].contains("print_stats") &&
                response["result"]["status"]["print_stats"].contains("state")) {
                state = response["result"]["status"]["print_stats"]["state"].get<std::string>();
            }
            on_result(state);
        },
        on_error
    );
}

// ============================================================================
// Private Helper Methods
// ============================================================================

std::vector<FileInfo> MoonrakerAPI::parse_file_list(const json& response) {
    std::vector<FileInfo> files;

    if (!response.contains("result")) {
        return files;
    }

    const json& result = response["result"];

    // Parse directory items
    if (result.contains("dirs")) {
        for (const auto& dir : result["dirs"]) {
            FileInfo info;
            if (dir.contains("dirname")) {
                info.filename = dir["dirname"].get<std::string>();
                info.is_dir = true;
            }
            if (dir.contains("modified")) {
                info.modified = dir["modified"].get<double>();
            }
            if (dir.contains("permissions")) {
                info.permissions = dir["permissions"].get<std::string>();
            }
            files.push_back(info);
        }
    }

    // Parse file items
    if (result.contains("files")) {
        for (const auto& file : result["files"]) {
            FileInfo info;
            if (file.contains("filename")) {
                info.filename = file["filename"].get<std::string>();
            }
            if (file.contains("path")) {
                info.path = file["path"].get<std::string>();
            }
            if (file.contains("size")) {
                info.size = file["size"].get<uint64_t>();
            }
            if (file.contains("modified")) {
                info.modified = file["modified"].get<double>();
            }
            if (file.contains("permissions")) {
                info.permissions = file["permissions"].get<std::string>();
            }
            info.is_dir = false;
            files.push_back(info);
        }
    }

    return files;
}

FileMetadata MoonrakerAPI::parse_file_metadata(const json& response) {
    FileMetadata metadata;

    if (!response.contains("result")) {
        return metadata;
    }

    const json& result = response["result"];

    // Basic file info
    if (result.contains("filename")) {
        metadata.filename = result["filename"].get<std::string>();
    }
    if (result.contains("size")) {
        metadata.size = result["size"].get<uint64_t>();
    }
    if (result.contains("modified")) {
        metadata.modified = result["modified"].get<double>();
    }

    // Slicer info
    if (result.contains("slicer")) {
        metadata.slicer = result["slicer"].get<std::string>();
    }
    if (result.contains("slicer_version")) {
        metadata.slicer_version = result["slicer_version"].get<std::string>();
    }

    // Print info
    if (result.contains("print_start_time")) {
        metadata.print_start_time = result["print_start_time"].get<double>();
    }
    if (result.contains("job_id")) {
        metadata.job_id = result["job_id"].get<double>();
    }
    if (result.contains("layer_count")) {
        metadata.layer_count = result["layer_count"].get<uint32_t>();
    }
    if (result.contains("object_height")) {
        metadata.object_height = result["object_height"].get<double>();
    }
    if (result.contains("estimated_time")) {
        metadata.estimated_time = result["estimated_time"].get<double>();
    }

    // Filament info
    if (result.contains("filament_total")) {
        metadata.filament_total = result["filament_total"].get<double>();
    }
    if (result.contains("filament_weight_total")) {
        metadata.filament_weight_total = result["filament_weight_total"].get<double>();
    }

    // Temperature info
    if (result.contains("first_layer_bed_temp")) {
        metadata.first_layer_bed_temp = result["first_layer_bed_temp"].get<double>();
    }
    if (result.contains("first_layer_extr_temp")) {
        metadata.first_layer_extr_temp = result["first_layer_extr_temp"].get<double>();
    }

    // G-code info
    if (result.contains("gcode_start_byte")) {
        metadata.gcode_start_byte = result["gcode_start_byte"].get<uint64_t>();
    }
    if (result.contains("gcode_end_byte")) {
        metadata.gcode_end_byte = result["gcode_end_byte"].get<uint64_t>();
    }

    // Thumbnails
    if (result.contains("thumbnails")) {
        for (const auto& thumb : result["thumbnails"]) {
            if (thumb.contains("relative_path")) {
                metadata.thumbnails.push_back(thumb["relative_path"].get<std::string>());
            }
        }
    }

    return metadata;
}

std::string MoonrakerAPI::generate_home_gcode(const std::string& axes) {
    if (axes.empty()) {
        return "G28";  // Home all axes
    } else {
        std::ostringstream gcode;
        gcode << "G28";
        for (char axis : axes) {
            gcode << " " << static_cast<char>(std::toupper(axis));
        }
        return gcode.str();
    }
}

std::string MoonrakerAPI::generate_move_gcode(char axis, double distance, double feedrate) {
    std::ostringstream gcode;
    gcode << "G91\n";  // Relative positioning
    gcode << "G0 " << static_cast<char>(std::toupper(axis)) << distance;
    if (feedrate > 0) {
        gcode << " F" << feedrate;
    }
    gcode << "\nG90";  // Back to absolute positioning
    return gcode.str();
}

std::string MoonrakerAPI::generate_absolute_move_gcode(char axis, double position, double feedrate) {
    std::ostringstream gcode;
    gcode << "G90\n";  // Absolute positioning
    gcode << "G0 " << static_cast<char>(std::toupper(axis)) << position;
    if (feedrate > 0) {
        gcode << " F" << feedrate;
    }
    return gcode.str();
}