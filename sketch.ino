#include <SPI.h>
#include <Ethernet.h>
#include <NinjaBlock.h>
//#include <MemoryFree.h>

#include <RCSwitch.h>

RCSwitch mySwitch = RCSwitch();

void setup(){
    Serial.begin(9600);
    Serial.println("Starting...");
    pinMode(11, OUTPUT); 
    delay(1000);   // This delay is to wait for the Ethernet Controller to get ready

    NinjaBlock.host = "api.ninja.is";
    NinjaBlock.port = 80;
    NinjaBlock.nodeID = "ARDUINOBLOCK";
    NinjaBlock.token = "YOURTOKENHERE"; // Get yours from https://a.ninja.is/hacking 
    NinjaBlock.guid = "0";
    NinjaBlock.vendorID=0;
    NinjaBlock.deviceID=11;

    if (NinjaBlock.begin()==0) {
      Serial.println("Init failed");
    }
    // Listen on Interrupt 0 (=Pin2)
    mySwitch.enableReceive(0);

}

void loop() {
  if (mySwitch.available()) {
    // If Data is Available, Read it and Fill with Zeros
    char* b = dec2binWzerofill(mySwitch.getReceivedValue(),mySwitch.getReceivedBitlength());
    // Finally send it to the cloud
    NinjaBlock.send(b);
    mySwitch.resetAvailable();
  }
  if(NinjaBlock.receive()) {
    // Check whether this data is for us
    if ((NinjaBlock.intDID == 11) && (NinjaBlock.intVID == 0)) {     
      // enable transmitter connected to Pin11
      mySwitch.enableTransmit(11);
      mySwitch.setPulseLength(350);
      // Send it 8 times to make sure it gets transmitted
      mySwitch.setRepeatTransmit(8);  
      mySwitch.send(NinjaBlock.strDATA);
      // Disable Transmitter
      mySwitch.disableTransmit();
    }
  }
}


// From RCSwitch Library
static char * dec2binWzerofill(unsigned long Dec, unsigned int bitLength){
  static char bin[64]; 
  unsigned int i=0;
  
  while (Dec > 0) {
    bin[32+i++] = (Dec & 1 > 0) ? '1' : '0';
    Dec = Dec >> 1;
  }
  
  for (unsigned int j = 0; j< bitLength; j++) {
    if (j >= bitLength - i) {
      bin[j] = bin[ 31 + i - (j - (bitLength - i)) ];
    } else {
      bin[j] = '0';
    }
  }
  bin[bitLength] = '\0';
  
  return bin;
}
