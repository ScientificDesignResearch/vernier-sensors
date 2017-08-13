/*
VernierAnalogAutoIDDisplay (v 2016.10)
Reads the information to AutoID any Vernier BTA sensor connected to the BTA 1 connector
on the Vernier Interface Shield. it will also work with homemade, breadboard connections
if all the lines are connected so AutoID can work. This sketch handles sensors with
memory chips and using the I2C lines and those with resistor autoID.

This sketch supports  (but does not require) the Two-Line Display,
sold by SparkFun as Serial Enabled 16x2 LCD display. See https://www.sparkfun.com/products/9395
The display should be connected as follows:
RX - Arduino Pin 9 (or pin 6 of the BTD 2 connector)
GND -Arduino GND Pin (or pin 5 of the BTD 2 connector)
+5V - Arduino +5V Pin (or pin 4 of the BTD 2 connector)

At startup, the program will list the Name, Units, and Calibration Page
for the sensor. After a two second delay, the Short Name and the senscor
reading, with units will be shown on the display.

If the D12 button on the Vernier Interface Shield is held down as the program starts, the
Sensor ID Number, equation type, intercept, slope, and c factor for the sensor will be
displayed briefly before the sensor readings appear.

This sketch also handles the two analog sensors that use the +/-10 Volt input line (VP-BTA and
30V-BTA).

This version supports non-linear sensors in special case If Statements. The non-linear sensor supported are:
thermistor temperture probe(10), Steinhart-Hart Equation
Wide Range Tempeature Sensor(78), reading = Intercept + Voltage * Slope + cfactor * Voltage * Voltage;
Ethanol Sensor(97), reading = Intercept * pow(Voltage, Slope)
Sound Level Sensor(118), reading = Intercept + Voltage * Slope + cfactor * Voltage * Voltage;
Melt Station(92), reading = Intercept + Voltage * Slope + cfactor * Voltage * Voltage;
ISEs, CA(38), NH4(39), NO3(40), Cl(41), Potasium(113) mV = (137.55 * Voltage - 0.1682);
New (Oct. 2016 Thermocouple(123), reading = Intercept + Voltage * Slope + cfactor * Voltage * Voltage;

This version reads the A-to-D converter many times quickly and averages readings to average out
noise. You can change the amount of averaging by changing the integer, numberAveraged, in the loop.

Note that this sketch handles multiple pages of sensor calibrations.

The only BTA (analog) sensors that are not read properly are Ion-Selective Electrodes. For those,
mV output is displayed.

After the AutoID:
As written, this sketch displays the time and sensor readings on the Serial Monitor.
The readings will be displayed every second.
Change the variable TimeBetweenReadings (just below) to change the rate.

When taking data with any sensor, if the D12 button on the Vernier Interface Shield is pressed,
the display will show the raw voltage instead of the sensor reading. You will also notice that
the LED will flicker (only on for a short time) while the voltage is being displayed.

This 2015.10 version was reworked to use less RAM. More constants were declared and PROGMEM was used
used to store a lot of strings in program memory. Earlier versions were sometimves acting strangely,
especially when using newer versions of the Arduino IDE, because the
sketch ran out of RAM. This version also added support for the new Sound Level Sensor and displays a
message if no sensor is attached.

See www.vernier.com/arduino for more information.
*/
// all the constants below are used for resistor ID sensors.
//#define DEBUG
const char Name_0[] PROGMEM = "Name 0";   // "Name 0" etc are Names to store - change to suit.
const char Name_1[] PROGMEM = "Termocouple";
const char Name_2[] PROGMEM = "Voltage +/-10V" ;
const char Name_3[] PROGMEM = "Current";
const char Name_4[] PROGMEM = "Resistance";
const char Name_5[] PROGMEM = "EL Temp" ;
const char Name_6[] PROGMEM = "missing" ;
const char Name_7[] PROGMEM = "missing" ;
const char Name_8[] PROGMEM = "Diff Voltage" ;
const char Name_9[] PROGMEM = "Current" ;
const char Name_10[] PROGMEM = "Temperature";
const char Name_11[] PROGMEM = "Voltage +/-30V" ;
const char Name_12[] PROGMEM = "TI Light";
const char Name_13[] PROGMEM = "Ex Heart Rate" ;
const char Name_14[] PROGMEM = "Raw Volts" ;
const char Name_15[] PROGMEM = "EKG" ;
const char Name_16[] PROGMEM = "missing16" ;
const char Name_17[] PROGMEM = "Carbon Dioxide" ;
const char Name_18[] PROGMEM = "Oxygen" ;

