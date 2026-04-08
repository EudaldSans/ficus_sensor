#ifndef CHANNEL_LINKING_ROUTER_HH
#define CHANNEL_LINKING_ROUTER_HH

#include "fic_log.hh"

#include "channels.hh"

#include "task.hh"

class Router : public IContinuousTask {
public:
    Router(ILink* const* links, size_t count) : _links(links), _count(count) {}

    void setup() override {}

    void update(uint64_t now) override {
        std::apply([](auto&... link_items) {
            (link_items.sync(), ...); 
        }, _links);
    }

private:
    ILink* const* _links; // A pointer to a constant array of pointers
    const size_t _count;
};

#endif