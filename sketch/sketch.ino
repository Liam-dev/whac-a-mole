// assign LEDs and button to pins
int ledPin[] = {4, 5, 6};
int playerOneButton = 2;
int whiteLED = 9;

// declare variables
int delayTime = 1000; // time delay between lights on/off
int randNumber;
int whiteLEDOn;
unsigned long lastButtonPress;
int score = 0;

//setup interrupt, button input and LED outputs
void setup() {
  attachInterrupt(0, playerOneInput, FALLING); // specify interrupt routine
  for (int i=0; i<3; i++){
    pinMode(ledPin[i], OUTPUT);
  }
  pinMode(playerOneButton, INPUT);
  pinMode(whiteLED, OUTPUT);

  Serial.begin(9600);
}

//run main program loop
void loop() {
  randNumber = random(3); // select a random number
  digitalWrite(ledPin[randNumber], HIGH); // light the LED with this number
  delay(delayTime);
  digitalWrite(ledPin[randNumber], LOW);
  delay(delayTime);
  whiteLEDOn = digitalRead(whiteLED);
  if(whiteLEDOn == HIGH){
    digitalWrite(whiteLED, LOW);
  }  //if whiteLED on = turn it off
}


void playerOneInput() {
  if (millis() - lastButtonPress > 100){    
    bool ledsOn = false;
    for (int i = 0; i < 3; i++){
      if (digitalRead(ledPin[i])){
        ledsOn = true;
      }
    }
    
    if (ledsOn){
      // Win point
      score++;
      digitalWrite(whiteLED, HIGH);
      digitalWrite(ledPin[randNumber], LOW);
    }
    else{
      // Lost point
      score--;
    }
  
    Serial.println(score);
    lastButtonPress = millis();
  } 
}