const char ShortName_0[] PROGMEM = "ShortName 0";   // "ShortName 0" etc are ShortNames to store - change to suit.
const char ShortName_1[] PROGMEM = "Temp";
const char ShortName_2[] PROGMEM = "Pot" ;
const char ShortName_3[] PROGMEM = "I";
const char ShortName_4[] PROGMEM = "Res";
const char ShortName_5[] PROGMEM = "Temp" ;
const char ShortName_6[] PROGMEM = "missing" ;
const char ShortName_7[] PROGMEM = "missing" ;
const char ShortName_8[] PROGMEM = "Pot" ;
const char ShortName_9[] PROGMEM = "I" ;
const char ShortName_10[] PROGMEM = "Temp";
const char ShortName_11[] PROGMEM = "Pot" ;
const char ShortName_12[] PROGMEM = "Light";
const char ShortName_13[] PROGMEM = "V" ;
const char ShortName_14[] PROGMEM = "V" ;
const char ShortName_15[] PROGMEM = "mV" ;
const char ShortName_16[] PROGMEM = "missing" ;
const char ShortName_17[] PROGMEM = "CO2" ;
const char ShortName_18[] PROGMEM = "O2" ;

const char Units_0[] PROGMEM = "Units 0";   // "Units 0" etc are Unitss to store - change to suit.
const char Units_1[] PROGMEM = "Deg C";
const char Units_2[] PROGMEM = "V" ;
const char Units_3[] PROGMEM = "I";
const char Units_4[] PROGMEM = "Ohms";
const char Units_5[] PROGMEM = "Deg C" ;
const char Units_6[] PROGMEM = "missing" ;
const char Units_7[] PROGMEM = "missing" ;
const char Units_8[] PROGMEM = "V" ;
const char Units_9[] PROGMEM = "A" ;
const char Units_10[] PROGMEM = "Deg C";
const char Units_11[] PROGMEM = "V" ;
const char Units_12[] PROGMEM = " ";
const char Units_13[] PROGMEM = "V" ;
const char Units_14[] PROGMEM = "V" ;
const char Units_15[] PROGMEM = "mV" ;
const char Units_16[] PROGMEM = "missing" ;
const char Units_17[] PROGMEM = "ppm" ;
const char Units_18[] PROGMEM = "%" ;

//Set up tables to refer to your char strings for Name, ShortName, and Units.
const char* const Name_table[] PROGMEM = {Name_0, Name_1, Name_2, Name_3, Name_4, Name_5, Name_6, Name_7, Name_8, Name_9,
                                          Name_10, Name_11, Name_12, Name_13, Name_14, Name_15, Name_16, Name_17, Name_18
                                         };
const char* const ShortName_table[] PROGMEM = {ShortName_0, ShortName_1, ShortName_2, ShortName_3, ShortName_4, ShortName_5, ShortName_6, ShortName_7, ShortName_8, ShortName_9,
                                               ShortName_10, ShortName_11, ShortName_12, ShortName_13, ShortName_14, ShortName_15, ShortName_16, ShortName_17,  ShortName_18
                                              };
const char* const Units_table[] PROGMEM = {Units_0, Units_1, Units_2, Units_3, Units_4, Units_5, Units_6, Units_7, Units_8, Units_9,
                                           Units_10, Units_11, Units_12, Units_13, Units_14, Units_15, Units_16, Units_17,  Units_18
                                          };
