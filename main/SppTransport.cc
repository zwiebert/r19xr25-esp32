#include "SppTransport.hh"
#include "bt_classic/spp_acceptor.hh"

void SppTransport::start(ReadCallback cb)
{
    spp_main();
}

void SppTransport::stop()
{
    // Implementation for stopping SPP transport, if necessary
}