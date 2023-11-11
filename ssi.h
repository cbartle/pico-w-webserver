#include "lwip/apps/httpd.h"
#include "pico/cyw43_arch.h"
#include "hardware/adc.h"
#include "mpl3115a2.h"

// SSI tags - tag length limited to 8 bytes by default
const char * ssi_tags[] = {"volt","temp","led", "mplf", "mplc", "mpla"};
float temp_f = 0.00;
float temp_c = 0.00;
float altitude_m = 0.00;
u16_t ssi_handler(int iIndex, char *pcInsert, int iInsertLen) {
  size_t printed;
  if(has_new_data){
    read_data();
    temp_f = get_farenheit_temp();
    temp_c = get_celcius_temp();
    altitude = get_altitude();
  }

  switch (iIndex) {
    case 0: // volt
      {
        const float voltage = adc_read() * 3.3f / (1 << 12);
        printed = snprintf(pcInsert, iInsertLen, "%f", voltage);
      }
      break;
    case 1: // temp
      {
      const float voltage = adc_read() * 3.3f / (1 << 12);
      const float tempF = ((27.0f - (voltage - 0.706f) / 0.001721f) * 9 / 5) + 32;
      printed = snprintf(pcInsert, iInsertLen, "%.2f", tempF);
      }
      break;
    case 2: // led
      {
        bool led_status = cyw43_arch_gpio_get(CYW43_WL_GPIO_LED_PIN);
        if(led_status == true){
          printed = snprintf(pcInsert, iInsertLen, "ON");
        }
        else{
          printed = snprintf(pcInsert, iInsertLen, "OFF");
        }
      }
      break;
    case 3:
      {
        printed = snprintf(pcInsert, iInsertLen, "%.2f", temp_f);
      }
      break;
    case 4:
      {
        printed = snprintf(pcInsert, iInsertLen, "%.2f", temp_c);
      }
      break;
    case 5:
      {
        printed = snprintf(pcInsert, iInsertLen, "%.2f", altitude);
      } 
      break;
    default:
      printed = 0;
      break;
  }

  return (u16_t)printed;
}

// Initialise the SSI handler
void ssi_init() {
  // Initialise ADC (internal pin)
  adc_init();
  adc_set_temp_sensor_enabled(true);
  adc_select_input(4);

  http_set_ssi_handler(ssi_handler, ssi_tags, LWIP_ARRAYSIZE(ssi_tags));
  sensor_init();
}