char buffer[20] = "                   ";  // make sure this is large enough for the largest Name it must hold
const byte ledPin = 13; //used to indicate data collection is going on
const byte buttonPin = 12; //button on Vernier Interface Shield
byte buttonState = 0;//variable for reading the pushbutton status
const int device = 0x50; // for I2C commmunication
byte SensorNumber = 0; //integer indicating sensor number
float VoltageID;//voltage read when checking for resistor-ID sensors
char Name[20] = "                   "; //initialized char strings, LongName is 20 characters
char ShortName[11] = "          "; //ShortName is 11 characters
char Units[10] = "         "; // Units is 10 characters
float Intercept = 0.000; // the three factors used for calibration:
float Slope = 1.00;
float cfactor = 0; //used only in complex calibrations
int Page;// page of memory where calibration is stored
byte CalEquationType;
const byte Channel = 1; //always BTA Channel 1 in this script
/////////////////////////////////////////
const int TimeBetweenReadings = 1000; // in ms (change this to control rate of data collection)
////////////////////////////////////////
//#include <math.h> // (no longer needed!!!) library used in thermistor calibrations
#include <avr/pgmspace.h> //needed to allow PROGMEM to store dat in static ram
#include <Wire.h>  //library used in digital autoID
#include <SoftwareSerial.h> //library used in printing to display
SoftwareSerial mySerial(3, 9); //for display, pin 9 = TX, pin 3 = RX (unused)

