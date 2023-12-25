// Define the ports numbers
const byte ON = 2, START=3, EXTRA=4, FULL=5;
const byte HEATER = 8, LOCK=9, DRAIN=10, PUMP=12;
const byte MOTOR=11;
// Define the state
byte state = 0;
// Define two timers
unsigned long timer1_interval=0, timer2_interval=0;
unsigned long timer1_prev = 0, timer2_prev = 0;
unsigned long timer1_current=0, timer2_current=0;
bool timer1_On = false, timer2_On = false;
void stopButtonInterrupt() {
      Serial.println("Stop button was clicked.");
      Serial.println("Move to state 5 to drain water.");
      // Jumpt to state 5
      state = 5;
}
bool timer1(unsigned long amount) {
  if(timer1_On==false){
    timer1_On=true;
    timer1_interval = amount;
    timer1_prev = millis();
  } else{
      timer1_current = millis();
      if(timer1_current - timer1_prev > timer1_interval) {
        timer1_On = false;
        return true;
      }
  }
  return false;
}
bool timer2(unsigned long amount) {
  if(timer2_On==false){
    timer2_On=true;
    timer2_interval = amount;
    timer2_prev = millis();
  } else{
      timer2_current = millis();
      if(timer2_current - timer2_prev > timer2_interval) {
        timer2_On = false;
        return true;
      }
  }
  return false;
}
void setup() {
      Serial.begin(9600);
      // Setup the input pins
    pinMode(ON, INPUT_PULLUP);
    pinMode(START, INPUT_PULLUP);
    pinMode(EXTRA, INPUT_PULLUP);
    pinMode(FULL, INPUT_PULLUP);
      // Setup the output pins
    pinMode(HEATER, OUTPUT);
    pinMode(LOCK, OUTPUT);
    pinMode(DRAIN, OUTPUT);
    pinMode(PUMP, OUTPUT);
      pinMode(MOTOR, OUTPUT);
      // When the stop button is pressed, jump to state 5 directly
    attachInterrupt(digitalPinToInterrupt(START), stopButtonInterrupt, RISING);
}
void loop() {
  Serial.print("State ");
  Serial.println(state);
  // Define the washing machine six states
  switch(state)
  {
    // State zero is active when pin 2 (ON/OFF) is off
    // or when pin 2 is on and pin 3 (start/stop) is off
    case 0:
    {
      // Setup the output pins
      digitalWrite(HEATER, 0);
      digitalWrite(LOCK, 0);
      // Drain is off when pin 10 is 1
      digitalWrite(DRAIN, 1);
      analogWrite(MOTOR, 0);
      digitalWrite(PUMP, 0);
      // If the ON and START buttons are on, move on
      // to state one
      if(digitalRead(ON) && digitalRead(START)) state=1;
      break;
    }
    case 1:
    {
      // Setup the output pins
      digitalWrite(HEATER, 0);
      digitalWrite(LOCK, 1);
      digitalWrite(DRAIN, 1);
      analogWrite(MOTOR, 0);
      digitalWrite(PUMP, 1);
      // The conditions to move on to the next state
      if(timer1(5*60000)==true) state = 2;
      break;
    }
    case 2:
    {
      // Setup the output pins
      digitalWrite(HEATER, 1);
      digitalWrite(LOCK, 1);
      digitalWrite(DRAIN, 1);
      analogWrite(MOTOR, 0);
      digitalWrite(PUMP, 0);
      // Get the voltage reading from the TMP36
        int reading = analogRead(0);
        // Convert that reading into voltage
        float voltage = reading * (5.0 / 1024.0);
        // Convert the voltage into the temperature in Celsius
        float temperatureC = (voltage - 0.5) * 100;
      // The conditions to move on to the next state
      if (temperatureC >= 70) state = 3;
      break;
    }
    case 3:
    {
      digitalWrite(HEATER, 0);
      digitalWrite(LOCK, 1);

      digitalWrite(DRAIN, 1);
      // Turn motor on and off every 10 seconds
      if (timer1(10000) == true) digitalRead(11) == 0 ? analogWrite(MOTOR, 255) : analogWrite(MOTOR, 0);
      digitalWrite(PUMP, 0);
      if (digitalRead(EXTRA) == 1) {
        if (timer2(45*60000) == true) state = 4;
      } else {
        if (timer2(30*60000) == true) state = 4;
      }
      break; 
    }
    case 4:
    {
      digitalWrite(HEATER, 0);
      digitalWrite(LOCK, 1);
      digitalWrite(DRAIN, 1);
      analogWrite(MOTOR, 230);
      digitalWrite(PUMP, 0);
      if (timer2(15*60000) == true) state = 5;
      break;
    }
    case 5:
    {
      digitalWrite(HEATER, 0);
      digitalWrite(LOCK, 1);
      digitalWrite(DRAIN, 0);
      analogWrite(MOTOR, 0);
      digitalWrite(PUMP, 0);
      if (digitalRead(FULL) == 0) state = 0;
      break;
    } 
    default:
    break; 
  }
}