#ifndef COMPOSITION_HH
#define COMPOSITION_HH

#include "channels.hh"
#include "rgb_signalling.hh"
#include "wifi_hal.hh"

#include "wifi_controller.hh"

class TaskManager;

extern RGBSignaler&    rgb_signaler;
extern WiFiController& wifi_controller_ref;

WiFiState composition_get_wifi_state();

void composition_init_hardware();
void composition_add_tasks(TaskManager& tm);
void composition_start_comms();

#endif
