#pragma once

#include <bitset>
#include <cstdint>

namespace spp {
constexpr std::size_t JUST_CONNECTED = 1;
using status_t = std::bitset<16>;
}  // namespace spp

/**
 * @brief this callback is called periodically to get data to write to remote client
 *
 */
typedef int (*spp_write_cb_t)(char* dst, std::size_t dst_size,
                              spp::status_t& status);
/**
 * @brief this callback is called when data was read from remote client
 * @param src_dst      the data buffer
 * @param src_len      number of bytes received
 * @param dst_size     the size of src_dst usable for holding the reply data
 * 
 * @returns            byte length of reply (0..dst_size)
 *
 */
typedef std::size_t (*spp_read_cb_t)(char* src_dst, std::size_t src_len,
                              std::size_t dst_size, spp::status_t& status);

void spp_main(spp_read_cb_t rcb, spp_write_cb_t wcb);
