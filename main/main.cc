// main.cc
#include <stdio.h>
#include <string.h>

#include <chrono>
#include <iostream>
#include <thread>

#include "FrameProcessor.hh"
#include "bt_classic/spp_acceptor.hh"

std::vector<uint8_t> hexStringToByteArray(const std::string& hex) {
  std::vector<uint8_t> bytes;

  for (unsigned int i = 0; i < hex.length(); i += 2) {
    std::string byteString = hex.substr(i, 2);
    uint8_t byte = (uint8_t)strtol(byteString.c_str(), NULL, 16);
    bytes.push_back(byte);
  }

  return bytes;
}

bool test2(FrameProcessor& processor) {
  processor.feedBytes(hexStringToByteArray("ff00107710447d79bf1aa45fc608080c"));
  processor.feedBytes(hexStringToByteArray("0073ffff0100006b3004048079811288"));
  processor.feedBytes(hexStringToByteArray("ff00107710437d79ba19705fc608040c"));
  processor.feedBytes(hexStringToByteArray("0073ffff0100006b3004048079811288"));
  processor.feedBytes(hexStringToByteArray("ff00107710437d79b8198c5fc408050c"));

  return true;
}

R19Frame R19_frame;

inline const char* btoa(bool v) { return v ? "Y" : "N"; }
constexpr unsigned FORCE_IVAL_S = 60;
using r19frame_mask_t = std::bitset<32>;

int spp_write_r19_frame(char* dst, size_t dst_siz, r19frame_mask_t mask,
                        bool force = false) {
  ssize_t dst_size = ssize_t(dst_siz);
  static R19Frame c;        // copy of last written frame
  static time_t last_time;  // last time when data was successfully written
  int ct = 0;
  const auto& d = R19_frame;
  r19frame_mask_t changed_mask = 0;

  {
    unsigned bit = 0;
    changed_mask.set(bit, (c.EngineSpeed_RPM != d.EngineSpeed_RPM));
    ++bit, changed_mask.set(bit, (c.MAP_mBar != d.MAP_mBar));
    ++bit, changed_mask.set(bit, (c.IAT_Celsius != d.IAT_Celsius));
    ++bit, changed_mask.set(bit, (c.ECT_Celsius != d.ECT_Celsius));
    ++bit, changed_mask.set(bit, (c.O2_Sensor_mV != d.O2_Sensor_mV));
    ++bit, changed_mask.set(bit, (c.AP_mBar != d.AP_mBar));
    ++bit, changed_mask.set(bit, (c.BatteryVoltage_V != d.BatteryVoltage_V));
    ++bit, changed_mask.set(bit, (c.ID_msec != d.ID_msec));
    ++bit, changed_mask.set(bit, (c.isThrottleOpen != d.isThrottleOpen));
    ++bit, changed_mask.set(bit, (c.isThrottleClosed != d.isThrottleClosed));
    ++bit, changed_mask.set(bit, (c.isAGR_AKF != d.isAGR_AKF));
    ++bit, changed_mask.set(bit, (c.IdleSpeedCorr != d.IdleSpeedCorr));
    ++bit, changed_mask.set(bit, (c.EngineKnocking != d.EngineKnocking));
  }
  bool data_changed = (changed_mask & mask).any();
  bool hide_unchanged = !force && true;  ///< control which members to show
  r19frame_mask_t view_mask = hide_unchanged ? (changed_mask & mask) : mask;
  if (last_time + FORCE_IVAL_S < time(0)) view_mask = ~0UL; // XXX

  if (view_mask.any()) {
    unsigned bit = 0;
    auto dst_max = dst + dst_size - 1;

    if (ct >= 0 && view_mask.test(bit++)) {
      auto p = std::min(dst_max, dst + ct);
      auto l = std::max(ssize_t(0), dst_size - ct);
      ct += snprintf(p, l, "%u: rpm=%d\r\n", bit, d.EngineSpeed_RPM);
    }

    if (ct >= 0 && view_mask.test(bit++)) {
      auto p = std::min(dst_max, dst + ct);
      auto l = std::max(ssize_t(0), dst_size - ct);
      ct += snprintf(p, l, "%u: map=%d mBar\r\n", bit, d.MAP_mBar);
    }

    if (ct >= 0 && view_mask.test(bit++)) {
      auto p = std::min(dst_max, dst + ct);
      auto l = std::max(ssize_t(0), dst_size - ct);
      ct += snprintf(p, l, "%u: IAT=%d °C\r\n", bit, d.IAT_Celsius);
    }

    if (ct >= 0 && view_mask.test(bit++)) {
      auto p = std::min(dst_max, dst + ct);
      auto l = std::max(ssize_t(0), dst_size - ct);
      ct += snprintf(p, l, "%u: ECT=%d °C\r\n", bit, d.ECT_Celsius);
    }

    if (ct >= 0 && view_mask.test(bit++)) {
      auto p = std::min(dst_max, dst + ct);
      auto l = std::max(ssize_t(0), dst_size - ct);
      ct += snprintf(p, l, "%u: O2=%d mV\r\n", bit, d.O2_Sensor_mV);
    }

    if (ct >= 0 && view_mask.test(bit++)) {
      auto p = std::min(dst_max, dst + ct);
      auto l = std::max(ssize_t(0), dst_size - ct);
      ct += snprintf(p, l, "%u: AP=%d mBar\r\n", bit, d.AP_mBar);
    }

    if (ct >= 0 && view_mask.test(bit++)) {
      auto p = std::min(dst_max, dst + ct);
      auto l = std::max(ssize_t(0), dst_size - ct);
      ct += snprintf(p, l, "%u: Batt=%0.1f V\r\n", bit, d.BatteryVoltage_V);
    }

    if (ct >= 0 && view_mask.test(bit++)) {
      auto p = std::min(dst_max, dst + ct);
      auto l = std::max(ssize_t(0), dst_size - ct);
      ct += snprintf(p, l, "%u: ID=%0.2f ms\r\n", bit, d.ID_msec);
    }

    if (ct >= 0 && view_mask.test(bit++)) {
      auto p = std::min(dst_max, dst + ct);
      auto l = std::max(ssize_t(0), dst_size - ct);
      ct += snprintf(p, l, "%u: TFO=%s\r\n", bit, btoa(d.isThrottleOpen));
    }

    if (ct >= 0 && view_mask.test(bit++)) {
      auto p = std::min(dst_max, dst + ct);
      auto l = std::max(ssize_t(0), dst_size - ct);
      ct += snprintf(p, l, "%u: TC=%s\r\n", bit, btoa(d.isThrottleClosed));
    }

    if (ct >= 0 && view_mask.test(bit++)) {
      auto p = std::min(dst_max, dst + ct);
      auto l = std::max(ssize_t(0), dst_size - ct);
      ct += snprintf(p, l, "%u: EGR/EVap=%s\r\n", bit, btoa(d.isAGR_AKF));
    }

    if (ct >= 0 && view_mask.test(bit++)) {
      auto p = std::min(dst_max, dst + ct);
      auto l = std::max(ssize_t(0), dst_size - ct);
      ct += snprintf(p, l, "%u: IdleCorr=%d\r\n", bit, d.IdleSpeedCorr);
    }

    if (ct >= 0 && view_mask.test(bit++)) {
      auto p = std::min(dst_max, dst + ct);
      auto l = std::max(ssize_t(0), dst_size - ct);
      ct += snprintf(p, l, "%u: Knock=%d\r\n", bit, d.EngineKnocking);
    }

    if (ct >= 0) {
      auto p = std::min(dst_max, dst + ct);
      auto l = std::max(ssize_t(0), dst_size - ct);
      ct += snprintf(p, l, "\r\n");
    }

    if (ct >= dst_size || ct < 0)
      return ct;  // ERROR: data was not fully written (e.g. buffer too small)

    // all done. update our local state variables
    c = R19_frame;
    last_time = time(0);
  }

  return ct;
}

