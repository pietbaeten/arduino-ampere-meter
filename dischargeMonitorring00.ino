/* To arduino:
>> Symmetrical for Bat+ en Bat-:
-> ADC value giving Ampères with 2 seconds integrator function.
>> should be checked every 500ms to 1000ms
-> ADC value of the Bat+ voltage (not stable if power was drawn,
should be given some rest in order to recover and have indicative
voltage.. On the other hand if power is drawn, we have an indication
on the core battery voltage available. We also can calculate the 
internal resistance!
>> Extra digital input for overcurrent detection..(foreseen for future)
*/
/* From arduino:
Symmetrical for Bat+ and Bat-:
-> Three LED's green, yellow and red to indicate the total charge used
-> A blue LED to indicate the current momentarily drawn (maybe to made
outside of arduino !!) -- PWM output @ 2Hz | % duty cycle
-> Auditive signal reflection currrent momentarily
drawn. 50% duty cycle but shifting frequency
-> A Flash memory that can be read to get more extensive information,
on last usage cycle and overall condition of battery!
*/
/*
Within Arduino:
Symmetrical for Bat+ and Bat-:
-> Initialisation and power-down functions
Power-up Retrieve init values from Flash
Reset Button to reset monitorring after battery charged
End cycle Store init values to Flash
-> Cyclic function in order of 500ms:
-> store to Flash every 5 minutes
*/
//Bat+
#define ADC_AsBatPlus  4  //The Analogue input for the Ampère seconds of the + Battery

#define OUT_GreenLedBatPlus   2  //Three LEDs indicating the state of charge of the + Battery
#define OUT_YellowLedBatPlus  4  
#define OUT_RedLedBatPlus     5

#define PWM_BlueLedBatPlus  3  //The Blue LED blinking @ a few Hz with duty Cycle indicating the current

#define PWM_AudioBatPlus    9  //An audio signal pitching in frequency relative to the current

//Bat-
#define ADC_AsBatMinus  5

#define OUT_GreenLedBatMinus  6
#define OUT_YellowLedBatMinus  7
#define OUT_RedLedBatMinus    8

#define PWM_BlueLedBatMinus  10

#define PWM_AudioBatMinus    11

#define OUT_Dia1 12
#define OUT_Dia2 13

// general parameters
#define SampleRate  500    //every 500ms to be sampled
#define AsBatCharged 5000000    //Available Ampère . seconds after battery is charged (measured and calculated from electronic circuitry and for 500ms sample rate)
#define AsBat90  4000000
#define AsBat80  3000000
#define AsBat70  2000000
#define AsBat60  1000000

// global variables
long BatPlusAvailableCharge = AsBatCharged; //The total remaining charge on the + Battery
long BatMinusAvailableCharge = AsBatCharged;  //The total remaining charge on the - Battery
unsigned int AsBatPlus;  //Ampère . seconds of the + Battery
unsigned int AsBatMinus;  //Ampère . seconds of the - Battery
long AsBatPlusSq;
long AsBatMinusSq;
long JumpValue = 0;
int Period = 1;
boolean FlipDia;
int SendToSerialCount = 0;
boolean Nix;

void setup() {
//for daignosis: setup communication line with computer
Serial.begin(115200);
Serial.println("start setup");
// when comport available, then send Flash memory over comport and then reinitialise pointer

//get value from Flash to count down from fully charged for Bat+ & Bat-

// initialise PWM's, ADC's and digital I/O's
pinMode(OUT_GreenLedBatPlus, OUTPUT);
pinMode(OUT_YellowLedBatPlus, OUTPUT);
pinMode(OUT_RedLedBatPlus, OUTPUT);
pinMode(OUT_GreenLedBatMinus, OUTPUT);
pinMode(OUT_YellowLedBatMinus, OUTPUT);
pinMode(OUT_RedLedBatMinus, OUTPUT);
pinMode(OUT_Dia1, OUTPUT);
pinMode(OUT_Dia2, OUTPUT);

//pinMode(PWM_BlueLedBatPlus, OUTPUT);
//pinMode(PWM_AudioBatPlus, OUTPUT);

//pinMode(PWM_BlueLedBatMinus, OUTPUT);
//pinMode(PWM_AudioBatMinus, OUTPUT);


Serial.print(BatPlusAvailableCharge);
Serial.print("\t");
Serial.println(BatMinusAvailableCharge);
Serial.println(analogRead(ADC_AsBatPlus));
Serial.println(analogRead(ADC_AsBatMinus));
Serial.println(JumpValue);
}

