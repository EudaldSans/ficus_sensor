#ifndef COMPOSITION_HH
#define COMPOSITION_HH

#include "channels.hh"
#include "rgb_signalling.hh"
#include "wifi_hal.hh"

#include "sntp_client.hh"

#include "button.hh"
#include "esp_gpio.hh"

class TaskManager;

extern RGBSignaler&     rgb_signaler;
extern EspSntpClient&   sntp_client;

extern ButtonBase& boot_button;

WiFiState composition_get_wifi_state();

void composition_init_hardware();
void composition_add_tasks(TaskManager& tm);
void composition_add_hw_tasks(TaskManager&tm);
void composition_start_comms();

#endif
