// assign LEDs and button to pins
int ledPins[] = {4, 5, 6, 8, 9, 10};
int playerOneButton = 2;
int playerTwoButton = 3;
int whiteLED[] = {7, 11};

// declare variables
int delayTime = 1000; // time delay between lights on/off
int randNumber;
unsigned long lastButtonPress;
int score[] = {0, 0};
int winScore = 3;
int totalWins = 0;
int winningSequence = 0;
int win = 0;

//setup interrupt, button input and LED outputs
void setup() {
	// specify interrupt routine
	attachInterrupt(digitalPinToInterrupt(playerOneButton), buttonPressed, FALLING);
	attachInterrupt(digitalPinToInterrupt(playerTwoButton), buttonPressed, FALLING); 

	for (int i=0; i<6; i++){
	pinMode(ledPins[i], OUTPUT);
	}

	pinMode(playerOneButton, INPUT_PULLUP);
	pinMode(playerTwoButton, INPUT_PULLUP);
	pinMode(whiteLED[0], OUTPUT);
	pinMode(whiteLED[1], OUTPUT);

	Serial.begin(9600);
}

//run main program loop
void loop() {
	randNumber = random(3); // select a random number

	digitalWrite(ledPins[randNumber], HIGH); // light the LED with this number
	digitalWrite(ledPins[randNumber + 3], HIGH);
	delay(delayTime);

	digitalWrite(ledPins[randNumber], LOW);
	digitalWrite(ledPins[randNumber + 3], LOW);
	delay(delayTime);

	//if whiteLED on = turn it off
	digitalWrite(whiteLED[0], LOW);
	digitalWrite(whiteLED[1], LOW); 

	if (score[0] == winScore || score[1] == winScore){
		winningSequence = 1;
		// DO NOT REMOVE
		Serial.print("");
		//delay(100);
		flashLEDS(score[1] == winScore);
		winningSequence = 0;
		score[0] = 0;
		score[1] = 0;
		totalWins++;
	}
}

void buttonPressed() {
	Serial.print("winningSequence: ");
	Serial.println(winningSequence);
	if (winningSequence == 0){
		if (!digitalRead(playerOneButton)){
			playerInput(0);
		}
		else playerInput(1);
	}
}


void playerInput(int button) {
	if (millis() - lastButtonPress > 300){
		bool ledsOn = false;
		for (int i = 0; i < 3; i++){
			if (digitalRead(ledPins[i + (3 * button)])){
				ledsOn = true;
			}
		}
		
		if (ledsOn){
			// Win point
			score[button]++;
			digitalWrite(whiteLED[0 + button], HIGH);
			digitalWrite(ledPins[randNumber], LOW);
			digitalWrite(ledPins[randNumber + 3], LOW);
		}
		else{
			// Lost point
			if (score[button] > 0){
				score[button]--;
			}
		}
	
		Serial.println(score[0]);
		Serial.println(score[1]);
		lastButtonPress = millis();
	} 
}

void flashLEDS(int player){
	for (int i = 0; i < 20; i++){
		for (int j = 0; j < 3; j++){
			digitalWrite(ledPins[j + (3 * player)], HIGH);		
		}
		digitalWrite(whiteLED[0 + player], HIGH);
		delay(150);
		for (int j = 0; j < 6; j++){
			digitalWrite(ledPins[j], LOW);
		}
		digitalWrite(whiteLED[0 + player], LOW);
		delay(150);
	}
}