void loop() {
//diagnostics:
Serial.print(SendToSerialCount);
Serial.print("\t");
Serial.print(JumpValue);
Serial.print("\t");
Serial.print(Period);
Serial.print("\t");
Serial.print(AsBatPlus);
Serial.print("\t");
Serial.println(AsBatMinus);
//delay used to generate sound according to current and measure time for running code
digitalWrite(OUT_Dia2,HIGH);
while (JumpValue < 5000) //should continue to run for 0,5 seconds minus the time for executing the rest of the code. To be measured by oscilloscope.
{
if (FlipDia == true) {FlipDia = false;}
else {FlipDia = true;}
digitalWrite(OUT_Dia1, FlipDia);
JumpValue++;
JumpValue += Period;
digitalWrite(OUT_Dia2,HIGH);
for (int x=0;x<200+Period*100;x++)
{
Nix = true;
Nix = false;
}
digitalWrite(OUT_Dia2,LOW);
}
JumpValue = 0;
if (FlipDia == true) {FlipDia = false;}
else {FlipDia = true;}
digitalWrite(OUT_Dia1, FlipDia);

//read and proces ADC value with A.s from Bat+ & Bat- and substract from total available charge
AsBatPlus = analogRead(ADC_AsBatPlus);
if (AsBatPlus < 200)
{AsBatPlusSq = AsBatPlus;}
else if (AsBatPlus < 500)
{AsBatPlusSq = AsBatPlus * 3;}
else {AsBatPlusSq = AsBatPlus * 10;}

BatPlusAvailableCharge -= AsBatPlusSq;
BatPlusAvailableCharge--;

AsBatMinus = analogRead(ADC_AsBatMinus);
if (AsBatMinus < 200)
{AsBatMinusSq = AsBatMinus;}
else if (AsBatMinus < 500)
{AsBatMinusSq = AsBatMinus * 3;}
else {AsBatMinusSq = AsBatMinus * 10;}
BatMinusAvailableCharge -= AsBatMinusSq;
BatMinusAvailableCharge--;

//use available charge to set green/Yellow/Red LED's for Bat+ & Bat-
if (BatPlusAvailableCharge < AsBat60)
{
digitalWrite(OUT_GreenLedBatPlus,LOW);
digitalWrite(OUT_YellowLedBatPlus,LOW);
digitalWrite(OUT_RedLedBatPlus,HIGH);
}
else if (BatPlusAvailableCharge < AsBat70)
{
digitalWrite(OUT_GreenLedBatPlus,LOW);
digitalWrite(OUT_YellowLedBatPlus,HIGH);
digitalWrite(OUT_RedLedBatPlus,HIGH);
}
else if (BatPlusAvailableCharge < AsBat80)
{
digitalWrite(OUT_GreenLedBatPlus,LOW);
digitalWrite(OUT_YellowLedBatPlus,HIGH);
digitalWrite(OUT_RedLedBatPlus,LOW);
}
else if (BatPlusAvailableCharge < AsBat90)
{
digitalWrite(OUT_GreenLedBatPlus,HIGH);
digitalWrite(OUT_YellowLedBatPlus,HIGH);
digitalWrite(OUT_RedLedBatPlus,LOW);
}
else
{
digitalWrite(OUT_GreenLedBatPlus,HIGH);
digitalWrite(OUT_YellowLedBatPlus,LOW);
digitalWrite(OUT_RedLedBatPlus,LOW);
}

if (BatMinusAvailableCharge < AsBat60)
{
digitalWrite(OUT_GreenLedBatMinus,LOW);
digitalWrite(OUT_YellowLedBatMinus,LOW);
digitalWrite(OUT_RedLedBatMinus,HIGH);
}
else if (BatMinusAvailableCharge < AsBat70)
{
digitalWrite(OUT_GreenLedBatMinus,LOW);
digitalWrite(OUT_YellowLedBatMinus,HIGH);
digitalWrite(OUT_RedLedBatMinus,HIGH);
}
else if (BatMinusAvailableCharge < AsBat80)
{
digitalWrite(OUT_GreenLedBatMinus,LOW);
digitalWrite(OUT_YellowLedBatMinus,HIGH);
digitalWrite(OUT_RedLedBatMinus,LOW);
}
else if (BatMinusAvailableCharge < AsBat90)
{
digitalWrite(OUT_GreenLedBatMinus,HIGH);
digitalWrite(OUT_YellowLedBatMinus,HIGH);
digitalWrite(OUT_RedLedBatMinus,LOW);
}
else
{
digitalWrite(OUT_GreenLedBatMinus,HIGH);
digitalWrite(OUT_YellowLedBatMinus,LOW);
digitalWrite(OUT_RedLedBatMinus,LOW);
}
//use current A.s to drive Blue LED from Bat+ & Bat-
//analogWrite(PWM_BlueLedBatPlus,AsBatPlus/4);

//analogWrite(PWM_BlueLedBatMinus,AsBatMinus/4);

//use current A.s to drive audio output from Bat+ & Bat-
//analogWrite(PWM_AudioBatPlus,128+AsBatPlus/8);

//analogWrite(PWM_AudioBatMinus,128+AsBatMinus/8);

//every 5 minutes store total available charge from Bat- & Bat+ in Flash memory

//delay(SampleRate); //to optimise the time to run code above is to be substracted.. is probably just in order of ms?
//DIAGNOSTICS
// audio signal for time to run through code
if (AsBatPlus > AsBatMinus)
{Period = AsBatPlus;}
else
{Period = AsBatMinus;}
//used in delay @ beginning of loop; it's the period of the sound
// printouts to serial of computer(introducing major delay though?)

SendToSerialCount++;
if (SendToSerialCount == 600)
{
SendToSerialCount = 0;
//Serial.print("BatPlusAvailableCharge");
//Serial.print("\t");
//Serial.println("BatMinusAvailableCharge");
Serial.print(BatPlusAvailableCharge);
Serial.print("\t");
Serial.println(BatMinusAvailableCharge);
}
//delay(400);
}
