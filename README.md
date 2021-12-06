# MQTT client with XENSIV&trade; sensors: BGT60TR13C 60-GHz radar

## Overview

This code example demonstrates implementing an MQTT client using the [MQTT client library](https://github.com/Infineon/mqtt) for XENSIV&trade; sensors with Infineon connectivity devices. The library uses the following:

 - AWS IoT device SDK MQTT client library that includes an MQTT 3.1.1 client

 - RadarSensing library configured for presence detection application

In this example, the MQTT client RTOS task establishes a connection with the configured MQTT broker, and creates three tasks - publisher, subscriber, and radar. The publisher task publishes messages on the `MQTT_PUB_TOPIC` topic when radar events are detected. The subscriber task subscribes to `MQTT_SUB_TOPIC`. The radar task initializes the RadarSensing context object for detecting presence, registers a callback to handle presence detection events, and continuously processes the data acquired from the radar.

From 'radar_task', there is another radar configuration task created to enable dynamically configuring the RadarSensing library. From the MQTT broker, you can send a JSON message to the MQTT client within the `MQTT_SUB_TOPIC` to perform the configuration.


[View this README on GitHub.](https://github.com/Infineon/mtb-example-sensors-radar-anycloud-mqtt-client)

[Provide feedback on this code example.](https://cypress.co1.qualtrics.com/jfe/form/SV_1NTns53sK2yiljn?Q_EED=eyJVbmlxdWUgRG9jIElkIjoiQ0UyMzMyMzciLCJTcGVjIE51bWJlciI6IjAwMi0zMzIzNyIsIkRvYyBUaXRsZSI6Ik1RVFQgY2xpZW50IHdpdGggWEVOU0lWJnRyYWRlOyBzZW5zb3JzOiBCR1Q2MFRSMTNDIDYwLUdIeiByYWRhciIsInJpZCI6Ind1eWFuZyIsIkRvYyB2ZXJzaW9uIjoiMS4wLjAiLCJEb2MgTGFuZ3VhZ2UiOiJFbmdsaXNoIiwiRG9jIERpdmlzaW9uIjoiTUNEIiwiRG9jIEJVIjoiU0JTWVMiLCJEb2MgRmFtaWx5IjoiU1VCU1lTIn0=)


## Sequence of operation

1. One of the following radar events occurs: presence, absence, or counter event. At the same time, the LED indicates other events. See *Note 2* for details.

2. The radar sensing callback function notifies the publisher task.

3. The publisher task publishes this event to the remote server on a defined topic. You can verify this data from the MQTT broker.

4. The MQTT broker sends the configuration as a JSON string to this client to configure the 'xensiv-radar-sensing' library. This allows remote configuration.

## Requirements

- [ModusToolbox&trade; software](https://www.cypress.com/products/modustoolbox-software-environment) v2.2 or later

    **Note:** This code example version requires ModusToolbox&trade; software version 2.2 or later and is not backward compatible with v2.1 or older versions.

- Board support package (BSP) minimum required version: 2.0.0
- Programming language: C
- Associated parts: All [PSoC&trade; 6 MCU](http://www.cypress.com/PSoC6) parts with SDIO, [AIROC™ CYW43012 Wi-Fi & Bluetooth® combo chip](https://www.cypress.com/documentation/datasheets/cyw43012-single-chip-ultra-low-power-ieee-80211n-compliant-ieee-80211ac), [AIROC™ CYW4343W Wi-Fi & Bluetooth® combo chip](https://www.cypress.com/documentation/datasheets/cyw4343w-single-chip-80211-bgn-macbasebandradio-bluetooth-51)


## Supported toolchains (make variable 'TOOLCHAIN')

- GNU Arm® Embedded Compiler v9.3.1 (`GCC_ARM`) - Default value of `TOOLCHAIN`


## Supported kits (make variable 'TARGET')

- PSoC&trade; 6 rapid IoT connect platform RP01 feather kit (`CYSBSYSKIT-DEV-01`)

**Note:** This example requires PSoC&trade; 6 MCU devices with at least 2-MB flash and 1-MB SRAM and therefore does not support other PSoC&trade; 6 MCU kits.


## Hardware setup

This example uses the board's default configuration. See the kit user guide to ensure that the board is configured correctly.


## Software setup

Install a terminal emulator if you don't have one. Instructions in this document use [Tera Term](https://ttssh2.osdn.jp/index.html.en).

This example requires no additional software or tools.


## Using the code example

Create the project and open it using one of the following:

<details><summary><b>In Eclipse IDE for ModusToolbox&trade; software</b></summary>

1. Click the **New application** link in the **quick panel** (or, use **File** > **New** > **ModusToolbox&trade; application**). This launches the [project creator](http://www.cypress.com/ModusToolboxProjectCreator) tool.

2. Pick a kit supported by the code example from the list shown in the **Project creator - Choose board support package (BSP)** dialog.

   When you select a supported kit, the example is reconfigured automatically to work with the kit. To work with a different supported kit later, use the [library manager](https://www.cypress.com/ModusToolboxLibraryManager) to choose the BSP for the supported kit. You can use the library manager to select or update the BSP and firmware libraries used in this application. To access the library manager, click the link from the **quick panel**.

   You can also just start the application creation process again and select a different kit.

   If you want to use the application for a kit not listed here, you may need to update the source files. If the kit does not have the required resources, the application may not work.

3. In the **Project creator - Select application** dialog, choose the example by enabling the checkbox.

4. (Optional) Change the suggested **New application name**.

5. Enter the local path in the **Application(s) root path** field to indicate where the application needs to be created.

   Applications that can share libraries can be placed in the same root path.

6. Click **Create** to complete the application creation process.

For more details, see the [Eclipse IDE for ModusToolbox&trade; software user guide](https://www.cypress.com/MTBEclipseIDEUserGuide) (locally available at *{ModusToolbox&trade; software install directory}/ide_{version}/docs/mt_ide_user_guide.pdf*).

</details>

<details><summary><b>In command-line interface (CLI)</b></summary>

ModusToolbox&trade; software provides the project creator as both a GUI tool and the command line tool, "project-creator-cli". The CLI tool can be used to create applications from a CLI terminal or from within batch files or shell scripts. This tool is available in the *{ModusToolbox&trade; software install directory}/tools_{version}/project-creator/* directory.

Use a CLI terminal to invoke the "project-creator-cli" tool. On Windows, use the command line "modus-shell" program provided in the ModusToolbox&trade; software installation instead of a standard Windows command-line application. This shell provides access to all ModusToolbox&trade; software tools. You can access it by typing `modus-shell` in the search box in the Windows menu. In Linux and macOS, you can use any terminal application.

This tool has the following arguments:

Argument | Description | Required/optional
---------|-------------|-----------
`--board-id` | Defined in the `<id>` field of the [BSP](https://github.com/Infineon?q=bsp-manifest&type=&language=&sort=) manifest | Required
`--app-id`   | Defined in the `<id>` field of the [CE](https://github.com/Infineon?q=ce-manifest&type=&language=&sort=) manifest | Required
`--target-dir`| Specify the directory in which the application is to be created if you prefer not to use the default current working directory | Optional
`--user-app-name`| Specify the name of the application if you prefer to have a name other than the example's default name | Optional

<br>

The following example will clone the "[Hello world](https://github.com/Infineon/mtb-example-psoc6-hello-world)" application with the desired name "MyHelloWorld" configured for the *CY8CKIT-062-WIFI-BT* BSP into the specified working directory, *C:/mtb_projects*:

   ```
   project-creator-cli --board-id CY8CKIT-062-WIFI-BT --app-id mtb-example-psoc6-hello-world --user-app-name MyHelloWorld --target-dir "C:/mtb_projects"
   ```

**Note:** The project-creator-cli tool uses the `git clone` and `make getlibs` commands to fetch the repository and import the required libraries. For details, see the "Project creator tools" section of the [ModusToolbox&trade; software user guide](https://www.cypress.com/ModusToolboxUserGuide) (locally available at *{ModusToolbox&trade; software install directory}/docs_{version}/mtb_user_guide.pdf*).

</details>

<details><summary><b>In third-party IDEs</b></summary>

Use one of the following options:

- **Use the standalone [project creator](https://www.cypress.com/ModusToolboxProjectCreator) tool:**

   1. Launch the project creator from the Windows Start menu or from *{ModusToolbox&trade; software install directory}/tools_{version}/project-creator/project-creator.exe*.

   2. In the initial **Choose board support package** screen, select the BSP, and click **Next**.

   3. In the **Select application** screen, select the appropriate IDE from the **Target IDE** drop-down menu.

   4. Click **Create** and follow the instructions printed in the bottom pane to import or open the exported project in the respective IDE.

<br>

- **Use command-line interface (CLI):**

   1. Follow the instructions from the **In command-line interface (CLI)** section to create the application, and then import the libraries using the `make getlibs` command.

   2. Export the application to a supported IDE using the `make <ide>` command.

   3. Follow the instructions displayed in the terminal to create or import the application as an IDE project.

For a list of supported IDEs and more details, see the "Exporting to IDEs" section of the [ModusToolbox&trade; software user guide](https://www.cypress.com/ModusToolboxUserGuide) (locally available at *{ModusToolbox&trade; software install directory}/docs_{version}/mtb_user_guide.pdf*).

</details>

**Note:** There are two working modes for the RadarSensing library: **presence sensing** and **entrance counter**. You can switch the working mode at compile time by `define` or `undef` `RADAR_ENTRANCE_COUNTER_MODE` inside *radar_task.h*. By default, it works in the **presence sensing** mode.


## Operation

1. Connect the board to your PC using the provided USB cable through the KitProg3 USB connector.

2. Modify the user configuration files in the *configs* directory as follows:

      1. **Wi-Fi configuration:** Set the Wi-Fi credentials in *configs/wifi_config.h*: Modify the macros `WIFI_SSID`, `WIFI_PASSWORD`, and `WIFI_SECURITY` to match with that of the Wi-Fi network that you want to connect.

      2. **MQTT configuration:** Set up the MQTT client and configure the credentials in *configs/mqtt_client_config.h* as follows:

         - **For AWS IoT MQTT:**

             1. Set up the MQTT device (also known as a *Thing*) in the AWS IoT core as described in the [Getting started with AWS IoT tutorial](https://docs.aws.amazon.com/iot/latest/developerguide/iot-gs.html).

                **Note:** While setting up your device, ensure that the policy associated with this device permits all MQTT operations (*iot:Connect*, *iot:Publish*, *iot:Receive*, and *iot:Subscribe*) for the resource used by this device. For testing purposes, it is recommended to have the following policy document which allows all *MQTT Policy Actions* on all *Amazon Resource Names (ARNs)*.
                ```
                {
                    "Version": "2012-10-17",
                    "Statement": [
                        {
                            "Effect": "Allow",
                            "Action": "iot:*",
                            "Resource": "*"
                        }
                    ]
                }
                ```

             2. Set `MQTT_BROKER_ADDRESS` to your custom endpoint on the **Settings** page of the AWS IoT console. This has the format `ABCDEFG1234567.iot.<region>.amazonaws.com`.

             3. Download the following certificates and keys that are created and activated in the previous step:
                   - A certificate for the AWS IoT Thing – *xxxxxxxxxx.certificate.pem.crt*
                   - A public key – *xxxxxxxxxx.public.pem.key*
                   - A private key – *xxxxxxxxxx.private.pem.key*
                   - Root CA "RSA 2048 bit key: Amazon Root CA 1" for AWS IoT from [CA certificates for server authentication](https://docs.aws.amazon.com/iot/latest/developerguide/server-authentication.html#server-authentication-certs) – *xxxxxxxxxx.AmazonRootCA1.pem*

             4. Using these certificates and keys, enter the following parameters in *mqtt_client_config.h* in Privacy-Enhanced Mail (PEM) format:
                   - `CLIENT_CERTIFICATE` – *xxxxxxxxxx.certificate.pem.crt*
                   - `CLIENT_PRIVATE_KEY` – *xxxxxxxxxx.private.pem.key*
                   - `ROOT_CA_CERTIFICATE` – *xxxxxxxxxx.AmazonRootCA1.pem*

                You can either convert the values to strings manually following the format shown in *mqtt_client_config.h* or you can use the HTML utility available [here](https://github.com/Infineon/amazon-freertos/blob/master/tools/certificate_configuration/PEMfileToCString.html) to convert the certificates and keys from PEM format to C string format. You need to clone the repository from GitHub to use the utility.

         - **For public Mosquitto broker:**

           **Note:** The public test MQTT broker at [test.mosquitto.org](https://test.mosquitto.org/) uses the SHA-1 hashing algorithm for certificate signing. As cautioned by Mbed TLS, SHA-1 is considered a weak message digest. The use of SHA-1 for certificate signing constitutes a security risk. It is recommended to avoid dependencies on it, and consider stronger message digests instead.

           Note that this code example enables SHA-1 support in Mbed TLS and MQTT client libraries in order to support secure TLS-based connections to the public test MQTT broker as described [here](https://github.com/Infineon/mqtt/tree/latest-v2.X#quick-start).

             1. Set `MQTT_BROKER_ADDRESS` as `"test.mosquitto.org"`.

             2. Set `AWS_IOT_MQTT_MODE` to `0` because you are connecting to an MQTT broker other than AWS IoT.

             3. For a *secure connection* that requires client authentication using TLS (port 8884), configure as follows:

                   - Set `MQTT_PORT` as `8884`.

                   - Set `MQTT_SECURE_CONNECTION` as `1`.

                   - Generate the client certificate and the private key according to the instructions from [test.mosquitto.org](https://test.mosquitto.org/). The root CA certificate is available [here](https://test.mosquitto.org/ssl/mosquitto.org.crt).

             4. Using the client certificate, private key, and root CA certificate, configure the `CLIENT_CERTIFICATE`, `CLIENT_PRIVATE_KEY`, and `ROOT_CA_CERTIFICATE` macros respectively.

                You can either convert the PEM format values to strings manually following the format shown in *mqtt_client_config.h* or you can use the HTML utility available [here](https://github.com/Infineon/amazon-freertos/blob/master/tools/certificate_configuration/PEMfileToCString.html) to convert the certificates and keys from PEM format to C string format. You need to clone the repository from GitHub to use the utility.

             5. In the application *Makefile*, set the variable `ENABLE_SECURE_MOSQUITTO_BROKER_SUPPORT` as `1` to enable SHA-1 support.

                For *non-TLS connections* (port 1883), configure the macros as follows:

                   - Set `MQTT_PORT` as `1883`.

                   - Set `MQTT_SECURE_CONNECTION` as `0`.

         - **Other MQTT client configuration macros:**

             1. `MQTT_PUB_TOPIC` – Set the MQTT topic to the publisher in this example.

             2. `MQTT_SUB_TOPIC` – Set the MQTT topic to the subscriber in this example.

             3. `ENABLE_LWT_MESSAGE` – Set to `1` to use MQTT's 'last will and testament' (LWT) feature. This is an MQTT message that will be published by the MQTT broker if the MQTT connection is unexpectedly closed.

             4. `GENERATE_UNIQUE_CLIENT_ID` – Every active MQTT connection must have a unique client identifier. If this macro is set to `1`, the device will generate a unique client identifier by appending a timestamp to the string specified by the `MQTT_CLIENT_IDENTIFIER` macro. This feature is useful if you are using the same code on multiple kits simultaneously.

         Although this code example provides instructions only for AWS IoT and the public test Mosquitto broker, the MQTT client implemented in this example is generic. It is expected to work with other MQTT brokers (see the [list of publicly-accessible MQTT brokers](https://github.com/mqtt/mqtt.github.io/wiki/public_brokers)) with appropriate configurations.

      3. Other configuration files: You can optionally modify the configuration macros in the following files according to your application:

         - *configs/iot_config.h* used by the [MQTT library](https://github.com/Infineon/mqtt)

         - *configs/FreeRTOSConfig.h* used by the [FreeRTOS library](https://github.com/Infineon/freertos)

3. Open a terminal program and select the KitProg3 COM port. Set the serial port parameters to 8N1 and 115200 baud.

4. Program the board using one of the following:

   <details><summary><b>Using Eclipse IDE for ModusToolbox&trade; software</b></summary>

      1. Select the application project in the project explorer.

      2. In the **quick panel**, scroll down, and click **\<Application name> Program (KitProg3_MiniProg4)**.
   </details>

   <details><summary><b>Using CLI</b></summary>

     From the terminal, execute the `make program` command to build and program the application using the default toolchain to the default target. The default toolchain and target are specified in the application's Makefile but you can override those values manually:
      ```
      make program TARGET=<BSP> TOOLCHAIN=<toolchain>
      ```

      Example:
      ```
      make program TARGET=CYSBSYSKIT-DEV-01 TOOLCHAIN=GCC_ARM
      ```
   </details>

   After programming, the application starts automatically. Observe the messages on the UART terminal, and wait for the device to make all the required connections.

   **Figure 1. UART terminal showing the application initialization status**

   ![](images/app_init.png)

5. Once the initialization is complete, confirm that subscription to the topic is successful.

6. If the radar wing board is connected and it detects a sensor event, the following messages are displayed in the terminal:

   **Figure 2. Radar event detected**

   ![](images/module_output.png)

7. Do the following if a radar event is detected:

   1. Go to AWS IoT-core on the bar on the left.

   2. Open the **MQTT test client** from the **Test** menu to subscribe to the `MQTT_PUB_TOPIC` topic, which is **radar_status**.

      You should see the following:

      **Figure 3. Radar event detected message on the MQTT broker**

      ![](images/module_output_from_aws.png)

      When you configure the device from the MQTT broker (see the first half of Figure 4), the configuration JSON message will be sent to the device within `MQTT_SUB_TOPIC`. The message will be printed out in the terminal. In addition, the device will check and apply the configuration, and give feedback to the MQTT broker (see the second half of Figure 4).

      **Figure 4. Receive configuration message on the MQTT broker**

      ![](images/module_config_from_aws.png)

      **Note:** The warning message appears only because the received message is not in JSON format.


      **Figure 5. Receive configuration message**

      ![](images/module_config.png)


      **Table 2. Configuration JSON objects**

      (See XENSIV&trade; RadarSensing library documentation)

      | Key  |  Default value     | Valid values |
      | :------- | :------------    | :--------------------|
      | **Presence sensing** |
      | `radar_presence_range_max` | "2.0" | 0.66 - 10.2 m|
      | `radar_presence_sensitivity` | "medium" | "low", "medium", "high" |
      | **Entrance counter** |
      | `radar_counter_installation` | "side" | "ceiling", "side" |
      | `radar_counter_orientation` | "portrait" | "portrait", "landscape" |
      | `radar_counter_ceiling_height` | "2.5" | 0.0 - 3.0 m |
      | `radar_counter_entrance_width` | "1.0" | 0.0 - 3.0 m |
      | `radar_counter_sensitivity` | "0.5" | 0.0 - 1.0 |
      | `radar_counter_traffic_light_zone` | "1.0" | 0.0 - 1.0 m |
      | `radar_counter_reverse` | "false" | "true", "false" |
      | `radar_counter_min_person_height` | "1.0" | 0.0 - 2.0 m |
      | `radar_counter_in_number` | "0" | any non-negative integer (32-bit)
      | `radar_counter_out_number` | "0" | any non-negative integer (32-bit)

   <br>

8. Confirm that the following messages are printed when no wing boards are connected:

   **Figure 6. No wing board connected**

   ![](images/module_missing.png)

This example can be programmed on multiple kits (*Only when `GENERATE_UNIQUE_CLIENT_ID` is set to `1`*).

Alternatively, the publish and subscribe functionalities of the MQTT client can be individually verified if the MQTT broker supports a Test MQTT client like the AWS IoT.

Do one of the following:

- *To verify the subscribe functionality*: Using the Test MQTT client, publish messages on the topic specified by the `MQTT_SUB_TOPIC` macro in *mqtt_client_config.h*.

- *To verify the publish functionality*: From the Test MQTT client, subscribe to the MQTT topic specified by the `MQTT_PUB_TOPIC` macro and confirm that the messages published by the kit are displayed on the Test MQTT client's console.


### Sensor Information and LEDs
1. The radar task is suspended if the radar wing board is not connected to the feather kit. The sensor initialization process is indicated by blinking the red LED (CYBSP_USER_LED) on CYSBSYSKIT-DEV-01. The red LED (CYBSP_USER_LED) on CYSBSYSKIT-DEV-01 remains turned on when system is operational (ready state).

2. The LED indicates different events with different patterns as follows:

    **Table 1. Events and LED glow patterns**

   | LED pattern  |  Event type  |  Comment  |
   | ----------- | ----------- | -----    |
   | **Presence sensing** |
   | LED glows in red color | `MTB_RADAR_SENSING_EVENT_PRESENCE_IN` | Presence event detected. Entering field of view
   | LED stable in green color | `MTB_RADAR_SENSING_EVENT_PRESENCE_OUT` | Presence event detected. Leaving filed of view
   | **Entrance counter** |
   | LED glows in red color | `MTB_RADAR_SENSING_EVENT_COUNTER_OCCUPIED` | Counter event detected. Entering field of view
   | LED glows in green color | `MTB_RADAR_SENSING_EVENT_COUNTER_FREE` |Counter event detected. Leaving field of view
   | LED blinking in red/green color | `MTB_RADAR_SENSING_EVENT_COUNTER_IN` or `MTB_RADAR_SENSING_EVENT_COUNTER_OUT` | Depends on the installation position to determine which scenario is **IN**, which is **OUT** |

## Debugging

You can debug the example to step through the code. In the IDE, use the **\<Application name> Debug (KitProg3_MiniProg4)** configuration in the **quick panel**. For more details, see the "Program and debug" section in the [Eclipse IDE for ModusToolbox&trade; software user guide](https://www.cypress.com/MTBEclipseIDEUserGuide).

**Note:** **(Only while debugging)** On the CM4 CPU, some code in `main()` may execute before the debugger halts at the beginning of `main()`. This means that some code executes twice - once before the debugger stops execution, and again after the debugger resets the program counter to the beginning of `main()`. See [KBA231071](https://community.cypress.com/docs/DOC-21143) to learn about this and for the workaround.


## Design and implementation

This example implements three RTOS tasks: MQTT client, publisher, and subscriber. The main function initializes the BSP and the retarget-io library, and creates the MQTT client task.

The MQTT client task initializes the Wi-Fi connection manager (WCM) and connects to a Wi-Fi access point (AP) using the Wi-Fi network credentials that are configured in *wifi_config.h*. Upon a successful Wi-Fi connection, the task initializes the MQTT library and establishes a connection with the MQTT broker/server.

The MQTT connection is configured to be secure by default; the secure connection requires a client certificate, a private key, and the root CA certificate of the MQTT broker that are configured in *mqtt_client_config.h*.

After a successful MQTT connection, the subscriber and publisher tasks are created. The MQTT client task then waits for messages from the other two tasks and callbacks, and handles the cleanup operations of various libraries if the messages indicate failure.

The subscriber task subscribes to messages on the topic specified by the `MQTT_SUB_TOPIC` macro that can be configured in *mqtt_client_config.h*. When the subscribe operation fails, a message is sent to the MQTT client task over a message queue. When the subscriber task receives a message from the broker, it prints the information.

The radar sensing callback function notifies the publisher task upon a radar event. The publisher task then publishes messages (*PRESENCE IN*/*PRESENCE OUT*) on the topic specified by the `MQTT_PUB_TOPIC` macro. When the publish operation fails, a message is sent over a queue to the MQTT client task.

When a failure occurs, the MQTT client task handles the cleanup operations of various libraries, thereby terminating any existing MQTT and Wi-Fi connections and deleting the MQTT, publisher, and subscriber tasks.


### Resources and settings

**Table 3. Application source files**

|**File name**            |**Comments**         |
| ------------------------|-------------------- |
| *main.c* |Contains the application entry point. It initializes the UART for debugging and then initializes the controller tasks|
| *mqtt_client_config.c* |Global variables for MQTT connection|
| *mqtt_task.c* |Contains the task function to do the following: <br> 1. Establish an MQTT connection <br> 2. Start the publisher and subscriber tasks <br> 3. Start the radar task|
| *publisher_task.c* |Contains the task function to publish message to the MQTT broker|
| *subscriber_task.c* |Contains the task function to subscribe message from the MQTT broker|
| *radar_task.c* |Contains the task function for the presence and entrance counter application (select at compile time), as well as the callback function|
| *radar_config_task.c* |Contains the task function to configure the xensiv-radar-sensing library |
| *radar_led_task.c* |Contains the task function that handles the LEDs |

<br>

## Related resources


Resources  | Links
-----------|----------------------------------
Application notes  | [AN228571](https://www.cypress.com/AN228571) – Getting started with PSoC&trade; 6 MCU on ModusToolbox&trade; software <br>  [AN215656](https://www.cypress.com/AN215656) – PSoC&trade; 6 MCU: Dual-CPU system design
Code examples  | [Using ModusToolbox&trade; software](https://github.com/Infineon/Code-Examples-for-ModusToolbox-Software) on GitHub
Device documentation | [PSoC&trade; 6 MCU datasheets](https://www.cypress.com/search/all?f[0]=meta_type%3Atechnical_documents&f[1]=resource_meta_type%3A575&f[2]=field_related_products%3A114026) <br> [PSoC&trade; 6 technical reference manuals](https://www.cypress.com/search/all/PSoC%206%20Technical%20Reference%20Manual?f[0]=meta_type%3Atechnical_documents&f[1]=resource_meta_type%3A583)
Development kits | [CYSBSYSKIT-DEV-01](https://github.com/Infineon/TARGET_CYSBSYSKIT-DEV-01) Rapid IoT connect developer kit
Libraries on GitHub  | [xensiv-radar-sensing](https://github.com/Infineon/xensiv-radar-sensing) – RadarSensing library to detect presence and count people using XENSIV™ BGT60TR13C <br> [mtb-pdl-cat1](https://github.com/infineon/mtb-pdl-cat1) – PSoC&trade; 6 peripheral driver library (PDL)  <br> [mtb-hal-cat1](https://github.com/infineon/mtb-hal-cat1) – Hardware abstraction layer (HAL) library <br> [retarget-io](https://github.com/infineon/retarget-io) – Utility library to retarget STDIO messages to a UART port
Middleware on GitHub  | [mqtt](https://github.com/Infineon/mqtt) – MQTT client library and docs <br>  [wifi-connection-manager](https://github.com/Infineon/wifi-connection-manager) – Wi-Fi connection manager (WCM) library and docs <br> [wifi-mw-core](https://github.com/Infineon/wifi-mw-core) – Connectivity utilities and docs<br> [psoc6-middleware](https://github.com/Infineon/modustoolbox-software#psoc-6-middleware-libraries) – Links to all PSoC&trade; 6 MCU middleware
Tools  | [Eclipse IDE for ModusToolbox&trade; software](https://www.cypress.com/modustoolbox) – ModusToolbox&trade; software is a collection of easy-to-use software and tools enabling rapid development with Infineon MCUs, covering applications from embedded sense and control to wireless and cloud-connected systems using AIROC&trade; Wi-Fi and Bluetooth® connectivity devices.


## Other resources

Cypress provides a wealth of data at www.cypress.com to help you select the right device, and quickly and effectively integrate it into your design.

For PSoC&trade; 6 MCU devices, see [How to design with PSoC&trade; 6 MCU - KBA223067](https://community.cypress.com/docs/DOC-14644) in the Cypress community.


## Document history

Document title: *CE233237* – *MQTT client with XENSIV&trade; sensors: BGT60TR13C 60-GHz radar*

| Version | Description of change |
| ------- | --------------------- |
| 0.5.0   | New code example      |
| 1.0.0   | Update to: <br>1. Support xensiv-radar-sensing v1.X library. <br>2. Reduce drive strength to improve EMI.      |

------

All other trademarks or registered trademarks referenced herein are the property of their respective owners.


-------------------------------------------------------------------------------

© Cypress Semiconductor Corporation, 2020-2021. This document is the property of Cypress Semiconductor Corporation, an Infineon Technologies company, and its affiliates ("Cypress").  This document, including any software or firmware included or referenced in this document ("Software"), is owned by Cypress under the intellectual property laws and treaties of the United States and other countries worldwide.  Cypress reserves all rights under such laws and treaties and does not, except as specifically stated in this paragraph, grant any license under its patents, copyrights, trademarks, or other intellectual property rights.  If the Software is not accompanied by a license agreement and you do not otherwise have a written agreement with Cypress governing the use of the Software, then Cypress hereby grants you a personal, non-exclusive, nontransferable license (without the right to sublicense) (1) under its copyright rights in the Software (a) for Software provided in source code form, to modify and reproduce the Software solely for use with Cypress hardware products, only internally within your organization, and (b) to distribute the Software in binary code form externally to end users (either directly or indirectly through resellers and distributors), solely for use on Cypress hardware product units, and (2) under those claims of Cypress’s patents that are infringed by the Software (as provided by Cypress, unmodified) to make, use, distribute, and import the Software solely for use with Cypress hardware products.  Any other use, reproduction, modification, translation, or compilation of the Software is prohibited.
<br>
TO THE EXTENT PERMITTED BY APPLICABLE LAW, CYPRESS MAKES NO WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, WITH REGARD TO THIS DOCUMENT OR ANY SOFTWARE OR ACCOMPANYING HARDWARE, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.  No computing device can be absolutely secure.  Therefore, despite security measures implemented in Cypress hardware or software products, Cypress shall have no liability arising out of any security breach, such as unauthorized access to or use of a Cypress product. CYPRESS DOES NOT REPRESENT, WARRANT, OR GUARANTEE THAT CYPRESS PRODUCTS, OR SYSTEMS CREATED USING CYPRESS PRODUCTS, WILL BE FREE FROM CORRUPTION, ATTACK, VIRUSES, INTERFERENCE, HACKING, DATA LOSS OR THEFT, OR OTHER SECURITY INTRUSION (collectively, "Security Breach").  Cypress disclaims any liability relating to any Security Breach, and you shall and hereby do release Cypress from any claim, damage, or other liability arising from any Security Breach.  In addition, the products described in these materials may contain design defects or errors known as errata which may cause the product to deviate from published specifications. To the extent permitted by applicable law, Cypress reserves the right to make changes to this document without further notice. Cypress does not assume any liability arising out of the application or use of any product or circuit described in this document. Any information provided in this document, including any sample design information or programming code, is provided only for reference purposes.  It is the responsibility of the user of this document to properly design, program, and test the functionality and safety of any application made of this information and any resulting product.  "High-Risk Device" means any device or system whose failure could cause personal injury, death, or property damage.  Examples of High-Risk Devices are weapons, nuclear installations, surgical implants, and other medical devices.  "Critical Component" means any component of a High-Risk Device whose failure to perform can be reasonably expected to cause, directly or indirectly, the failure of the High-Risk Device, or to affect its safety or effectiveness.  Cypress is not liable, in whole or in part, and you shall and hereby do release Cypress from any claim, damage, or other liability arising from any use of a Cypress product as a Critical Component in a High-Risk Device. You shall indemnify and hold Cypress, including its affiliates, and its directors, officers, employees, agents, distributors, and assigns harmless from and against all claims, costs, damages, and expenses, arising out of any claim, including claims for product liability, personal injury or death, or property damage arising from any use of a Cypress product as a Critical Component in a High-Risk Device. Cypress products are not intended or authorized for use as a Critical Component in any High-Risk Device except to the limited extent that (i) Cypress’s published data sheet for the product explicitly states Cypress has qualified the product for use in a specific High-Risk Device, or (ii) Cypress has given you advance written authorization to use the product as a Critical Component in the specific High-Risk Device and you have signed a separate indemnification agreement.
<br>
Cypress, the Cypress logo, and combinations thereof, WICED, ModusToolbox, PSoC, CapSense, EZ-USB, F-RAM, and Traveo are trademarks or registered trademarks of Cypress or a subsidiary of Cypress in the United States or in other countries. For a more complete list of Cypress trademarks, visit cypress.com. Other names and brands may be claimed as property of their respective owners.
