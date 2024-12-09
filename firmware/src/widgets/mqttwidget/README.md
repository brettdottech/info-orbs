# Using MQTT Widget

To enable MQTTWidget in your setup, do the following:

## Step 1

Add the following to your config.h file
```c
#define MQTT_WIDGET_HOST "192.168.3.40"           // MQTT broker host
#define MQTT_WIDGET_PORT 1883                     // MQTT broker port
#define MQTT_SETUP_TOPIC "info-orbs/setup/orbs"   // Setup topic
#define MQTT_WIDGET_USER "user" // Leave empty if authentication is not required
#define MQTT_WIDGET_PASS "pass" // Leave empty if authentication is not required
```

## Step 2

Publish the following to the MQTT topic you defined above.
(Update the below to suit your setup:)

```json
{
  "orbs": [
    {
      "orbid": 0,
      "orbdesc": "House battery",
      "orb-bg": "TFT_SILVER",
      "orb-textcol": "TFT_BLACK",
      "topicsrc": "hamqtt/sensor/solis_battery_soc/state",
      "xpostxt": 115,
      "ypostxt": 100,
      "xposval": 130,
      "yposval": 140,
      "orbsize": 20,
      "orbvalunit": "%"
    },
    {
      "orbid": 1,
      "orbdesc": "Solar yield",
      "orb-bg": "TFT_BLACK",
      "orb-textcol": "TFT_WHITE",
      "topicsrc": "hamqtt/sensor/solis_power_generation_today/state",
      "xpostxt": 110,
      "ypostxt": 100,
      "xposval": 130,
      "yposval": 140,
      "orbsize": 20,
      "orbvalunit": "kWh"
    },
    {
      "orbid": 2,
      "orbdesc": "PV Solar Today",
      "orb-bg": "TFT_BLUE",
      "orb-textcol": "TFT_WHITE",
      "topicsrc": "hamqtt/sensor/solcast_pv_forecast_forecast_today/state",
      "xpostxt": 110,
      "ypostxt": 100,
      "xposval": 130,
      "yposval": 140,
      "orbsize": 20,
      "orbvalunit": "kWh"
    },
    {
      "orbid": 3,
      "orbdesc": "PV Solar Today+",
      "orb-bg": "TFT_BLUE",
      "orb-textcol": "TFT_WHITE",
      "topicsrc": "hamqtt/sensor/solcast_pv_forecast_forecast_tomorrow/state",
      "xpostxt": 110,
      "ypostxt": 100,
      "xposval": 130,
      "yposval": 140,
      "orbsize": 20,
      "orbvalunit": "kWh"
    },
    {
      "orbid": 4,
      "orbdesc": "Garage temp",
      "orb-bg": "TFT_BLUE",
      "orb-textcol": "TFT_WHITE",
      "topicsrc": "zigbee2mqtt/Garage Temp2",
      "xpostxt": 110,
      "ypostxt": 100,
      "xposval": 130,
      "yposval": 140,
      "orbsize": 20,
      "orbvalunit": "°c",
      "jsonfield": "temperature"
    }
  ]
}
```

**Note:** For Orbid 4, there's an additional jsonfield that is used to extract a value from a JSON payload returned from the topic `zigbee2mqtt/Garage Temp2`.

Example 1 payload:
```json
{
    "battery": 100,
    "humidity": 56.7,
    "linkquality": 69,
    "temperature": 17.8
}
```

To extract `temperature`, use: `"jsonfield": "temperature"`

Example 2 payload:
```json
{
    "brightness": 132,
    "linkquality": 140,
    "update": {
        "state": "idle"
    }
}
```

To extract `state`, use: `"jsonfield": "update.state"`


Example 3 payload:
```json
{
    "power_delivered": [
        {
            "value": 0.321,
            "unit": "kW"
        }
    ]
}
```

To extract `value`, use: `"jsonfield": "power_delivered[0].value"`