std::bitset<32> Mask = ~0LU;
bool Force;

int spp_write_cb(char* dst, size_t dst_size, spp::status_t& status) {
  int ret = 0;

  ret = spp_write_r19_frame(dst, dst_size, Mask,
                            status.test(spp::JUST_CONNECTED) || Force);
  Force = false;
  if (0 < ret && ret < dst_size) return ret;

  return ret;
}

struct CliCmd {
  const char* name = 0;
  bool (*handler)(struct CliCmd& cli_cmd) = 0;
  char* args = 0;
  char* dst = 0;
  size_t dst_size = 0;

  bool execute(char* cmd_line_buf, size_t buf_size) {
    if (strstr(cmd_line_buf, name) != cmd_line_buf) return false;
    args = cmd_line_buf + strlen(name);
    dst = cmd_line_buf;
    dst_size = buf_size;  ///< in: size of buf, out: length of reply data/string
    if (handler) return handler(*this);

    dst_size = 0;
    return true;
  }
};

#include <esp_log.h>

const std::array<CliCmd, 1> cmds = {
    {"filter ",
     [](CliCmd& cmd) -> bool {
       // command line was like: "filter 1,2,3,8,12".
       // cmd.args is now "1,2,3,8,12"
       // we need to convert this into a bitset<32> with only bits 0,1,2,7,11
       // are set to true.
       // TODO: write a function to do the above
       r19frame_mask_t mask;
       for (char *str = cmd.args, *save_ptr = nullptr, *tok;
            (tok = strtok_r(str, ", ", &save_ptr)); str = nullptr) {
         ESP_LOGI("db", "token: %s", tok);
         mask.set(strtoul(tok, nullptr, 10) - 1);
       }
       Mask = mask;
       Force = true;

       cmd.dst_size =
           snprintf(cmd.dst, cmd.dst_size,
                    "We made it to the filter handler... yippie\r\n");
       return true;
     }},
};

std::size_t spp_read_cb(char* src_dst, std::size_t src_len,
                        std::size_t dst_size, spp::status_t& status) {
  printf("spp-in: %.*s", src_len, src_dst);

  for (auto cmd : cmds) {
    if (cmd.execute(src_dst, dst_size)) return cmd.dst_size;
  }

  // we are here, because no command handler matched
  return snprintf(src_dst, dst_size, "Thanks for your input, friend :)\r\n");
}

extern "C" int app_main() {
  FrameProcessor processor([](const std::string& hex, R19Frame&& r19_frame) {
    R19_frame = r19_frame;  // store latest frame for later use
    std::cout << "HEX: " << hex << "\n";
    std::cout << r19_frame.getDataAsText() << "\n----------------\n";
  });

  test2(processor);

  spp_main(spp_read_cb, spp_write_cb);

#if 0
    UsbTransport usb;

    usb.start([&processor](auto data){ processor.feedBytes(data); });

    // simple loop – replace with real event loop or RTOS task
    while (true) {
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }

    spp.stop();
#endif

  return 0;
}

int main() { return app_main(); }