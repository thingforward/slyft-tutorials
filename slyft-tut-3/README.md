# Handling multiple CoAP endpoints

The [first tutorial]() revolved around the usage of the slyft client and a simple
binklr example, where one can send a MessagePack-formatted message to a Device
and thus controlling its blinking frequency.

In this example we'd like to extend the functionality of the device a bit
and introduce more endpoints and data to the API.

Image a small device with
* a switch, to turn off an on,
* an LED and
* an analog sensor, i.e. a rotary.

We'd like to define a range of analog values (of the sensor). When the sensor
reports a value in range, LED should be turned on, off otherwise.

A client should be capable to
* configure this range,
* query the range currently active and
* query the current state (value of sensor plus state of LED).

We're using slyft to build the CoAP API, and put it on an ESP8266.

## Prerequisites

Please make sure you have the slyft command line client installed. If not, please
do so. You can find detailed instructions on [the documentation pages](http://www.slyft.io/docs/install/). Please also [register](http://www.slyft.io/docs/register/) to be able to use the slyft service.

For building the example, we recommend an ESP8266-based board such as NodeMCU,
and [PlatformIO](platformio.org) to build the code and work with devices. In our
examples we use the Grove Kit for prototyping. Slyft does not build specific code
for these components, so you're free to choose whatever sensors you like. But make
sure you adapt the tutorial code where necessary.

## Project directory & slyft project creation

Start by making a new work directory and `cd` there:

```
$ mkdir slyft-tut-3
$ cd slyft-tut-3
```

Next step is to create a slyft project:

```
$ slyft project create --name slyft-tut-3

Project Name: slyft-tut-3
Details to the project (optional): 3rd tutorial

---- Project Details ----
     KEY    |             VALUE
+-----------+-------------------------------+
  Name      | slyft-tut-3
  Details   | 3rd tutorial
  CreatedAt | 2017-03-03 14:26:52 +0000 UTC
  UpdatedAt | 2017-03-03 14:26:52 +0000 UTC
  Settings  | {}
```

As this becomes the default slyft project for the current directory, we can
put its name to a `.slyftproject` file, so that the client knows what we're dealing with:

```
$ echo 'slyft-tut-3' >.slyftproject
```

And we also create the embedded project using PlatformIO. Please check the `-b`
parameter and adapt to your board, or create a project with any other IDE of your
choice.

```
$ pio init -b nodemcu2
```

## API

Lets start by thinking about our desired API structure.
The API capabilities from the description above can be transformed into a RAML-like snippet:

```yaml
#%RAML 0.8
title: tutorial3
/state:
  get:
    description: Return device state
    responses:
      205:
/config:
  get:
    description: Return device configuration
    responses:
      205:
  post:
    description: Update device configuration
    responses:
      204:
        description: Changed device configuration
      406:
        description: Configuration request not acceptable
```

The above snippet defines two endpoints, `/state` and `/config`. `/state` allows
for a CoAP GET request, returning a 2.05 CONTENT message with the current state.
`/config` allows for both GET and POST, where GET returns the configuration as a
2.05 message, and POST returns either 2.04 CHANGED when a new configuration has
been received by a client and stored in the device, or a 4.06 NOT ACCEPTABLE when
the configuration request is not valid (i.e. a value less that zero).

Slyft allows us to specify the data structures as well. For state, we'd like to
transfer the switch state as a boolean plus the current state of the LED, so it's:

```json
{
    "$schema": "http://json-schema.org/draft-04/schema#",
    "id": "state",
    "title": "Device state",
    "description": "",
    "type": "object",
    "properties" : {
      "switch_on": {
	       "type": "boolean"
	    },
	    "led1": {
        "type": "string",
        "maxLength": 3
      }
    },
    "required": ["switch_on", "led1" ]
}
```

Put the above contents in a new file called `state-schema.json`. The configuration
endpoint shall understand the following schema:

```
{
    "$schema": "http://json-schema.org/draft-04/schema#",
    "id": "config",
    "title": "Device config",
    "description": "",
    "type": "object",
    "properties" : {
      "lower_bound": {
        "type": "integer",
        "minimum": 0,
        "maximum": 1023
      },
      "upper_bound": {
        "type": "integer",
        "minimum": 0,
        "maximum": 1023
      }
    },
    "required": ["lower_bound", "upper_bound" ]
}
```  

Put this schema in a file named `config-schema.json`. Looking back at the RAML
API definition above, we can insert reference to both schemas in the API endpoints.
Put the following RAML contents in a file called `api.raml`:

```
#%RAML 0.8
title: tutorial3
baseUri: coap://api.sylft.io/
version: 0.1
schemas:
  - state: state-schema.json
  - config: config-schema.json
/state:
  get:
    description: Return device state
    responses:
      205:
        body:
          application/x-msgpack:
            schema: state
/config:
  get:
    description: Return device configuration
    responses:
      205:
        body:
          application/x-msgpack:
            schema: config
  post:
    description: Update device configuration
    body:
      application/x-msgpack:
        schema: config
    responses:
      204:
        description: Changed device configuration
      406:
        description: Configuration request not acceptable
```

## Validate and build the project

Before added the files to our slyft project, we need to tell slyft what to
generate with it. We're again using the slyftlet `c99-arduino-coap-msgpack`,
so create a file named `c99-arduino-coap-msgpack.json` and place the following
configuration entries inside:

```
{
  "c_define_prefix": "tut3",
  "c_identifier_prefix": "TUT3",
  "source_file_extension": "cpp"
}
```

Now we can add all the files to our slyft project using the `slyft asset` subcommand:

```
$ slyft asset add --file api.raml
$ slyft asset add --file config-schema.json
$ slyft asset add --file state-schema.json
$ slyft asset add --file c99-arduino-coap-msgpack.json
```

Your slyft project should now look like this:

```
$ slyft asset list

  NUMBER |             NAME              | PROJECT NAME | ORIGIN
+--------+-------------------------------+--------------+--------+
       1 | api.raml                      | slyft-tut-3  | user
       2 | config-schema.json            | slyft-tut-3  | user
       3 | state-schema.json             | slyft-tut-3  | user
       4 | c99-arduino-coap-msgpack.json | slyft-tut-3  | user
```

A run to `$ slyft project validate` should come back with a **successful**
result message. If not, please check the error message and compare the file
contents.

Next up, build the project:

```
$ slyft project build
(...)
Waiting (30s) for job completion....
---- Job Details ----
       KEY      |             VALUE
+---------------+-------------------------------+
  Id            | 190
  Kind          | build
  Status        | processed
  ProjectId     | 74
  ProjectName   | slyft-tut-3
  CreatedAt     | 2017-03-03 14:37:51 +0000 UTC
  UpdatedAt     | 2017-03-03 14:37:59 +0000 UTC
  ResultMessage | Successful
  ResultStatus  | 0
```

Good, you should now have a file `all.tar` in your asset list:

```
$ slyft asset list

  NUMBER |             NAME              | PROJECT NAME |  ORIGIN
+--------+-------------------------------+--------------+----------+
(...)
       5 | all.tar                       | slyft-tut-3  | slyftlet
```

Download this asset, and unpack it to the `src/` folder:

```
$ slyft asset get --file all.tar
Downloaded all.tar

$ tar xfv all.tar -C src/
```

## Building the firmware for Arduino platforms

This part is very similar to the one in the first tutorial. You might want to copy
over the files and directories from there, or create them fresh:

For this project to compile we need to fulfil some library dependencies, namely microcoap and msgpack for c. Clone the repositories [https://github.com/1248/microcoap](https://github.com/1248/microcoap) and [https://github.com/ludocode/mpack](https://github.com/ludocode/mpack) from github into a `vendor/` directory, and copy over some files to the `lib/private_lib/` folder (this
  applies to PlatformIO projects. For other IDEs, adapt path settings as necessary):

```
$ mkdir vendor
$ cd vendor

$ git clone https://github.com/1248/microcoap
Cloning into 'microcoap'...
remote: Counting objects: 172, done.
remote: Total 172 (delta 0), reused 0 (delta 0), pack-reused 172
Receiving objects: 100% (172/172), 35.63 KiB | 0 bytes/s, done.
Resolving deltas: 100% (91/91), done.

$ mkdir -p ../lib/private_lib/microcoap
$ cp microcoap/coap.c ../lib/private_lib/microcoap
$ cp microcoap/coap.h ../lib/private_lib/microcoap


$ git clone https://github.com/ludocode/mpack
Cloning into 'mpack'...
remote: Counting objects: 3957, done.
remote: Total 3957 (delta 0), reused 0 (delta 0), pack-reused 3957
Receiving objects: 100% (3957/3957), 2.32 MiB | 612.00 KiB/s, done.
Resolving deltas: 100% (2830/2830), done.
Checking connectivity... done.
$ cp -r mpack/src/mpack ../lib/private_lib/
$ cp mpack/src/mpack-config.h.sample ../lib/private_lib/mpack/
$ cd ..
```

It should look like this:

```
$ tree             
.
├── lib
│   ├── private_lib
│   │   ├── microcoap
│   │   │   ├── coap.c
│   │   │   └── coap.h
│   │   └── mpack
│   │       ├── mpack-common.c
│   │       ├── mpack-common.h
│   │       ├── mpack-config.h.sample
│   │       ├── mpack-expect.c
│   │       ├── mpack-expect.h
│   │       ├── mpack-node.c
│   │       ├── mpack-node.h
│   │       ├── mpack-platform.c
│   │       ├── mpack-platform.h
│   │       ├── mpack-reader.c
│   │       ├── mpack-reader.h
│   │       ├── mpack-writer.c
│   │       ├── mpack-writer.h
│   │       └── mpack.h
│   └── readme.txt
├── platformio.ini
├── src
│   ├── config.cpp
│   ├── config.h
│   ├── main.cpp
│   ├── state.cpp
│   ├── state.h
│   ├── tutorial3_app.cpp
│   ├── tutorial3_app.cpp.template
│   ├── tutorial3_app.h
│   ├── tutorial3_coapwrapper_ep.cpp
│   ├── tutorial3_coapwrapper_impl.cpp
│   ├── udp_microcoap_wrapper.cpp
│   └── udp_microcoap_wrapper.h
└── vendor
     (....)
```

These files do compile, but we still need our main loop and setup code in an Arduino sketch. Create
a `src/main.cpp` with a basic skeleton. Note that this is for an ESP8266, code will vary for other boards and platforms.

```c
#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <WiFiUdp.h>

#include "microcoap/coap.h"
#include "udp_microcoap_wrapper.h"
#include "config.h"
#include "state.h"

WiFiUDP Udp;

const char* ssid     = "<<YOUR WIFI SID GOES HERE>>";
const char* password = "<<SAME WITH YOUR WIFI PWD>>";

void setup_wifi() {
    Serial.println();
    Serial.println();
    Serial.print("Connecting to ");
    Serial.println(ssid);

    WiFi.mode(WIFI_STA);
    WiFi.begin(ssid, password);

    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
    }

    Serial.println("");
    Serial.println("WiFi connected");
    Serial.println("IP address: ");
    Serial.println(WiFi.localIP());
}

void setup() {
    Serial.begin(9600);

    setup_wifi();

    Udp.begin(5683);

    coap_setup();
    udp_microcoap_wrapper_init(&Udp);

    pinMode(LED_BUILTIN, OUTPUT);
    pinMode(A0, INPUT);
    pinMode(D3, INPUT);
    pinMode(D5, OUTPUT);
    digitalWrite(D5, LOW);
}

extern void app_loop();

void loop() {
    // get coap processing object
    udp_microcoap_wrapper *c = udp_microcoap_wrapper_get();

    // let it handle a message (if one is available via WifiUdp)
    c->ops->handle_udp_coap_message(c);

    app_loop();
}
```

Slyft created a file `tutorial3_app.cpp.template` for us. Rename this and
remove the `.template` suffix:

```bash
$ mv src/tutorial3_app.cpp.template src/tutorial3_app.cpp
```

This file contains our application handlers, in short:

```c
//
// CoAP Application level handlers
//

// -- Application level handler for GET to /state
bool TUT3__tutorial3__get___state(
    struct TUT3__tutorial3__get___state_req *req,
    struct TUT3__tutorial3__get___state_resp *resp);


// -- Application level handler for GET to /config
bool TUT3__tutorial3__get___config(
    struct TUT3__tutorial3__get___config_req *req,
    struct TUT3__tutorial3__get___config_resp *resp);


// -- Application level handler for POST to /config
bool TUT3__tutorial3__post___config(
    struct TUT3__tutorial3__post___config_req *req,
    struct TUT3__tutorial3__post___config_resp *resp);
```


We need to fill those with our own device logic such as setting variables,
querying the analog sensor and letting the LED go off or on. For this we're going
to define an `app_loop()` function.

Place the following contents in `src/tutorial3_app.cpp`:

```
#include "Arduino.h"

#include "microcoap/coap.h"
#include "tutorial3_app.h"

// include data structure headers
#include "config.h"
#include "state.h"


int __upper_bound = 300;
int __lower_bound = 700;
int __led1_state = LOW;
int __led2_state = LOW;

void update_led() {
  digitalWrite(D5, __led1_state);
}

bool get_switch_state() {
  return digitalRead(D3);
}

void app_loop() {
  int v = analogRead(A0);
  Serial.println(v);
  if ( v <= __lower_bound || v >= __upper_bound)  {
    __led1_state = HIGH;
  } else {
    __led1_state = LOW;
  }
  update_led();
  delay(100);
}

//
// CoAP Application level handlers
//

// -- Application level handler for GET to /state
bool TUT3__tutorial3__get___state(
    struct TUT3__tutorial3__get___state_req *req,
    struct TUT3__tutorial3__get___state_resp *resp) {

  	Serial.println("TUT3__tutorial3__get___state");
    strcpy(resp->data.state_205.led1, (__led1_state?"ON":"OFF"));
    resp->data.state_205.switch_on = get_switch_state();
    resp->response_code = COAP_RSPCODE_CONTENT;

    return true;
}


// -- Application level handler for GET to /config
bool TUT3__tutorial3__get___config(
    struct TUT3__tutorial3__get___config_req *req,
    struct TUT3__tutorial3__get___config_resp *resp) {

  	Serial.println("TUT3__tutorial3__get___config");

    resp->data.config_205.lower_bound = __lower_bound;
    resp->data.config_205.upper_bound = __upper_bound;
    resp->response_code = COAP_RSPCODE_CONTENT;

    return true;
}


// -- Application level handler for POST to /config
bool TUT3__tutorial3__post___config(
    struct TUT3__tutorial3__post___config_req *req,
    struct TUT3__tutorial3__post___config_resp *resp) {

    Serial.println("TUT3__tutorial3__post___config");

    __lower_bound = req->data.config.lower_bound;
    __upper_bound = req->data.config.upper_bound;
    resp->response_code = COAP_RSPCODE_CHANGED;

    Serial.print( "Changed lower_bound="); Serial.println( __lower_bound);
    Serial.print( "Changed upper_bound="); Serial.println( __upper_bound);

    return true;
}
```

The above code includes some debug whenever a CoAP endpoint is called.
Try to compile and flash it to the device:

```bash
$ pio run -t upload && pio device monitor
```

If this worked out you should see the serial monitor where the device dumps
debug while connecting to your WiFi etc.

## Testing your API

You may choose to build to custom client using i.e. python or ruby, comparable
to the second tutorial. For demo purposes, we use a simple command line client
here, `coapthon` and `msgpack-tools`.

* https://github.com/Tanganelli/CoAPthon
* https://github.com/ludocode/msgpack-tools

Query the current status:

```
$ coapclient.py -o GET -p coap://172.20.10.12/state  
(...)
Source: ('172.20.10.12', 5683)
Destination: None
Type: ACK
MID: 42663
Code: CONTENT
Token: None
Content-Type: 42
Payload:
��switch_on¤led1�ON
```

Update the configuration requires a file workaround, because coapthon client is
not able to read from STDIN.

```bash
$ echo '{ "lower_bound": 300, "upper_bound": 500}' | json2msgpack >cfg.bin
$ coapclient.py -o POST -p coap://172.20.10.12/config -f cfg.bin       
Source: ('172.20.10.12', 5683)
Destination: None
Type: ACK
MID: 59071
Code: CHANGED
Token:
Content-Type: 65535
Payload:
None
```

On the device monitor:

```
TUT3__tutorial3__post___config
Changed lower_bound=300
Changed upper_bound=500
```
