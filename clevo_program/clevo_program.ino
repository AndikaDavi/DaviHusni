#define trigPin 6
#define echoPin 7
long duration;

int ledPin13 = 13; 
int ledPin3_Light = 3; 
int pin4_Taser = 4;

int motor_Left1 = 5; 
int motor_Left2 = 9; 

int motor_Right1 = 10; 
int motor_Right2 = 11; 

int carSpeed = 100;  
int lightBrightness = 100;  

int photoresistor_Right = 14; //A0
int photoresistor_Center = 15; //A1
int photoresistor_Left = 16; //A2


char inputbuffer[20];
String valSerial = "";

boolean flashing = false;

void setup() {
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);
  
  pinMode(ledPin13, OUTPUT);
  pinMode(ledPin3_Light, OUTPUT);
  pinMode(pin4_Taser, OUTPUT);
  pinMode(motor_Left1, OUTPUT);
  pinMode(motor_Left2, OUTPUT);
  pinMode(motor_Right1, OUTPUT);
  pinMode(motor_Right2, OUTPUT);
  Serial.begin(9600); 
  
  getDistance(); //helps to avoid error in the first reading (dont know why)
}

void loop() {

  if(Serial.available()){
    Serial.readBytesUntil('\n', inputbuffer, 10);
    valSerial = inputbuffer;
    /*
     * Clean Buffer+++++++++++
     */
    for(int x=0; x<sizeof(inputbuffer); x++){
      inputbuffer[x] = (char)0;
    }
  }
  

  if ( valSerial == "Foward" ) {
    MouveFoward();
  }
  if ( valSerial == "Backward" ) {
    MouveBackward();
  }
  if ( valSerial == "Left" ) {
    turnLeft();
  }
  if ( valSerial == "Right" ) {
    turnRight();
  }
  if ( valSerial == "Stop" ) {
    stopAll();
  }
  
  /*
   *Taser
   */
  if ( valSerial == "TOn" ) {
    valSerial = "";
    digitalWrite(pin4_Taser, HIGH);
  }
  if ( valSerial == "TOff" ) {
    valSerial = "";
    digitalWrite(pin4_Taser, LOW);
  }
  
  /*
   *Autonomous
   */
   if ( valSerial == "Autonomous" ) {
     valSerial = "";
     autonomous();
   }
  
  /*
   * Light Related
   */
  if( valSerial.startsWith("Bright") ){
    Serial.println(valSerial);
    lightBrightness = valSerial.substring(6, 9).toInt();
    //Serial.println(lightBrightness);
    analogWrite(ledPin3_Light, lightBrightness);
  }
  if ( valSerial == "LightOn" ) {
    analogWrite(ledPin3_Light, lightBrightness);
  }
  if ( valSerial == "LightOff" ) {
    digitalWrite(ledPin3_Light, LOW);
  }
  if ( valSerial == "Flashing" ) {
    flashing = !flashing;
  }
  if(flashing){
    //digitalWrite(ledPin3_Light, HIGH);
    analogWrite(ledPin3_Light, lightBrightness);
    delay(30);
    digitalWrite(ledPin3_Light, LOW);
    delay(30);
  }

  /*
   * Car Speed
   */
  if ( valSerial.startsWith("Speed") ) {
    carSpeed = valSerial.substring(5, 8).toInt();
    //Serial.println(carSpeed);
  }

  /*
   * Light Following
   */
  if (valSerial == "LightF"){
    valSerial = "";
    lightFollowing();
  }

  valSerial = "";
}

