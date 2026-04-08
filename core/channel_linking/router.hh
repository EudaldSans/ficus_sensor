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
        for (size_t i = 0; i < _count; ++i) {
            if (_links[i]) {
                _links[i]->sync();
            }
        }
    }

private:
    ILink* const* _links;
    const size_t _count;
};

#endif