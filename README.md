# Introduction

This code example demonstrates using the CSD current digital-to-analog converter (IDAC) as a current source and a current sink. CSD IDAC block supports two channels - A and B.

1. IDAC as current source: Channel A is configured as a current source. The current increases when a switch is pressed. Once the output reaches its maximum value, it resets to zero and starts to increase the value again. If the switch is not pressed, it holds the last value.

2. IDAC as current sink: Channel B is configured for sinking current and used for driving an LED. Firmware controls the sinking current to toggle the LED every second.

Additional PSoC 6 MCU-related code examples are available in other repos. See all examples at [Code Examples for Modus Toolbox](https://github.com/cypresssemiconductorco/Code-Examples-for-ModusToolbox-Software).

# Supported Kits

- [CY8CPROTO-062-4343W PSoC 6 Wi-Fi BT Prototyping kit](http://www.cypress.com/documentation/development-kitsboards/cy8cproto-063-4343w) (CY8CPROTO_062_4343W)
- [CY8CKIT-062-WiFi-BT PSoC 6 WiFi-BT Pioneer Kit](http://www.cypress.com/documentation/development-kitsboards/psoc-6-wifi-bt-pioneer-kit) (CY8CKIT_062_WIFI_BT)
- [CY8CKIT-062-BLE PSoC 6 BLE Pioneer Kit](http://www.cypress.com/documentation/development-kitsboards/psoc-6-ble-pioneer-kit) (CY8CKIT_062_BLE)

# Instructions to run the CapSense code example

1. Import the code example.
   
```
    mbed import https://github.com/cypresssemiconductorco/mbed-os-example-csdidac
```

2. Change the working directory to the example folder.
   
   ```
    cd mbed-os-example-csdidac
   ```
   
3. Plug in the CY8CPROTO_062_4343W kit and ensure that the kit is in DAPLink mode to allow programming from Mbed CLI. See the *Switch Kit to DAPLink Mode* section in [ModusToolbox IDE User Guide](https://www.cypress.com/ModusToolboxUserGuide). 

4. Compile the example and program.

    ```
    mbed compile --target CY8CPROTO_062_4343W --toolchain GCC_ARM --flash --sterm
    ```

    For other targets:

    ```
    mbed compile -m CY8CKIT_062_WIFI_BT -t GCC_ARM -f --sterm
    mbed compile -m CY8CKIT_062_BLE -t GCC_ARM -f --sterm
    ```

    **Note:** The *--sterm* option opens the serial terminal with 9600-8N1 setting on the command prompt itself after programming completes. Do not use this option if you wish to connect using another serial terminal application.

5. The following message is displayed on the serial terminal. The LED blinks every second when the application starts running.

    ```
    CSDIDAC Code Example
    ```

6. Place a resistor across Pin 10[0] and measure the voltage across the resistor to find out the current value. 

7. Press user button (SW2) to increase the source current. When the user button is pressed, the terminal application should also show the current value the IDAC is configured for.
	
	```
	CSDIDAC Code Example

	CSDIDAC is configured for sourcing current: 0 nA
	CSDIDAC is configured for sourcing current: 10000 nA
	CSDIDAC is configured for sourcing current: 20000 nA
	CSDIDAC is configured for sourcing current: 30000 nA
	```

# How to modify the CSDIDAC pins for this project

Do the following to modify the pins: 

1.  Update the variables `csdidac_a_pin` and `csdidac_b_pin`. For example, to connect P10[0] to channel A and P10[1] to channel B, update the variables as follows:

```c++
static const cy_stc_csdidac_pin_t csdidac_a_pin = 
{
	.ioPcPtr = GPIO_PRT10,
	.pin = 0u,
};

static const cy_stc_csdidac_pin_t csdidac_b_pin = 
{
	.ioPcPtr = GPIO_PRT10,
	.pin = 1u,
};
```

2. Update the function `config_routing` with new AMUX bus connection.

* For CY8CKIT-062-BLE PSoC 6 BLE Pioneer Kit: 

   | Port                                       | AMUX configuration                                           |
   | ------------------------------------------ | ------------------------------------------------------------ |
   | Port 0<br>Port 11<br>Port 12<br>Port 13    | `HSIOM->AMUX_SPLIT_CTL[5] = HSIOM_AMUX_SPLIT_CTL_SWITCH_AA_SL_Msk \| HSIOM_AMUX_SPLIT_CTL_SWITCH_AA_SR_Msk \| HSIOM_AMUX_SPLIT_CTL_SWITCH_BB_SL_Msk \| HSIOM_AMUX_SPLIT_CTL_SWITCH_BB_SR_Msk;` <br>`HSIOM->AMUX_SPLIT_CTL[6] = HSIOM_AMUX_SPLIT_CTL_SWITCH_AA_SL_Msk \| HSIOM_AMUX_SPLIT_CTL_SWITCH_AA_SR_Msk \| HSIOM_AMUX_SPLIT_CTL_SWITCH_BB_SL_Msk \| HSIOM_AMUX_SPLIT_CTL_SWITCH_BB_SR_Msk;` |
   | Port 1                                     | `HSIOM->AMUX_SPLIT_CTL[2] = HSIOM_AMUX_SPLIT_CTL_SWITCH_AA_SL_Msk \| HSIOM_AMUX_SPLIT_CTL_SWITCH_AA_SR_Msk \| HSIOM_AMUX_SPLIT_CTL_SWITCH_BB_SL_Msk \| HSIOM_AMUX_SPLIT_CTL_SWITCH_BB_SR_Msk;` <br>`HSIOM->AMUX_SPLIT_CTL[4] = HSIOM_AMUX_SPLIT_CTL_SWITCH_AA_SL_Msk \| HSIOM_AMUX_SPLIT_CTL_SWITCH_AA_SR_Msk \| HSIOM_AMUX_SPLIT_CTL_SWITCH_BB_SL_Msk \| HSIOM_AMUX_SPLIT_CTL_SWITCH_BB_SR_Msk;` |
   | Port 5<br />Port 6<br />Port 7<br />Port 8 | `HSIOM->AMUX_SPLIT_CTL[4] = HSIOM_AMUX_SPLIT_CTL_SWITCH_AA_SL_Msk \| HSIOM_AMUX_SPLIT_CTL_SWITCH_AA_SR_Msk \| HSIOM_AMUX_SPLIT_CTL_SWITCH_BB_SL_Msk \| HSIOM_AMUX_SPLIT_CTL_SWITCH_BB_SR_Msk;` |
   | Port 9 <br>Port 10                         | `HSIOM->AMUX_SPLIT_CTL[5] = HSIOM_AMUX_SPLIT_CTL_SWITCH_AA_SL_Msk \| HSIOM_AMUX_SPLIT_CTL_SWITCH_AA_SR_Msk \| HSIOM_AMUX_SPLIT_CTL_SWITCH_BB_SL_Msk \| HSIOM_AMUX_SPLIT_CTL_SWITCH_BB_SR_Msk;` |

* For CY8CKIT-062-WiFi-BT PSoC 6 WiFi-BT Pioneer Kit:

   | Port                                       | AMUX configuration                                           |
   | ------------------------------------------ | ------------------------------------------------------------ |
   | Port 0<br>Port 11<br>Port 12<br>Port 13    | `HSIOM->AMUX_SPLIT_CTL[5] = HSIOM_AMUX_SPLIT_CTL_SWITCH_AA_SL_Msk \| HSIOM_AMUX_SPLIT_CTL_SWITCH_AA_SR_Msk \| HSIOM_AMUX_SPLIT_CTL_SWITCH_BB_SL_Msk \| HSIOM_AMUX_SPLIT_CTL_SWITCH_BB_SR_Msk;` <br>`HSIOM->AMUX_SPLIT_CTL[6] = HSIOM_AMUX_SPLIT_CTL_SWITCH_AA_SL_Msk \| HSIOM_AMUX_SPLIT_CTL_SWITCH_AA_SR_Msk \| HSIOM_AMUX_SPLIT_CTL_SWITCH_BB_SL_Msk \| HSIOM_AMUX_SPLIT_CTL_SWITCH_BB_SR_Msk;` |
   | Port 1<br>Port 2<br>Port 3<br>Port 4       | `HSIOM->AMUX_SPLIT_CTL[2] = HSIOM_AMUX_SPLIT_CTL_SWITCH_AA_SL_Msk \| HSIOM_AMUX_SPLIT_CTL_SWITCH_AA_SR_Msk \| HSIOM_AMUX_SPLIT_CTL_SWITCH_BB_SL_Msk \| HSIOM_AMUX_SPLIT_CTL_SWITCH_BB_SR_Msk;` <br>`HSIOM->AMUX_SPLIT_CTL[4] = HSIOM_AMUX_SPLIT_CTL_SWITCH_AA_SL_Msk \| HSIOM_AMUX_SPLIT_CTL_SWITCH_AA_SR_Msk \| HSIOM_AMUX_SPLIT_CTL_SWITCH_BB_SL_Msk \| HSIOM_AMUX_SPLIT_CTL_SWITCH_BB_SR_Msk;` |
   | Port 5<br />Port 6<br />Port 7<br />Port 8 | `HSIOM->AMUX_SPLIT_CTL[4] = HSIOM_AMUX_SPLIT_CTL_SWITCH_AA_SL_Msk \| HSIOM_AMUX_SPLIT_CTL_SWITCH_AA_SR_Msk \| HSIOM_AMUX_SPLIT_CTL_SWITCH_BB_SL_Msk \| HSIOM_AMUX_SPLIT_CTL_SWITCH_BB_SR_Msk;` |
   | Port 9 <br>Port 10                         | `HSIOM->AMUX_SPLIT_CTL[5] = HSIOM_AMUX_SPLIT_CTL_SWITCH_AA_SL_Msk \| HSIOM_AMUX_SPLIT_CTL_SWITCH_AA_SR_Msk \| HSIOM_AMUX_SPLIT_CTL_SWITCH_BB_SL_Msk \| HSIOM_AMUX_SPLIT_CTL_SWITCH_BB_SR_Msk;` |

* For CY8CPROTO-062-4343W PSoC 6 Wi-Fi BT Prototyping Kit

   | Port                                       | AMUX configuration                                           |
   | ------------------------------------------ | ------------------------------------------------------------ |
   | Port 0<br>Port 11<br>Port 12<br>Port 13    | `HSIOM->AMUX_SPLIT_CTL[5] = HSIOM_AMUX_SPLIT_CTL_SWITCH_AA_SL_Msk \| HSIOM_AMUX_SPLIT_CTL_SWITCH_AA_SR_Msk \| HSIOM_AMUX_SPLIT_CTL_SWITCH_BB_SL_Msk \| HSIOM_AMUX_SPLIT_CTL_SWITCH_BB_SR_Msk;` <br>`HSIOM->AMUX_SPLIT_CTL[6] = HSIOM_AMUX_SPLIT_CTL_SWITCH_AA_SL_Msk \| HSIOM_AMUX_SPLIT_CTL_SWITCH_AA_SR_Msk \| HSIOM_AMUX_SPLIT_CTL_SWITCH_BB_SL_Msk \| HSIOM_AMUX_SPLIT_CTL_SWITCH_BB_SR_Msk;` |
   | Port 1<br>Port 2<br>Port 3<br>Port 4       | `HSIOM->AMUX_SPLIT_CTL[2] = HSIOM_AMUX_SPLIT_CTL_SWITCH_AA_SL_Msk \| HSIOM_AMUX_SPLIT_CTL_SWITCH_AA_SR_Msk \| HSIOM_AMUX_SPLIT_CTL_SWITCH_BB_SL_Msk \| HSIOM_AMUX_SPLIT_CTL_SWITCH_BB_SR_Msk;` <br>`HSIOM->AMUX_SPLIT_CTL[4] = HSIOM_AMUX_SPLIT_CTL_SWITCH_AA_SL_Msk \| HSIOM_AMUX_SPLIT_CTL_SWITCH_AA_SR_Msk \| HSIOM_AMUX_SPLIT_CTL_SWITCH_BB_SL_Msk \| HSIOM_AMUX_SPLIT_CTL_SWITCH_BB_SR_Msk;` |
   | Port 5<br />Port 6<br />Port 7<br />Port 8 | `HSIOM->AMUX_SPLIT_CTL[4] = HSIOM_AMUX_SPLIT_CTL_SWITCH_AA_SL_Msk \| HSIOM_AMUX_SPLIT_CTL_SWITCH_AA_SR_Msk \| HSIOM_AMUX_SPLIT_CTL_SWITCH_BB_SL_Msk \| HSIOM_AMUX_SPLIT_CTL_SWITCH_BB_SR_Msk;` |
   | Port 9 <br>Port 10                         | `HSIOM->AMUX_SPLIT_CTL[5] = HSIOM_AMUX_SPLIT_CTL_SWITCH_AA_SL_Msk \| HSIOM_AMUX_SPLIT_CTL_SWITCH_AA_SR_Msk \| HSIOM_AMUX_SPLIT_CTL_SWITCH_BB_SL_Msk \| HSIOM_AMUX_SPLIT_CTL_SWITCH_BB_SR_Msk;` |

# Reference

- [CSDIDAC Middleware API Reference Guide](https://cypresssemiconductorco.github.io/middleware-csdidac/csdidac_api_reference_manual/html/index.html)
- [ModusToolbox IDE User Guide](https://www.cypress.com/ModusToolboxUserGuide)
- [CY8CPROTO-062-4343W PSoC 6 Wi-Fi BT Prototyping kit](http://www.cypress.com/documentation/development-kitsboards/cy8cproto-063-4343w)
- [CY8CKIT-062-WiFi-BT PSoC 6 WiFi-BT Pioneer Kit](http://www.cypress.com/documentation/development-kitsboards/psoc-6-wifi-bt-pioneer-kit)
- [CY8CKIT-062-BLE PSoC 6 BLE Pioneer Kit](http://www.cypress.com/documentation/development-kitsboards/psoc-6-ble-pioneer-kit)

# Release Notes

| Version | Description                                                  |
| ------- | ------------------------------------------------------------ |
| 1.0     | Initial release. <br>Tested with mbed-os v5.13.1 and CSDIDAC MW v2.0 |