//---------------------------------------------------
//------------------- Turn Left + -------------------
void turnLeft(){
  analogWrite(motor_Left1 , carSpeed);
  analogWrite(motor_Left2 , 0);
  
  analogWrite(motor_Right1 , 0);
  analogWrite(motor_Right2 , carSpeed);
  
  digitalWrite(ledPin13, LOW);
}
//------------------- Turn Left - -------------------
//------------------- Turn Right + ------------------
void turnRight(){

  analogWrite(motor_Left1 , 0);
  analogWrite(motor_Left2 , carSpeed);

  analogWrite(motor_Right1 , carSpeed);
  analogWrite(motor_Right2 , 0);
  
  digitalWrite(ledPin13, LOW);
}
//------------------- Turn Right - ------------------
//---------------------------------------------------
//----------------- Mouve Foward + ------------------
void MouveFoward(){

  analogWrite(motor_Left1 , 0);
  analogWrite(motor_Left2 , carSpeed);

  analogWrite(motor_Right1 , 0);
  analogWrite(motor_Right2 , carSpeed);

  digitalWrite(ledPin13, LOW);

}
//----------------- Mouve Foward - ------------------
//---------------- Mouve Backward + -----------------
void MouveBackward(){

  analogWrite(motor_Left1 , carSpeed);
  analogWrite(motor_Left2 , 0);

  analogWrite(motor_Right1 , carSpeed);
  analogWrite(motor_Right2 , 0);

  digitalWrite(ledPin13, HIGH);

}
//---------------- Mouve Backward - -----------------
//---------------------------------------------------
//-------------------- stopAll + -----------------------
void stopAll(){
  analogWrite(motor_Left1 , 0);
  analogWrite(motor_Left2 , 0);
  analogWrite(motor_Right1 , 0);
  analogWrite(motor_Right2 , 0);
}
//-------------------- stopAll - -----------------------
//------------------------------------------------------
//-------------------- + lightFollowing + -----------------------
void lightFollowing(){
  /*
   * Update offset based on the ambient light
   */
  int sensorInLight = analogRead(photoresistor_Center); 
  int offsetLight = sensorInLight - 80;
  
  while(1){
    /*
     * Monitor serial to exit lightFollowing
     */
    if(Serial.available()){
      Serial.readBytesUntil('\n', inputbuffer, 10);
      valSerial = inputbuffer;
      /*
       * Clean Buffer
       */
      for(int x=0; x<sizeof(inputbuffer); x++){
        inputbuffer[x] = (char)0;
      }
    }
    if (valSerial == "LightF"){
      stopAll();
      valSerial = "";
      break;
    }

    int val_Right = analogRead(photoresistor_Right); 
    int val_Center = analogRead(photoresistor_Center); 
    int val_Left = analogRead(photoresistor_Left); 

    if(val_Center < offsetLight ){
      MouveFoward();
    }else if(val_Right < offsetLight){
      turnRight();
    }else if(val_Left < offsetLight){
      turnLeft();
    }else if(val_Center < offsetLight && val_Right < offsetLight){
      turnRight();
    }else if(val_Center < offsetLight && val_Left < offsetLight){
      turnLeft();
    }else{
      stopAll();
    }
  }
}
//-------------------- lightFollowing - -----------------------
//-------------------------------------------------------------


void autonomous(){
  while(1){
    /*
     * Monitor serial to exit Autonomous
     */
    if(Serial.available()){
      Serial.readBytesUntil('\n', inputbuffer, 10);
      valSerial = inputbuffer;
      /*
       * Clean Buffer
       */
      for(int x=0; x<sizeof(inputbuffer); x++){
        inputbuffer[x] = (char)0;
      }
    }
    if (valSerial == "Autonomous"){
      stopAll();
      valSerial = "";
      break;
    }
   
    int distance = getDistance();
    Serial.println(distance);
    MouveFoward();
  
    /*
     *Regulate speed depending on the distance
     */
    if(distance <= 100){
      carSpeed = 60;  
    }else if(distance <= 50){
       carSpeed = 40;
    }else{
      carSpeed = 100;
    }
    
    if(distance <= 30){
      stopAll();
      turnRight();
      delay(500);
      stopAll();
      delay(500);
      int distanceRight = getDistance();
      turnLeft();
      delay(1000);
      stopAll();
      delay(500);
      int distanceLeft = getDistance();
      
      if(distanceLeft > distanceRight){
        
      }else{
        turnRight();
        delay(1000);
      }
    }
    
  }
}

//-------------------- demo + -----------------------
void demo(){
  MouveFoward();
  delay(1500);
  stopAll();
  delay(500);
  
  turnRight();
  delay(500);
  stopAll();
  delay(500);
  
  MouveFoward();
  delay(1500);
  stopAll();
  delay(500);
  
  MouveBackward();
  delay(1500);
  stopAll();
  delay(500);
  
  turnLeft();
  delay(500);
  stopAll();
  delay(500);
  
  MouveBackward();
  delay(1500);
  stopAll();
  delay(500); 
}

/*
* Distance
*/
int getDistance(){
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);
  duration = pulseIn(echoPin, HIGH);
  
  int distance = (duration/2) * 0.03495 ;//speed of sound at 30°C
  
  /*
  * Provides distances From 2cm to 400 cm
  * if reding is < 2 we call again teh fuction to correct reading
  */
  if (distance < 2){
    getDistance();
  }
  return distance;
}
//-------------------- demo - -----------------------
//---------------------------------------------------
