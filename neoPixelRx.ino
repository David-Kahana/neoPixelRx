// NeoPixel Ring simple sketch (c) 2013 Shae Erisson
// released under the GPLv3 license to match the rest of the AdaFruit NeoPixel library
#include <Adafruit_NeoPixel.h>
#ifdef __AVR__
  #include <avr/power.h>
#endif
#include "D:\\WORK\\David\\terminal\\Protocol.h"
#define PIN 2 // Which pin on the Arduino is connected to the NeoPixels?
#define NUMPIXELS 128 // How many NeoPixels are attached to the Arduino?

Adafruit_NeoPixel pixels = Adafruit_NeoPixel(NUMPIXELS, PIN, NEO_GRB + NEO_KHZ800);
word delayval = 1; // delay for half a second
byte bright = 255;
word inByte = 0;



byte inBuf[400]; //input buffer
byte outBuf[10]; //output buffer
char* buf = (char*)inBuf;
byte bytesToRead = 8;
word bufInd = 0; 
word chkInd = 0;
bool finishedFrame = false;
//bool newByte = false;
byte mode = 0; 
MessageConfigPixle* conf = nullptr;
MessageReplay* rep = nullptr;
MessageFrame frame;
word dataBytes = 0;
void setup() 
{
	Serial.begin(115200);
	delay(100);
	pixels.begin(); // This initializes the NeoPixel library.
	delay(100);
	pixels.setBrightness(bright);
	for(word i = 0; i < NUMPIXELS; i++)
	{
		// pixels.Color takes RGB values, from 0,0,0 up to 255,255,255
		pixels.setPixelColor(i, pixels.Color(12,12,12));
	}
	pixels.show();
	delay(100);
}

void loop() 
{
	switch (mode)
	{
	case 0:  // getting config
		if (Serial.available() > 0)
		{
			if (bufInd > 399)
			{
				bufInd = 0;
			}
			inBuf[bufInd] = (byte)Serial.read();
			bufInd++;
			if (bufInd >= 19)
			{
				chkInd = bufInd - 19;
				conf = (MessageConfigPixle *)(&(inBuf[chkInd]));
				if (conf->start == START_MSG && conf->end == END_MSG)
				{
					//Serial.println("MSG");
					Serial.println("DEBUG: cfg");
					dataBytes = conf->frameSize + 7;
					mode = 1;
					bufInd = 0;
				}
			}
		}
		break;
	case 1: //sending ACK
		//Serial.println("DEBUG: ack");
		rep = (MessageReplay *)(&(outBuf[0]));
		rep->start = START_MSG;
		rep->size = 8;
		rep->msgType = MessageTypes::REPLAY;
		rep->replay = Replays::ACK;
		rep->end = END_MSG;
		Serial.write(outBuf, 8);
		mode = 2; 
		bufInd = 0;
		break;
	case 2:  // getting data
		if (Serial.available() > 0)
		{
			if (bufInd > 399)
			{
				bufInd = 0;
			}
			inBuf[bufInd] = (byte)Serial.read();
			bufInd++;
			if (bufInd >= dataBytes)
			{
				//Serial.println("DEBUG: data > 391");
				chkInd = bufInd - dataBytes;
				frame.start = inBuf[chkInd];
				frame.msgType = (MessageTypes)inBuf[chkInd + 5];
				frame.data = &(inBuf[chkInd + 6]);
				frame.end = inBuf[chkInd + dataBytes - 1];
				//Serial.print("D[0:5]: ");
				//Serial.print((char)inBuf[chkInd]);
				//Serial.print(", ");
				//Serial.print(inBuf[chkInd + 1]);
				//Serial.print(", ");
				//Serial.print(inBuf[chkInd + 2]);
				//Serial.print(", ");
				//Serial.print(inBuf[chkInd + 3]);
				//Serial.print(", ");
				//Serial.print(inBuf[chkInd + 4]);
				//Serial.print(", ");
				//Serial.print(inBuf[chkInd + 5]);
				//Serial.print(", ");
				//Serial.println((char)inBuf[chkInd + dataBytes - 1]);
				if (frame.start == START_MSG && frame.end == END_MSG)
				{
					//Serial.println("DEBUG: DATA");

					for (word i = 0; i < NUMPIXELS * 3; i+=3)
					{
						pixels.setPixelColor(i/3, pixels.Color(*(frame.data + i), *(frame.data + i + 1), *(frame.data + i + 2)));
					}
					pixels.show();
					delay(5);
					mode = 1;
					bufInd = 0;
				}
			}
		}
		break;

	}
	
	//if (Serial.available() >= 1)
	//{
	//	Serial.readBytes(buf + bufInd, bytesToRead);
	//	bufInd += bytesToRead;
	//	if (bufInd >= 7)
	//	{

	//		//bufInd = 0;
	//		finishedFrame = true;
	//	}
	//}
	//unsigned long start = millis();
	//for (int i = 0; i < 100; ++i)
	//{
	//	pixels.show();
	//}
	//Serial.println(millis() - start);
	//delay(1000);
}
