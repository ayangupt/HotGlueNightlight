// Define the pins for the RGB LED, pencil LED, and photoresistor
#define RED_PIN 3
#define GREEN_PIN 5
#define BLUE_PIN 6
#define PENCIL_PIN A1
#define PHOTO_PIN A0
#define BUTTON_PIN 2 // Assuming the button is connected to port 2
#define POTENTIOMETER_PIN A2 // Potentiometer pin

// Define the maximum and minimum brightness values
#define MAX_BRIGHTNESS 254
#define MIN_BRIGHTNESS 0

// Define the threshold values for color transitions
#define GREEN_THRESHOLD 500
#define BLUE_THRESHOLD 1023

// Variable to store the last RGB values
int lastRedValue = MIN_BRIGHTNESS;
int lastGreenValue = MAX_BRIGHTNESS;
int lastBlueValue = MIN_BRIGHTNESS;

// Variable to track the current mode (0 for pencil, 1 for potentiometer)
int mode = 0;

void setup() {
  // Initialize the RGB LED, photoresistor, and button pins
  pinMode(RED_PIN, OUTPUT);
  pinMode(GREEN_PIN, OUTPUT);
  pinMode(BLUE_PIN, OUTPUT);
  pinMode(PHOTO_PIN, INPUT);
  pinMode(POTENTIOMETER_PIN, INPUT);
  pinMode(BUTTON_PIN, INPUT_PULLUP); // Internal pull-up resistor enabled

  // Initialize Serial communication
  Serial.begin(9600);
}

void loop() {
  // Check if the button is pressed
  if (digitalRead(BUTTON_PIN) == LOW) { // LOW when the button is pressed
    delay(50); // Debounce delay
    // Toggle mode between 0 and 1
    mode = 1 - mode; // Toggle between 0 and 1

    // Wait until the button is released
    while (digitalRead(BUTTON_PIN) == LOW) {
      delay(50); // Debounce delay
    }
  }

  if (mode == 1) {
    // Potentiometer mode
    int potValue = analogRead(POTENTIOMETER_PIN);
    int staticColor = map(potValue, 0, 1023, 0, 255);
    analogWrite(RED_PIN, staticColor);
    //analogWrite(GREEN_PIN, staticColor);
    analogWrite(BLUE_PIN, 254);
  } else {
    // Pencil mode
    // Read the values from the pencil LED and photoresistor
    int pencilValue = analogRead(PENCIL_PIN);
    int photoValue = analogRead(PHOTO_PIN);

    // Print the values to the serial monitor
    Serial.print("Pencil Value: ");
    Serial.println(pencilValue);
    Serial.print("Photoresistor Value: ");
    Serial.println(photoValue);

    // Map the pencil value to the RGB LED color range
    int redValue, greenValue, blueValue;

    if (pencilValue < 10) {
      // Use the last saved RGB values
      redValue = lastRedValue;
      greenValue = lastGreenValue;
      blueValue = lastBlueValue;
    } else if (pencilValue <= 200) {
      // Set pure red
      redValue = MIN_BRIGHTNESS;
      greenValue = MAX_BRIGHTNESS;
      blueValue = MIN_BRIGHTNESS;
    } else if (pencilValue <= GREEN_THRESHOLD) {
      // Map the pencil value to the red-green transition
      redValue = map(pencilValue, 10, GREEN_THRESHOLD, MAX_BRIGHTNESS, MIN_BRIGHTNESS);
      greenValue = map(pencilValue, 10, GREEN_THRESHOLD, MIN_BRIGHTNESS, MAX_BRIGHTNESS);
      blueValue = MIN_BRIGHTNESS;  // No blue initially
    } else {
      // Map the pencil value to the green-blue transition
      redValue = MIN_BRIGHTNESS;  // No red beyond the green threshold
      greenValue = map(pencilValue, GREEN_THRESHOLD, BLUE_THRESHOLD, MAX_BRIGHTNESS, MIN_BRIGHTNESS);
      blueValue = map(pencilValue, GREEN_THRESHOLD, BLUE_THRESHOLD, MIN_BRIGHTNESS, MAX_BRIGHTNESS);
    }

    // Save the current RGB values
    lastRedValue = redValue;
    lastGreenValue = greenValue;
    lastBlueValue = blueValue;

    // Adjust brightness based on photoresistor
    int invertedBrightness = map(photoValue, 0, 1023, MAX_BRIGHTNESS, MIN_BRIGHTNESS);
    redValue = (redValue * invertedBrightness) / MAX_BRIGHTNESS;
    greenValue = (greenValue * invertedBrightness) / MAX_BRIGHTNESS;
    blueValue = (blueValue * invertedBrightness) / MAX_BRIGHTNESS;

    // Ensure minimum brightness
    redValue = max(redValue, 0);
    greenValue = max(greenValue, 0);
    blueValue = max(blueValue, 0);

    // Adjust the color of the RGB LED based on the values
    analogWrite(RED_PIN, redValue);
    analogWrite(GREEN_PIN, greenValue);
    analogWrite(BLUE_PIN, blueValue);
  }

  // Delay for stability
  delay(100);
}
