#include <Arduino.h>
#include <LiquidCrystal_I2C.h>

LiquidCrystal_I2C lcd(0x3F,16,2);  // set the LCD address to 0x3F for a 16 chars and 2 line display

#define TempSensor 13
#define QUEUE_SIZE 10

//global declaration of an array that stores 4 items
int tempQueue[QUEUE_SIZE];

//we need to variables front and rear to keep track of both the ends
int rear = -1;
int front = -1;
int count = 0;

unsigned long previousMillistemperature = 0;
const long intervaltemperature = 1000;    // Interval at which to blink (milliseconds)

// put function declarations here:
float getTemperature(int lm35Reading);
bool isEmpty();
bool isFull();
void enqueue(float value);
float dequeue();
float getAverage();
void setup() {
  Serial.begin(9600);
  lcd.init();
  lcd.clear();         
  lcd.backlight();      // Make sure backlight is on
  
  // Print a message on both lines of the LCD.
  lcd.setCursor(2,0);   //Set cursor to character 2 on line 0
  lcd.print("LM35 SENSOR");
  
  lcd.setCursor(2,1);   //Move cursor to character 2 on line 1
  lcd.print("LCD Tutorial");
}

void loop() {
  unsigned long currentMillis = millis();


  // Check if 1000ms (1 second) has passed
  if (currentMillis - previousMillistemperature >= intervaltemperature) {
    previousMillistemperature = currentMillis;               // Save the last time the LED was toggled

     // Read and convert temperature only once per interval
    int lm35Reading = analogRead(TempSensor);
    float temperature = getTemperature(lm35Reading);
    enqueue(temperature);


    Serial.println(temperature);

    // Clear only the second line where the temperature is displayed
    lcd.setCursor(0, 0);         // 1st character of line 2
    lcd.print("Temp: ");
    lcd.print(temperature);
    lcd.print((char)223);        // Degree symbol
    lcd.print("C   ");           // Extra spaces to clear leftover characters


    float avgTemp = getAverage();
    if(avgTemp != 1){
      lcd.setCursor(0, 1);         // 1st character of line 2
      lcd.print("Avg: ");
      lcd.print(avgTemp);
      lcd.print((char)223);        // Degree symbol
      lcd.print("C   ");           // Extra spaces to clear leftover characters 
    }

  }
  
}

// put function definitions here:
float getTemperature(int lm35Reading) {
  //we use the formulae to convert the analog reading into voltage 
  float voltage = lm35Reading * (5.0 / 1024.0);
  //we then convert voltage into temperature
  float temperatureReading = (voltage * 100) - 60.0;

  return temperatureReading;

}

bool isEmpty(){
  if(front == -1 && rear == -1){
    return true;
  }else{
    return false;
  }
};

bool isFull(){
  if((rear+1) % QUEUE_SIZE == front){
    return true;
  }else{
    return false;
  }
}

void enqueue(float value){
  if(isFull()){
    Serial.println("The queue is full");
    return;
  }else if(isEmpty()){
    rear = 0;
    front = 0;
  }else{
    rear = (rear + 1) % QUEUE_SIZE;
  }

  tempQueue[rear] = value;
  count++;
}

float dequeue(){
  float x = 0;
  if(isEmpty()){
    Serial.println("The queue is empty");
    return -1;
  }else if(front == rear){
    x = tempQueue[front];
    front = -1;
    rear = -1;
  }else{
    x = tempQueue[front];
    front = (front + 1 ) % QUEUE_SIZE;
  }
  count--;
  return x;
}
float getAverage() {
  if (count < QUEUE_SIZE) {
    Serial.println("Queue is not full yet. current count: ");
    Serial.println(count);
    return 1;
  }

  float sum = 0;

  //we create a temporary variable to represent front
  int idx = front;
  
  //we then iterate through the list and we increment sum with the stored values in the queue
  for (int i = 0; i < QUEUE_SIZE; i++)
  {
    sum += tempQueue[idx];
    //we update idx to point to the next element
    idx = (idx + 1) % QUEUE_SIZE;
  }

  //we then get the average which is the sum of the elements divided by the queue size
  float average = sum / QUEUE_SIZE;

  //we iterate the list and we dequeue each and every element in the list
  for (int i = 0; i < QUEUE_SIZE; i++)
  {
    dequeue();
  }
  Serial.println("Averaged and dequeued 10 values.");
  //we return the average
  return average;

}