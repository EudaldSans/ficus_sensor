#ifndef COMPOSITION_HH
#define COMPOSITION_HH

#include "channels.hh"
#include "rgb_signalling.hh"
#include "wifi_hal.hh"

#include "sntp_client.hh"

#include "button.hh"
#include "esp_gpio.hh"

using BootPinGpio = EspGPIO<GPIO_NUM_9, GPIO_MODE_INPUT, PULL_DISABLED>;
using BootButton = Button<BootPinGpio>;

class TaskManager;

extern RGBSignaler&     rgb_signaler;
extern EspSntpClient&   sntp_client;

extern BootButton& boot_button;

WiFiState composition_get_wifi_state();

void composition_init_hardware();
void composition_add_tasks(TaskManager& tm);
void composition_start_comms();

#endif