void setup()
{
	Serial.begin(9600);// for printing to monitor
	mySerial.begin(9600); // for sending characters to display
	delay(500); // wait for display to boot up
	mySerial.write(124); // adjust backlight brightness of display
	mySerial.write(150); //max=157, 150=73%, 130=40%,128=off
	delay(500); // wait for display to adjust
	ClearDisplay();
	pinMode(ledPin, OUTPUT); //LED on SparkFun Vernier Shield
	// Set up button input pin:
	pinMode(buttonPin, INPUT_PULLUP);
	const int muxlsb = 10; //low byte of multiplexer
	const int muxmsb = 11; //high byte of multiplexer
	pinMode(muxlsb, OUTPUT);
	pinMode(muxmsb, OUTPUT);
	digitalWrite(muxlsb, LOW); //set multiplexer for BTA1
	digitalWrite(muxmsb, LOW);
	//Read BTA1 Sensor
	BTAResistorSensorID(Channel);// check for resistor AutoID
	if (SensorNumber == 0) //if no resistor AutoID found. check for digital AutoID
	{
		Wire.begin(); //start I2C communication
		pinMode(A4, OUTPUT); //set up I2C lines
		pinMode(A5, OUTPUT);
		DigitalSensorID(Channel);// check for digital AutoID
		pinMode(A4, INPUT); //turn off I2C lines
		pinMode(A5, INPUT); // if you do not turn these off, some analog readings are affected
	}
	// Special handling for ISEs, CA, NH4, NO3, or Cl
	if  (SensorNumber > 37 && SensorNumber < 42) strncpy(Units, "(mV)", 9);
	//Special calibration for Potasium ISE:
	if  (SensorNumber == 113) strncpy(Units, "(mV)", 9); // assign name based on sensor number;
#if defined DEGUG //
	PrintSensorInfo();// useful for debugging; lines between if and endif are executed only if there is a "#define DEBUG" is in the code
#endif
	// The Print statements below form the heading of a table that can be easily
	// transferred to the Vernier Logger Pro computer program for Mac or Windows.
	Serial.println(" ");
	Serial.println("Vernier Format 2");
	Serial.println("Readings taken using Ardunio");
	Serial.println("Data Set");
	Serial.print("Time");
	Serial.print("\t"); //tab character
	Serial.println(Name);
	Serial.print("t");
	Serial.print("\t"); //tab character
	Serial.println(ShortName);
	Serial.print("seconds");
	Serial.print("\t"); //tab character
	Serial.println (Units);
	//send characters to 2-line display: (you could get rid of all of this if you do not have a 2-line display)
	if (SensorNumber == 0)
	{
		strncpy(Name, " Plug a Sensor", 16);
		mySerial.print(Name);
		mySerial.write(254); // cursor to beginning of 2nd line
		mySerial.write(192);
		strncpy(Name, " into Analog 1", 16);
		mySerial.print(Name);
		delay (1500);
		ClearDisplay();
		strncpy(Name, " and then, press ", 16);
		mySerial.print(Name);
		mySerial.write(254); // cursor to beginning of 2nd line
		mySerial.write(192);
		strncpy(Name, "<RESET> button", 16);
		mySerial.print(Name);
		delay (2000);
		ClearDisplay();
	}// end of special message, if no sensor
	//Display Name, Units, and Page
	mySerial.print(Name); // display name on first line
	mySerial.write(254); // cursor to beginning of second line (position 196)
	mySerial.write(192);
	mySerial.print(Units); // display units at start of second line
	mySerial.write(254);// cursor to the end of second line
	mySerial.write(204);
	mySerial.print("p=");// display page at end of second line (-1 if resistor ID)
	mySerial.print(Page);
	delay(2000); // short delay
	ClearDisplay();
	//Special section to display SensorNumber, equation type, slope, intercept, and c, iff button is pressed:
	buttonState = digitalRead(buttonPin);
	// check if the pushbutton is pressed.
	// if it is, the buttonState is HIGH:
	if (buttonState == LOW)
	{
		digitalWrite(ledPin, HIGH);
		//Display SensorNumber, cal equation type, and a (intercept):
		mySerial.print("#");
		mySerial.print(SensorNumber); // Sensor ID number
		mySerial.write(254);// cursor to end of first line
		mySerial.write(134);
		mySerial.print(" cal eq="); // // display cal eq type
		mySerial.print(CalEquationType); // // display page at end of second line
		mySerial.write(254); // cursor to beginning of 2nd line
		mySerial.write(192);
		mySerial.print("a(int)="); // one space
		mySerial.print(Intercept); //
		delay (2000);
		ClearDisplay();
		//Display b(slope), and c (cfactor):
		mySerial.print("b(sl)="); // one space
		mySerial.print(Slope); //
		mySerial.write(254); // cursor to beginning of second line
		mySerial.write(192);
		mySerial.print("c=");
		mySerial.print(cfactor); //
		delay (2000);
		ClearDisplay();
	}// end of special display done if button is down
	//set up screen for displaying ShortName, Units, and data:
	mySerial.print(ShortName); // display Short Name
	mySerial.write(254);// cursor to end of first line
	mySerial.write(138);
	mySerial.print(Units); // display Units
}

