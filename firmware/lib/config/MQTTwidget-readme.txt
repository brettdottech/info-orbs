To enable MQTTWidget in your setup, do the following:

Step 1 - add the following to your config.h file
------
#define MQTT_WIDGET_HOST "192.168.3.40" // MQTT broker host
#define MQTT_WIDGET_PORT 1883 // MQTT broker port
#define MQTT_SETUP_TOPIC "info-orbs/setup/orbs" // Setup topic
#define MQTT_WIDGET_USER "user" // Leave empty if authentication is not required
#define MQTT_WIDGET_PASS "pass" // Leave empty if authentication is not required


Step 2 - publish the following to your MQTT topic as defined above.  Update the below to suit your setup:

{
  "orbs": [
    {
      "orbid": 0,
      "orbdesc": "House battery",
      "orb-bg": "TFT_SILVER",
      "orb-textcol": "TFT_BLACK",
      "topicsrc": "hamqtt/sensor/solis_battery_soc/state",
      "xpostxt": 110,
      "ypostxt": 1,
      "xposval": 140,
      "yposval": 1,
      "orbsize": 3,
      "orbvalunit": "%"
    },
    {
      "orbid": 1,
      "orbdesc": "Solar yield",
      "orb-bg": "TFT_BLUE",
      "orb-textcol": "TFT_WHITE",
      "topicsrc": "hamqtt/sensor/solis_power_generation_today/state",
      "xpostxt": 110,
      "ypostxt": 1,
      "xposval": 140,
      "yposval": 1,
      "orbsize": 3,
      "orbvalunit": "kWh"
    },
    {
      "orbid": 2,
      "orbdesc": "PV Solar Tdy",
      "orb-bg": "TFT_BLUE",
      "orb-textcol": "TFT_WHITE",
      "topicsrc": "hamqtt/sensor/solcast_pv_forecast_forecast_today/state",
      "xpostxt": 110,
      "ypostxt": 1,
      "xposval": 140,
      "yposval": 1,
      "orbsize": 3,
      "orbvalunit": "kWh"
    },
    {
      "orbid": 3,
      "orbdesc": "PV Solar Tdy+",
      "orb-bg": "TFT_BLUE",
      "orb-textcol": "TFT_WHITE",
      "topicsrc": "hamqtt/sensor/solcast_pv_forecast_forecast_tomorrow/state",
      "xpostxt": 110,
      "ypostxt": 1,
      "xposval": 140,
      "yposval": 1,
      "orbsize": 3,
      "orbvalunit": "kWh"
    },
    {
      "orbid": 4,
      "orbdesc": "Garage temp",
      "orb-bg": "TFT_BLUE",
      "orb-textcol": "TFT_WHITE",
      "topicsrc": "zigbee2mqtt/Garage Temp2",
      "xpostxt": 110,
      "ypostxt": 1,
      "xposval": 140,
      "yposval": 1,
      "orbsize": 3,
      "orbvalunit": "'c",
      "jsonfield": "temperature"
    }
  ]
}

Note: for Orbid 4, there's an additional jsonfield that is used to extract a value from a JSON payload returned from the topic zigbee2mqtt/Garage Temp2.  It looks like this:

{
  "battery": 100,
  "humidity": 56.7,
  "linkquality": 69,
  "temperature": 17.8,
}

------