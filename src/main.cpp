/*
  RadioLib LoRaWAN ABP Example

  ABP = Activation by Personalisation, an alternative
  to OTAA (Over the Air Activation). OTAA is preferable.

  This example will send uplink packets to a LoRaWAN network. 
  Before you start, you will have to register your device at 
  https://www.thethingsnetwork.org/
  After your device is registered, you can run this example.
  The device will join the network and start uploading data.

  LoRaWAN v1.0.4/v1.1 requires the use of persistent storage.
  As this example does not use persistent storage, running this 
  examples REQUIRES you to check "Resets frame counters"
  on your LoRaWAN dashboard. Refer to the notes or the 
  network's documentation on how to do this.
  To comply with LoRaWAN's persistent storage, refer to
  https://github.com/radiolib-org/radiolib-persistence

  For default module settings, see the wiki page
  https://github.com/jgromes/RadioLib/wiki/Default-configuration

  For full API reference, see the GitHub Pages
  https://jgromes.github.io/RadioLib/

  For LoRaWAN details, see the wiki page
  https://github.com/jgromes/RadioLib/wiki/LoRaWAN

*/


#include "configABP.h"

uint32_t counter=0;

void setup() { 
  SetupBoard();
  SerialInit();
  RadioBeginSPI();
  delay(5000);  // Give time to switch to the serial monitor
  serial.println("\nSetup ... ");
  serial.println("Initialise the radio");
  int state = radio.begin();
  debug(state != RADIOLIB_ERR_NONE, F("Initialise radio failed"), state, true);
  serial.println(F("Initialise LoRaWAN Network credentials"));
  node.setDutyCycle(false);
  node.setDwellTime(false);
  node.beginABP(devAddr, NULL, NULL, nwkSEncKey, appSKey);
  node.activateABP(DR_SF7);
  debug(state != RADIOLIB_ERR_NONE, F("Activate ABP failed"), state, true);
  serial.printf("DEVADDR: 0x%x\n\r", node.getDevAddr());
  serial.println(F("Ready!\n"));
  
}

void loop() { 
  serial.printf("\nSending uplink\n\r");

  // Build payload byte array
  uint8_t uplinkPayload[242];
  snprintf((char*) uplinkPayload, sizeof(uplinkPayload), "Hello World Number %d", node.getFCntUp());
  uint8_t size = strlen((char*) uplinkPayload);
  serial.printf("Packet: %s\n", (char*) uplinkPayload);
  serial.printf("Payload Size: %d\n", size);

  // Perform an uplink
  int state = node.sendReceive(uplinkPayload, size);
  debug(state < RADIOLIB_ERR_NONE, F("Error in sendReceive"), state, false);
  RadioLibTime_t tempo = node.getLastToA();
  serial.printf("%lu milliseconds to send (ToA)\n", tempo);
  counter++;
  // Check if a downlink was received 
  // (state 0 = no downlink, state 1/2 = downlink in window Rx1/Rx2)
  if(state > 0) {
    serial.println(F("Received a downlink"));
  } else {
    serial.println("No downlink received");
  }

  // Wait until next uplink - observing legal & TTN FUP constraints
  // delayInterval(uplinkIntervalSeconds);
  delay(1000 * uplinkIntervalSeconds);
}