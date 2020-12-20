//By Jonathan Williams (z5162987)
//res-rssi resource responds with the rssi of the request
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "contiki.h"
#include "contiki-net.h"
#include "rest-engine.h"

#if PLATFORM_HAS_BUTTON
#include "dev/button-sensor.h"
#endif

#define DEBUG 0
#if DEBUG
#include <stdio.h>
#define PRINTF(...) printf(__VA_ARGS__)
#define PRINT6ADDR(addr) PRINTF("[%02x%02x:%02x%02x:%02x%02x:%02x%02x:%02x%02x:%02x%02x:%02x%02x:%02x%02x]", ((uint8_t *)addr)[0], ((uint8_t *)addr)[1], ((uint8_t *)addr)[2], ((uint8_t *)addr)[3], ((uint8_t *)addr)[4], ((uint8_t *)addr)[5], ((uint8_t *)addr)[6], ((uint8_t *)addr)[7], ((uint8_t *)addr)[8], ((uint8_t *)addr)[9], ((uint8_t *)addr)[10], ((uint8_t *)addr)[11], ((uint8_t *)addr)[12], ((uint8_t *)addr)[13], ((uint8_t *)addr)[14], ((uint8_t *)addr)[15])
#define PRINTLLADDR(lladdr) PRINTF("[%02x:%02x:%02x:%02x:%02x:%02x]", (lladdr)->addr[0], (lladdr)->addr[1], (lladdr)->addr[2], (lladdr)->addr[3], (lladdr)->addr[4], (lladdr)->addr[5])
#else
#define PRINTF(...)
#define PRINT6ADDR(addr)
#define PRINTLLADDR(addr)
#endif


extern resource_t res_event, res_separate;
static struct uip_udp_conn *server_conn;

static void res_get_handler(void *request, void *response, uint8_t *buffer, uint16_t preferred_size, int32_t *offset);
//rssi resource
RESOURCE(res_rssi,"title=\"RSSI: ?len=0..\";rt=\"Text\"",res_get_handler,NULL,NULL,NULL);

PROCESS(er_example_server, "Erbium Example Server");
AUTOSTART_PROCESSES(&er_example_server);

//handles get requests 
//sends RSSI of request back to client
static void
res_get_handler(void *request, void *response, uint8_t *buffer, uint16_t preferred_size, int32_t *offset)
{
  char msg[2];
  int length = sizeof(msg);
  if(uip_newdata()) {
	//copies rssi to msg and sends back source of request    
	sprintf(msg, "%d", (signed short)packetbuf_attr(PACKETBUF_ATTR_RSSI));
    memcpy(buffer, msg, sizeof(msg));
    REST.set_header_content_type(response, REST.type.TEXT_PLAIN);
    REST.set_header_etag(response, (uint8_t *)&length, 1);
    REST.set_response_payload(response, buffer, length);
    printf("%s\n",(char*)buffer);
    memset(&buffer, 0, sizeof(msg));
  }
}

PROCESS_THREAD(er_example_server, ev, data)
{
  PROCESS_BEGIN();

  PROCESS_PAUSE();

  rest_init_engine();

  rest_activate_resource(&res_rssi, "net/parent/RSSI");
  
  server_conn = udp_new(NULL, UIP_HTONS(3001), NULL);
  udp_bind(server_conn, UIP_HTONS(3001));
  /* Define application-specific events here. */
  while(1) {
    PROCESS_WAIT_EVENT();
#if PLATFORM_HAS_BUTTON
    if(ev == sensors_event && data == &button_sensor) {
      /* Call the event_handler for this application-specific event. */
      res_event.trigger();
      /* Also call the separate response example handler. */
      res_separate.resume();
    }
#endif /* PLATFORM_HAS_BUTTON */
  }                         

  PROCESS_END();
}
