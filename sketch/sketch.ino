#include <Servo.h>
#include <LiquidCrystal.h>

// Assign components to pins
int ledPins[] = {A5, A4, A3, 0, 1, 4};
int playerOneButton = 2;
int playerTwoButton = 3;
int winLEDPins[] = {A2, 5};
int servoPin = 6;
int buzzerPin = A0;
int difficultyPin = A1;

// Declare variables
int randNumber;
unsigned long lastButtonPress;
int scores[] = {0, 0};
int winScore = 10;
int totalWins[] = {0, 0};
int winningSequence = 0;
int win = 0;
Servo servo;
LiquidCrystal lcd(8, 9, 12, 10, 11, 13);

// Setup interrupt, button input, LED outputs, servo, buzzer and lcd
void setup() {
	// Specify interrupt routine
	attachInterrupt(digitalPinToInterrupt(playerOneButton), buttonPressed, FALLING);
	attachInterrupt(digitalPinToInterrupt(playerTwoButton), buttonPressed, FALLING); 

	// Setup LED ouputs
	for (int i = 0; i < 6; i++){
		pinMode(ledPins[i], OUTPUT);
	}

	pinMode(winLEDPins[0], OUTPUT);
	pinMode(winLEDPins[1], OUTPUT);

	// Setup buttons with pullup resistors
	pinMode(playerOneButton, INPUT_PULLUP);
	pinMode(playerTwoButton, INPUT_PULLUP);

	// Setup the servo
	servo.attach(servoPin);

	// Setup the buzzer output
	pinMode(buzzerPin, OUTPUT);

	// Setup the analogue input for controlling the difficulty of the game
	pinMode(difficultyPin, INPUT);

	// Setup lcd display and show initial points
	lcd.begin(16, 2);
	lcd.print("Points: R:0 G:0");
}

// Run main program loop
void loop() {
	// The noTone methods used here turn of the buzzer noise
	noTone(buzzerPin);

	// Show the winning LED for longer if a point has been won
	delay(500 * (winLEDPins[0] || winLEDPins[0]));

	noTone(buzzerPin);

	// If winLEDPins on, turn them off
	digitalWrite(winLEDPins[0], LOW);
	digitalWrite(winLEDPins[1], LOW);

	// Get the difficulty from the potentiometer
	int difficulty = getDifficulty();

	// Delay a random length of time with no LEDs on, of which the minimum is determined by the difficulty
	delay(random(1200 - difficulty, 5000));
	noTone(buzzerPin);

	// Select a random number for the led
	randNumber = random(3); 

	// Light the LED with the random number index
	digitalWrite(ledPins[randNumber], HIGH); 
	digitalWrite(ledPins[randNumber + 3], HIGH);

	// Delay a random length of time with a set of LEDs on, of which the minimum is determined by the difficulty
	delay(random(450 - (difficulty / 3), 1500));

	// Turn the leds off again
	digitalWrite(ledPins[randNumber], LOW);
	digitalWrite(ledPins[randNumber + 3], LOW);

	// Check if either player has reached the wininng score
	if (scores[0] == winScore || scores[1] == winScore){
		winSequence();
	}
}

// The interrupt handler routine which is called when a button is pressed
void buttonPressed() {
	// Check the game is not in its winning sequence and get the input of the player which pressed the button
	if (winningSequence == 0){
		if (!digitalRead(playerOneButton)){
			playerInput(0);
		}
		else playerInput(1);
	}
}


void playerInput(int button) {
	// Ensure the last button press was greater than 100ms ago to stop the buttons registering multiple false presses
	if (millis() - lastButtonPress > 100){
		lastButtonPress = millis();

		// Determine if any of the LEDs are on
		bool ledsOn = false;
		for (int i = 0; i < 3; i++){
			if (digitalRead(ledPins[i + (3 * button)])){
				ledsOn = true;
			}
		}
		
		if (ledsOn){
			// Win point
			scores[button]++;

			// Play noise
			tone(buzzerPin, 800);

			// Show on display
			lcd.setCursor(0, 1);
			lcd.print("                     ");
			lcd.setCursor(0, 1);
			if (button == 0){
				lcd.print("Red wins point");
			}
			else{
				lcd.print("Green wins point");
			}	

			// Move servo to show who is in the lead (and by how much)
			int angle = ((scores[0] - scores[1]) * 90 / winScore) + 90;
			servo.write(angle);

			// Turn on win LED indicator and turn off the other LEDs
			digitalWrite(winLEDPins[0 + button], HIGH);
			digitalWrite(ledPins[randNumber], LOW);
			digitalWrite(ledPins[randNumber + 3], LOW);
		}
		else{
			// Lost point (however points cannot go below zero)
			if (scores[button] > 0){
				scores[button]--;
			}

			// Play noies and show on lcd display
			tone(buzzerPin, 200);
			lcd.setCursor(0, 1);
			lcd.print("                     ");
			lcd.setCursor(0, 1);
			if (button == 0){
				lcd.print("Red lost point");
			}
			else{
				lcd.print("Green lost point");
			}
		}

		// Display current scores on the lcd display
		lcd.setCursor(10, 0);
		lcd.print(scores[0]);
		lcd.setCursor(14, 0);
		lcd.print(scores[1]);
	} 
}

// Plays the wining sequence when a player wins a game
void winSequence(){
	winningSequence = 1;
		lcd.clear();
		if (scores[1] == winScore){
			lcd.print("Green wins");
			totalWins[1]++;		
		}
		else{
			lcd.print("Red wins");
			totalWins[0]++;	
		}
		lcd.setCursor(0, 1);
		lcd.print("the game!");

		// DO NOT REMOVE (Somehow makes the program work)
		Serial.print("");
		flashLEDS(scores[1] == winScore);
		winningSequence = 0;
		scores[0] = 0;
		scores[1] = 0;	

		lcd.clear();
		lcd.print("Total game wins");
		lcd.setCursor(0, 1);
		lcd.print("Red:");
		lcd.print(totalWins[0]);
		lcd.print(" Green:");
		lcd.print(totalWins[1]);
		delay(8000);
		lcd.clear();
		lcd.print("Points: R:0 G:0");

		servo.write(90);
}

// Flashes the LEDs of a player's colour and also plays a tune
void flashLEDS(int player){
	for (int i = 0; i < 20; i++){
		for (int j = 0; j < 3; j++){
			digitalWrite(ledPins[j + (3 * player)], HIGH);		
		}

		digitalWrite(winLEDPins[0 + player], HIGH);
		tone(buzzerPin, 600);
		
		delay(150);

		for (int j = 0; j < 6; j++){
			digitalWrite(ledPins[j], LOW);
		}

		digitalWrite(winLEDPins[0 + player], LOW);
		noTone(buzzerPin);
		delay(150);
	}
}

// Reads the difficulty from an input and returns that difficulty (from 0 to 1023) and displays this as a value between 0 and 10
int getDifficulty(){
	int difficulty = analogRead(difficultyPin);

	lcd.setCursor(0, 1);
	lcd.print("                     ");
	lcd.setCursor(0, 1);
	lcd.print("Difficulty:");
	lcd.print(difficulty / 102);

	return difficulty;
}