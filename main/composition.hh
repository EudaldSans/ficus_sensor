#ifndef COMPOSITION_HH
#define COMPOSITION_HH

#include "channels.hh"
#include "rgb_signalling.hh"
#include "wifi_hal.hh"

class TaskManager;

extern RGBSignaler&    rgb_signaler;
extern value_t<int>&   firebase_t_input;
extern value_t<int>&   firebase_h_input;

WiFiState composition_get_wifi_state();

void composition_init_hardware();
void composition_add_tasks(TaskManager& tm);
void composition_start_comms();

#endif
