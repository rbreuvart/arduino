/**
* Try to make led Strip folow the music
* worck but not great to visualise
*/

#include "FastLED.h"
#include "arduinoFFT.h"

//The pin that we read sensor values form
#define ANALOG_READ 0

 //FFT audio
#define SAMPLES 8             //Must be a power of 2
#define SAMPLING_FREQUENCY 10000 //Hz, must be less than 10000 due to ADC

#if FASTLED_VERSION < 3001000
#error "Requires FastLED 3.1 or later; check github for latest code."
#endif
 
// Fixed definitions cannot change on the fly.
#define LED_DT 6                                               // Clock pin for WS2801 or APA102.
#define COLOR_ORDER RGB                                       // It's GRB for WS2812 and BGR for APA102.
#define LED_TYPE WS2812                                       // Using APA102, WS2812, WS2801. Don't forget to modify LEDS.addLeds to suit.
#define NUM_LEDS 144                                           // Number of LED's.



// Global variables can be changed on the fly.
uint8_t max_bright = 128;                                      // Overall brightness definition. It can be changed on the fly.

struct CRGB leds[NUM_LEDS];                                   // Initialize our LED array.

// Colours defined for below
long firstval = 0xff00ff;
CRGB rgbval(50,0,500);
CHSV hsvval(100,255,200);
uint8_t startpos = 0;
int endpos = NUM_LEDS-1;

uint8_t thishue = 0;
uint8_t deltahue = 1;

int LEDBYSAMPLE = (int)NUM_LEDS/SAMPLES;

arduinoFFT FFT = arduinoFFT();
unsigned int sampling_period_us;
unsigned long microseconds;
 
double vReal[SAMPLES];
double vImag[SAMPLES];
double valPeak = 0;
double realmax = 0;

void setup() {

  Serial.begin(57600);                                         // Initialize serial port for debugging.
  delay(1000);                                                 // Soft startup to ease the flow of electrons.

  LEDS.addLeds<LED_TYPE, LED_DT, COLOR_ORDER>(leds, NUM_LEDS);  // Use this for WS2812
  set_max_power_in_volts_and_milliamps(5, 500);               // This is used by the power management functionality and is currently set at 5V, 500mA.
  FastLED.setBrightness(max_bright);
 
    for (int j = 0; j < NUM_LEDS; j++) { 
         leds[j] =CHSV(50, 255, 255);
        
    }
} // setup()



void loop () { 
    delay(20);
  thishue++;
  Serial.println((String)thishue+" " );
  //if(thishue%10==0){
   //   Serial.println((String)thishue+" !!!!" );
      /*SAMPLING*/
      for(int i=0; i<SAMPLES; i++){
          microseconds = micros();    //Overflows after around 70 minutes!
          vReal[i] = analogRead(ANALOG_READ);
          vImag[i] = 0;
          while(micros() < (microseconds + sampling_period_us)){
          }
      }  

      /*FFT*/
      FFT.Windowing(vReal, SAMPLES, FFT_WIN_TYP_HAMMING, FFT_FORWARD);
      FFT.Compute(vReal, vImag, SAMPLES, FFT_FORWARD);
      FFT.ComplexToMagnitude(vReal, vImag, SAMPLES);
      double peak = FFT.MajorPeak(vReal, SAMPLES, SAMPLING_FREQUENCY);
      valPeak = peak/SAMPLING_FREQUENCY;

       /*get Max Real*/
      for (int i = 0; i < SAMPLES; i++) {
        if(realmax<vReal[i]){
          realmax = vReal[i];
        }
      }
 // }


     
      

   double lumin = 0;
   double ratioNbLed = 0;
   int cptled = 0;
   int val_lumLed = 0;
   for (int i = 0; i < SAMPLES; i++) {
   
      //double valSm = (i * 1.0 * SAMPLING_FREQUENCY) / SAMPLES;
      ratioNbLed = (vReal[i]/realmax)*LEDBYSAMPLE;
      for (int j = 0; j < LEDBYSAMPLE; j++) { 
            val_lumLed = (int)((double)(leds[j].r+ leds[j].g+ leds[j].b));
            
            if(((j)/ratioNbLed)<=1){
               lumin = 255;// val_lumLed+1;
              /* if(lumin>255){
                lumin = 255;
               }*/
               leds[cptled] =CHSV(thishue+((double)((double)j/(double)LEDBYSAMPLE)*70)+i*25, 255, lumin);
            }else{
             /*  lumin = val_lumLed-1;               
               if(lumin<150){
                lumin = 150;
               }*/
               leds[cptled] =CHSV(thishue+((double)((double)j/(double)LEDBYSAMPLE)*70)+i*25, 255, 0);
            }
           cptled++; 
      }
   }
   
   ratioNbLed = (valPeak)*LEDBYSAMPLE;
   for (int j = 0; j < LEDBYSAMPLE; j++) { 
    val_lumLed = (int)((double)(leds[j].r+ leds[j].g+ leds[j].b));
     if((j/ratioNbLed)<=1){
         lumin = 255;
        /* lumin = val_lumLed+lumin;
         if(lumin>255){
           lumin = 255;
         }*/
          leds[j] =CHSV(thishue+40+((double)((double)j/(double)LEDBYSAMPLE)*70), 255, lumin);
      }else{
         lumin = 0;
        /*lumin = val_lumLed+lumin;
         if(lumin<0){
           lumin = 0;
        }*/
         leds[j] =CHSV(thishue+40+((double)((double)j/(double)LEDBYSAMPLE)*70), 255, lumin);
      }
     
    }

    
  FastLED.show();                         // Power managed display

} 
