// BleTransport.hh
#pragma once

#include "Transport.hh"

// platform‑specific SPP reader (Bluedroid)
class SppTransport : public Transport {
public:
    void start(ReadCallback cb) override;
    void stop() override;
};