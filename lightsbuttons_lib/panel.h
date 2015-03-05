void onKeyPress(unsigned char);
void onKeyUp(unsigned char);
void pollUI();
void writeLeds(unsigned char leds);
void setLedBacklight(unsigned char led, unsigned char brightness);
void writeLed(unsigned char led, unsigned char state);
void writeKey(char key, unsigned char state);
void commandRecieved();
