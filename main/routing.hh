#ifndef ROUTING_H
#define ROUTING_H

#include "router.hh"

value_t<float> t_sensor_output;
value_t<float> h_sensor_output;

value_t<int> firebase_t_input;
value_t<int> firebase_h_input;

static ChannelLink<float, int> temperature_link{t_sensor_output, firebase_t_input};
static ChannelLink<float, int> humidity_link{h_sensor_output, firebase_h_input};

static ILink* master_link_list[] = {
    &temperature_link,
    &humidity_link,
};

Router router(master_link_list, sizeof(master_link_list) / sizeof(ILink*));

#endif