void loop()
{
	int Count; //reading from 0 to 5 volt input
	int AltCount ;//reading from -10 to +10V input
	float Voltage;
	float SensorReading = 0.00;
	float Time;
	float sum = 0;
	const int numberAveraged = 100; //to average out noise, we take a lot of readings quickly and average, you can change this
	int ReadingNumber = 0;
	digitalWrite(ledPin, HIGH); //turn on LED
	Serial.print(ReadingNumber / 1000.0 * TimeBetweenReadings);
	Serial.print("\t"); //tab character
	if  (SensorNumber == 2 || SensorNumber == 11) // one of two sensors using the +/- 10 volt line
	{
		for (int i = 0 ; i < numberAveraged; i++)
		{
			AltCount = analogRead(A1); //read  +/- 10 volt lin
			sum = sum + AltCount;
		}
		Voltage = sum / numberAveraged / 1024.0 * 5.0 ; // convert from count to raw voltage if using 10 volt range:
	}
	else
	{
		for (int i = 0 ; i < numberAveraged; i++)
		{
			Count = analogRead(A0); //read 0 to 5 volt analog lines
			sum = sum + Count;
		}
		Voltage = sum / numberAveraged / 1024.0 * 5.0; ; //convert average count to voltage (0 to 5 volt input)
	}
	SensorReading = Intercept + Voltage * Slope; //for all linear sensors
	//the code below deals with BTA sensors which have non-linear calibrations
	//special calibration for thermistor temperture probe(10):
	if (SensorNumber == 10) SensorReading = Thermistor(Count); //TMP or STS sensors
	//Special calibration for Wide Range Tempeature Sensor(78):
	if  (SensorNumber == 78 ) SensorReading = Intercept + Voltage * Slope + cfactor * Voltage * Voltage;
	//Special quadratic calibration for Ethanol Sensor(97):
	if  (SensorNumber == 97) SensorReading = Intercept * pow(Voltage, Slope);
	//Special quadratic calibration for Sound Level Sensor(118)
	if  (SensorNumber == 118) SensorReading = Intercept + Slope * Voltage + cfactor * Voltage * Voltage;
	//Special calibration for Melt Station(92):
	if  (SensorNumber == 92) SensorReading = Intercept + Voltage * Slope + cfactor * Voltage * Voltage;
	//Special calibration for ISEs, CA(38), NH4(39), NO3(40), Cl(41):
	if  (SensorNumber > 37 && SensorNumber < 42) SensorReading = (137.55 * Voltage - 0.1682);
	//Special calibration for Potasium(113) ISE:
	if  (SensorNumber == 113) SensorReading = (137.55 * Voltage - 0.1682);//Potasium ISE
  if  (SensorNumber == 123) SensorReading = Intercept + Voltage * Slope + cfactor * Voltage * Voltage;
  //Special quadratic calibration for New (Oct. 2016 Thermocouple(123));
	mySerial.write(254); // command character
	mySerial.write(192); // move to line 2, position 0,
	mySerial.print("   "); // print some text
	buttonState = digitalRead(buttonPin);
	// check if the pushbutton is pressed.
	// if it is, the buttonState is HIGH:
	if (buttonState == LOW) // button down
	{
		digitalWrite(ledPin, LOW);
		Serial.print("V= ");
		Serial.println(Voltage, 1);  // display raw voltage on Serial Monitor
		mySerial.print("V= ");
		mySerial.print(Voltage); // print our float
	}
	else
	{
		Serial.println(SensorReading);   // display Sensor Reading on monitor
		mySerial.print(SensorReading);
	}
	delay(TimeBetweenReadings / 2); // delay half of period
	digitalWrite(ledPin, LOW);// LED on D13 flashes once per readng
	delay(TimeBetweenReadings / 2); // delay the other half
	mySerial.write(254); // cursor to beginning of first line
	mySerial.write(192);
	mySerial.print("                "); // clear 2nd line of display for next loop
	ReadingNumber++;
}

