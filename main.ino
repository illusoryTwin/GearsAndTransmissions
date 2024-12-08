#include <Wire.h>
#include <AS5600.h>

#define D0 6
#define D1 5

#define C1 2
#define C2 3

int lastEncoded = 0;
long encoderValue = 0;

// Variables for velocity calculation
unsigned long lastTime = 0;
float lastOutAngle = 0;
float velocityOut = 0; // Angular velocity in degrees/second

float lastRotAngle = 0;
float velocityRot = 0; // Angular velocity in degrees/second

AMS_5600 ams5600;

void setup()
{
  Serial.begin(115200);
  Wire.begin();

  pinMode(D0, OUTPUT);
  pinMode(D1, OUTPUT);
  pinMode(C1, INPUT_PULLUP);
  pinMode(C2, INPUT_PULLUP);

  attachInterrupt(0, updateEncoder, CHANGE); 
  attachInterrupt(1, updateEncoder, CHANGE);

  set_speed(200);
}

void updateEncoder(){
  int MSB = digitalRead(C1); //MSB = most significant bit
  int LSB = digitalRead(C2); //LSB = least significant bit
 
  int encoded = (MSB << 1) |LSB; //converting the 2 pin value to single number
  int sum  = (lastEncoded << 2) | encoded; //adding it to the previous encoded value
 
  if(sum == 0b1101 || sum == 0b0100 || sum == 0b0010 || sum == 0b1011) encoderValue --;
  if(sum == 0b1110 || sum == 0b0111 || sum == 0b0001 || sum == 0b1000) encoderValue ++;
  
  lastEncoded = encoded; //store this value for next time
}

float convertOutRawAngleToDegrees(word newAngle)
{
  /* Raw data reports 0 - 4095 segments, which is 0.087890625 of a degree */
  float retVal = newAngle * 0.087890625;
  return retVal;
}


float convertRotorRawAngleToDegrees(long newAngle)
{
  /* Raw data reports 0 - 4095 segments, which is 0.087890625 of a degree */
  float retVal = newAngle * 360 / 28;
  return retVal;
}

void set_speed(int speed) {
  if (speed >= 0) {
    analogWrite(D0, speed);
    analogWrite(D1, LOW);
  } else {
    analogWrite(D1, -speed);
    analogWrite(D0, LOW);
  }
}


void loop()
{

  unsigned long currentTime = millis(); // Current time in milliseconds
  float currentOutAngle = convertOutRawAngleToDegrees(ams5600.getRawAngle()); // Get current angle
  float currentRotAngle = convertRotorRawAngleToDegrees(encoderValue);

  // Calculate velocity if enough time has passed
  if (currentTime > lastTime) {
    float deltaTime = (currentTime - lastTime); // Time in milliseconds
    float deltaOutAngle = currentOutAngle - lastOutAngle;
    float deltaRotAngle = currentRotAngle - lastRotAngle;

    // Handle angle wrapping (if the angle resets at 360 degrees)
    if (deltaOutAngle > 180) deltaOutAngle -= 360;
    if (deltaOutAngle < -180) deltaOutAngle += 360;

    velocityOut = deltaOutAngle / deltaTime; // Angular velocity in degrees/second
    velocityRot = deltaRotAngle / deltaTime; // Angular velocity in degrees/second

    // Update last values
    lastTime = currentTime;
    lastOutAngle = currentOutAngle;
    lastRotAngle = currentRotAngle;

    // Print angle and velocity
    Serial.print("Angle Rot: ");
    Serial.print(currentRotAngle);
    Serial.print(" degrees, Velocity Rot: ");
    Serial.print(velocityRot);
    Serial.println(" degrees/second");

    // Print angle and velocity
    Serial.print("Angle out: ");
    Serial.print(currentOutAngle);
    Serial.print(" degrees, Velocity out: ");
    Serial.print(velocityOut);
    Serial.println(" degrees/second");
    
    Serial.print(deltaTime);
    delay(10);
  }

}
