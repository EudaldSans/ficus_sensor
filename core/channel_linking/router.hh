#ifndef CHANNEL_LINKING_ROUTER_HH
#define CHANNEL_LINKING_ROUTER_HH

#include <tuple>

#include "fic_log.hh"

#include "channels.hh"

#include "task.hh"

template <typename... Links>
class Router : public IContinuousTask {
public:
    constexpr Router(Links... l) : _links(l...) {}

    void setup() override {}

    /**
     * @brief Unfolds the list of links at compile time and syncs them all one by one
     * 
     * @param now The time of update execution
     */
    void update(uint64_t now) override {
        std::apply([](auto&... link) {
            (link.sync(), ...);
        }, _links);
    }

private:
    std::tuple<Links...> _links;
};

#endif