void BTAResistorSensorID(int Channel)
{
	float VCC = 5.00; //"5 volt" power supply voltage used in resistor ID section and themistor calibration
	Page = -1; //assign to -1 so resistor ID sensors read this value
	delay (10);// delay just a bit before reading voltage (may not be important)
	int CountID = analogRead(A5);
	VoltageID = CountID / 1024.0 * VCC;// convert from count to voltage
	if (VoltageID > 0.86 & VoltageID < 0.95) SensorNumber = 1; //Thermocouple
	if (VoltageID > 3.72 & VoltageID < 3.86) SensorNumber = 2; // Voltage +/-10 V
	if (VoltageID > 1.92 & VoltageID < 2.13) SensorNumber = 3; // TI Current Probe (not used)
	if (VoltageID > 1.18 & VoltageID < 1.30) SensorNumber = 4; //Resistance
	if (VoltageID > 3.27 & VoltageID < 3.68) SensorNumber = 5; //Extra-Long Temperature Probe
	if (VoltageID > 4.64 & VoltageID < 4.73) SensorNumber = 8; //Differential Voltage
	if (VoltageID > 4.73 & VoltageID < 4.82) SensorNumber = 9; //Current
	if (VoltageID > 2.38 & VoltageID < 2.63) SensorNumber = 10; //Stainless Steel or Surface Temperature Probe
	if (VoltageID > 2.85 & VoltageID < 3.15) SensorNumber = 11; // Voltage 30 V
	if (VoltageID > 1.52 & VoltageID < 1.68) SensorNumber = 12; //TILT, TI Light Sensor
	if (VoltageID > 0.43 & VoltageID < 0.48) SensorNumber = 13; //Exercise Heart Rate
	if (VoltageID > 4.08 & VoltageID < 4.16) SensorNumber = 14; //Raw Voltage
	if (VoltageID > 0.62 & VoltageID < 0.68) SensorNumber = 15; //EKG
	if (VoltageID > 4.32 & VoltageID < 4.40) SensorNumber = 17; //CO2
	if (VoltageID > 4.50 & VoltageID < 4.59) SensorNumber = 18; //Oxygen
	if (SensorNumber != 0) // if any resistor ID sensor found
	{
		strcpy_P(buffer, (char*)pgm_read_word(&(Name_table[SensorNumber]))); // Necessary casts and dereferencing
		strcpy(Name, buffer);// assign name based on sensor number
		strcpy_P(buffer, (char*)pgm_read_word(&(ShortName_table[SensorNumber]))); // Necessary casts and dereferencing, just copy.
		strncpy(ShortName, buffer, 10); // assign name based on sensor number
		strcpy_P(buffer, (char*)pgm_read_word(&(Units_table[SensorNumber]))); // Necessary casts and dereferencing, just copy.
		strncpy(Units, buffer, 9); // assign name based on sensor number
		Page = -1; //calibration storage page
		CalEquationType = 1; // for all but thermistors; it will be changed in that case below
	}
	switch (SensorNumber)
	{
		case 1:
			// Thermocouple ;
			Slope = -2.45455;
			Intercept = 6.2115;
			break;
		case 2:
			// Voltage +/- 10V" ;
			Slope = 4; //note correction for Sparkfun circuit done in calculation of Voltage!
			Intercept = -10;
			break;
		case 3:
			// Current;
			Slope = -2.665;
			Intercept = 6.325;
			break;
		case 4:
			// Resistance ;
			Slope = -2.5;
			Intercept = 6.25;
			break;
		case 5:
			// EL Temp   //Extra-Long Temperature Probe
			Slope = 58.341;
			Intercept = -53.073;
			break;
		case 8:
			// Diff Voltage ;
			Slope = -2.5;
			Intercept = 6.25;
			break;
		case 9:
			// Current ;
			Slope = -0.25;
			Intercept = 0.625;
			break;
		case 10:
			// Temperature ;
			Slope = 1;
			Intercept = 0;
			CalEquationType = 12; //Steinhart-Hart (for this sensor only)
			break;
		case 11:
			// Voltage +/- 30V" ;//
			Slope = 15.41;
			Intercept = -40.35;
			break;
		case 12:
			// TI Light ;
			Slope = 1;
			Intercept = 0;
			break;
		case 13:
			// Exercise Heart Rate ;
			Slope = 1;
			Intercept = 0;
			break;
		case 14:
			// Raw Voltage ;
			Slope = 1;
			Intercept = 0;
			break;
		case 15:
			// EKG ;
			Slope = 1;
			Intercept = 0;
			break;
		case 17:
			// Carbon Dioxide ;
			Slope = 1;
			Intercept = 0;
			break;
		case 18:
			// Oxygen ;
			Slope = 1;
			Intercept = 0;
			break;
		default:
			Units[1] = 0; // Units is 10 charactersSlope=1;
			Intercept = 0;
			break;
	} // end of switch case
} //end of BTA resistor check

