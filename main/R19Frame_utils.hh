#pragma once
#include "R19Frame.hh"
#include <bitset>


using r19frame_mask_t = std::bitset<32>;

inline const char* btoa(bool v) { return v ? "Y" : "N"; }

/// @brief Conditional print members of R19Frame object
/// @param dst       write buffer
/// @param dst_siz   write buffer size
/// @param d         object
/// @param view_mask bitmask representing members. Only members with 1 bits are
/// printed.
/// @return  bytes written or if greater than dst_siz, the required buffer size
/// (man 3 snprintf)
inline int r19_frame_print(char* dst, size_t dst_siz, const R19Frame& d,
                    r19frame_mask_t view_mask) {
  ssize_t dst_size = ssize_t(dst_siz);
  int ct = 0;

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
  }

  return ct;
}

/// @brief      compare all members of 2 R19Frame objects
/// @param c    object
/// @param d    object
/// @return     bitset where none differing members are represented by zero
/// bits. order of bits is order of member definition.
inline r19frame_mask_t r19_frame_members_cmp(const R19Frame& c, const R19Frame& d) {
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
  return changed_mask;
}