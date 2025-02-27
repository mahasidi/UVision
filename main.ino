#include <Adafruit_NeoPixel.h>

#define PIN_NEO_PIXEL  8  
#define NUM_PIXELS     38  
#define DELAY_INTERVAL 2000
#define BUTTON_PIN     2
#define speakerPin     10

Adafruit_NeoPixel NeoPixel(NUM_PIXELS, PIN_NEO_PIXEL, NEO_GRB + NEO_KHZ800);

// monthly averages (for demonstration purposes)
int yearUV[] = {1, 3, 4, 6, 8, 9, 9, 8, 6, 4, 2, 1};

int index = 0;
int mode = 0; // 0 = tanning, 1 = UV sensitive
bool lastButtonState = HIGH; // stores last button state

void setup() {
  Serial.begin(9600);
  NeoPixel.begin();
  NeoPixel.clear();
  NeoPixel.show();
  
  pinMode(BUTTON_PIN, INPUT_PULLUP);
  pinMode(speakerPin, OUTPUT);
}

void loop() {
  int uvLevel = yearUV[index];
  Serial.print("UV Level: ");
  Serial.println(uvLevel);

  // check button press (toggle mode when pressed)
  bool buttonState = digitalRead(BUTTON_PIN);

  if (buttonState == LOW && lastButtonState == HIGH) { // button press detected
    Serial.println("button pressed");
    Serial.println("starting sound");
    if (mode == 1) {
      play('C', 1);
      play('E', 1);
      play('G', 1);
    } else {
      play('G', 1);
      play('E', 1);
      play('C', 1);
    }
    mode = !mode; // toggle mode (0 → 1 or 1 → 0)
    index = 0;
    Serial.print("Mode Changed: ");
    Serial.println(mode == 0 ? "Tanning" : "UV Sensitive");
    delay(300);
  }

  lastButtonState = buttonState; // update last button state

  // convert UV level to brightness
  int brightness = map(uvLevel, 0, 9, 0, 255);
  brightness = constrain(brightness, 0, 255);

  // mode change
  if (mode == 0) { 
    setColor(255, 160, 0, brightness);
  } else { 
    if (uvLevel > 6) {
      setColor(255, 0, 0, 255); // red
    } else {
      setColor(255, 160, 0, brightness); // otherwise: yellow
    }
  }

  index = (index + 1) % 12;  // move to next test value
  delay(DELAY_INTERVAL);
}

// function to set color
void setColor(int r, int g, int b, int brightness) {
  for (int i = 0; i < NUM_PIXELS; i++) {
    NeoPixel.setPixelColor(i, NeoPixel.Color(
      (r * brightness) / 255,
      (g * brightness) / 255,
      (b * brightness) / 255
    ));
  }
  NeoPixel.show();
}

void play( char note, int beats)
{
  int numNotes = 14;  // number of notes in our note and frequency array (there are 15 values, but arrays start at 0)

  //Note: these notes are C major (there are no sharps or flats)

  //this array is used to look up the notes
  char notes[] = { 'c', 'd', 'e', 'f', 'g', 'a', 'b', 'C', 'D', 'E', 'F', 'G', 'A', 'B', ' '};
  //this array matches frequencies with each letter (e.g. the 4th note is 'f', the 4th frequency is 175)
  int frequencies[] = {131, 147, 165, 175, 196, 220, 247, 262, 294, 330, 349, 392, 440, 494, 0};

  int currentFrequency = 0;    //the frequency that we find when we look up a frequency in the arrays
  int beatLength = 150;   //the length of one beat (changing this will speed up or slow down the tempo of the song)

  //look up the frequency that corresponds to the note
  for (int i = 0; i < numNotes; i++)  // check each value in notes from 0 to 14
  {
    if (notes[i] == note)             // does the letter passed to the play function match the letter in the array?
    {
      currentFrequency = frequencies[i];   // Yes! Set the current frequency to match that note
    }
  }

  //play the frequency that matched our letter for the number of beats passed to the play function
  tone(speakerPin, currentFrequency, beats * beatLength);
  delay(beats * beatLength);  //wait for the length of the tone so that it has time to play
  delay(50);                  //a little delay between the notes makes the song sound more natural
  noTone(speakerPin);
}