void DigitalSensorID(int Channel)
{
	int i;
	int x;
	char c;
	byte sensordata [128];
	byte floatbyte[5];
	for (i = 0; i < 129; i++) // clear our digital ID sensor data
	{
		sensordata[i] = 0;
	}
	//Now check for Digital AutoID sensor:
	Wire.begin(); // join I2C bus (address optional for master)
	//Reading device first time... ;
	Wire.beginTransmission(device); // Now we're going to read it back
	Wire.write(0x0); // Sending address 0, so it knows where we'll want
	Wire.endTransmission();
	x = Wire.requestFrom(device, 32);  // Start new transmission and keep reading for 32 bytes
	// note: the default buffer size for Arduino is 23 bytes. You can change it to larger. It would be
	// desirable to change it to 128 bytes and read all the data in one read. That is the way all Vernier
	//interfaces do it. is done as follows:  add#define SERIAL_BUFFER_SIZE 128 !!!
	//check it in your sketch with:
	// Serial.print(SERIAL_BUFFER_SIZE);
	i = 1;
	while(x > 1)
	{
		x = Wire.available();
		byte a = Wire.read();    // Read a byte and write it out to the Serial port
		sensordata [i] = a;
		i++;
	}
	//Reading device second time... ;
	Wire.beginTransmission(device); // Now we're going to read it back
	Wire.write(0x20);               // Sending address 0, so it knows where we'll want
	Wire.endTransmission();       // to read from
	x = Wire.requestFrom(device, 32);  // Start new transmission and keep reading for 128 bytes
	i = 1;
	while(x > 1)
	{
		x = Wire.available();
		byte c = Wire.read();    // Read a byte and write it out to the Serial port
		sensordata [i + 32] = c;
		i++;
	}
	//Reading device third time... ;
	Wire.beginTransmission(device); // Now we're going to read it back
	Wire.write(0x40);               // Sending address 0, so it knows where we'll want
	Wire.endTransmission();       // to read from
	x = Wire.requestFrom(device, 32);  // Start new transmission and keep reading for 128 bytes
	i = 1;
	while(x > 1)
	{
		x = Wire.available();
		byte c = Wire.read();    // Read a byte and write it out to the Serial port
		sensordata [i + 64] = c;
		i++;
	}
	//Reading device a forth time... ;
	Wire.beginTransmission(device); // Now we're going to read it back
	Wire.write(0x60);               // Sending address 0, so it knows where we'll want
	Wire.endTransmission();       // to read from
	x = Wire.requestFrom(device, 32);  // Start new transmission and keep reading for 128 bytes
	i = 1;
	while(x > 1)
	{
		x = Wire.available();
		byte c = Wire.read();    // Read a byte and write it out to the Serial port
		sensordata [i + 96] = c;
		i++;
	}
#if defined DEBUG
	Serial.print("array: ");  //only if "#define DEBUG" is in the code
	for (i = 0; i < 129; i++)
	{
		Serial.print (i);
		Serial.print (" ");
		Serial.print(sensordata[i]);
		Serial.print (" ");
		Serial.println(char(sensordata[i]));
	}
#endif
	//Determine sensor number:
	//  VoltageID[Channel]=-1;// indicated resistor ID not used
	SensorNumber = sensordata[2];
	//Determine the sensor name:
	for (i = 0; i < 18; i++)
	{
		c =  sensordata[i + 9];
		if (c != 0) Name[i] = c;
	}
	//Determine the short name:
	for (i = 0; i < 8; i++)
	{
		c =  sensordata[i + 29];
		if (c != 0) ShortName[i] = c;
	}
	//Determine the calibration equation type:
	CalEquationType = sensordata[59];
	//Determines the  calibration page:
	Page = sensordata[70];
	// the code below uses the calibration page set:
	// Intercept starts at 71 for page 1, 90 for p2, and 109 for p3
	//Determines intercept:
	for (i = 0; i < 4; i++)
	{
		floatbyte[i] = sensordata[i + 71 + (Page) * 19];
	}
	float j = *(float*) &floatbyte;
	Intercept = j;
	//Determines slope:
	// slope starts at 75 for page 1, 94 for p2, and 113 for p3
	for (i = 0; i < 4; i++)
	{
		floatbyte[i] = sensordata[i + 75 + (Page) * 19];
	}
	float y = *(float*) &floatbyte;
	Slope = y;
	//Determines the cfactor:
	// cfactor starts at 79 for page 1, 98 for p2, and 117 for p3
	for (i = 0; i < 4; i++)
	{
		floatbyte[i] = sensordata[i + 79 + (Page) * 19];
	}
	float z = *(float*) &floatbyte;
	cfactor = z;
	//Determine the units:
	// the cryptic code in the next line just uses the
	//correct bytes, depending on the page selected.
	// units start at 83 for page 1, 102 for p2, and 121 for p3
	for (i = 0; i < 8; i++)
	{
		c =  sensordata[83 + i + (Page) * 19];
		Units[i] = c;
	}
} //end of checking for digital ID sensor

void ClearDisplay() // and move cursor to start of top line
{
	mySerial.write(254); // cursor to beginning of first line
	mySerial.write(128);
	mySerial.print("                "); // clear display for next loop
	mySerial.print("                ");
	mySerial.write(254); // cursor to beginning of first line
	mySerial.write(128);
}

void PrintSensorInfo()
{
	// print out information about sensor:
	Serial.println(" ");
	Serial.print("BTA ");
	Serial.println(Channel);
	Serial.print("sensor number: ");
	Serial.println(SensorNumber);
	Serial.print("ID voltage level: ");
	Serial.println(VoltageID);
	Serial.print("sensor name: ");
	Serial.println (Name);
	Serial.print("sensor short name: ");
	Serial.println (ShortName);
	Serial.print("calibration page: ");
	Serial.println(Page);
	Serial.print("calibration equation type: ");
	Serial.println(CalEquationType);
	Serial.print("intercept: ");
	Serial.println (Intercept);
	Serial.print("slope ");
	Serial. println(Slope);
	Serial.print("cfactor ");
	Serial. println(cfactor);
	Serial.print("units: ");
	Serial.println (Units);
	// end of PrintSensorInfo
}

float Thermistor(int Raw) //This function calculates temperature from ADC count
{
	/* Inputs ADC count from Thermistor and outputs Temperature in Celsius
	*  requires: include <math.h>
	* There is a huge amount of information on the web about using thermistors with the Arduino.
	* Here we are concerned about using the Vernier Stainless Steel Temperature Probe TMP-BTA and the
	* Vernier Surface Temperature Probe STS-BTA, but the general principles are easy to extend to other
	* thermistors.
	* This version utilizes the Steinhart-Hart Thermistor Equation:
	*    Temperature in Kelvin = 1 / {A + B[ln(R)] + C[ln(R)]^3}
	*   for the themistor in the Vernier TMP-BTA probe:
	*    A =0.00102119 , B = 0.000222468 and C = 1.33342E-7
	*    Using these values should get agreement within 1 degree C to the same probe used with one
	*    of the Vernier interfaces
	*
	* Schematic:
	*   [Ground] -- [thermistor] -------- | -- [15,000 ohm resistor] --[Vcc (5v)]
	*                                     |
	*                                Analog Pin 0
	For the circuit above:
	* Resistance = ( Count*RawADC /(1024-Count))
	*/
	long Resistance;
	float Resistor = 15000; //fixed resistor
// the measured resistance of your particular fixed resistor in
// the Vernier BTA-ELV and in the SparkFun Vernier Adapter Shield
// is a precision 15K resisitor
	float Temp;  // Dual-Purpose variable to save space.
	Resistance = ( Resistor * Raw / (1024 - Raw));
	Temp = log(Resistance); // Saving the Log(resistance) so not to calculate  it 4 times later
	Temp = 1 / (0.00102119 + (0.000222468 * Temp) + (0.000000133342 * Temp * Temp * Temp));
	Temp = Temp - 273.15;  // Convert Kelvin to Celsius
	return Temp;                                      // Return the Temperature
}




