#include <DHT.h>

// DHT11 setup
#define DHTPIN 2
#define DHTTYPE DHT11
DHT dht(DHTPIN, DHTTYPE);

// Pulse sensor setup
const int pulsePin = A1;
int signal;
int threshold = 540; // Adjust this after testing raw signal values

unsigned long lastBeatTime = 0;
unsigned long startTime = 0;
const unsigned long measureDuration = 10000; // 10 seconds
const unsigned long debounceDelay = 600; // 600 ms debounce

int beatCount = 0;
bool pulseDetected = false;

int BPM = 0;

void setup() {
  Serial.begin(9600);
  dht.begin();
  startTime = millis();

  // Optional: Show analog signal for threshold tuning
  // Uncomment this section, observe values and adjust threshold
  /*
  while (true) {
    signal = analogRead(pulsePin);
    Serial.println(signal);
    delay(100);  // adjust delay to 50ms for faster readings
  }
  */
}

void loop() {
  float temp = dht.readTemperature();
  float hum = dht.readHumidity();
  signal = analogRead(pulsePin);

  unsigned long currentTime = millis();
  bool digitalPulse = (signal > threshold);

  // Detect new beat with debounce
  if (digitalPulse && !pulseDetected && (currentTime - lastBeatTime > debounceDelay)) {
    beatCount++;
    lastBeatTime = currentTime;
    pulseDetected = true;
  }

  // Reset for next detection
  if (!digitalPulse) {
    pulseDetected = false;
  }

  // Every 10 seconds, calculate BPM
  if (currentTime - startTime >= measureDuration) {
    BPM = beatCount * 6;
    BPM = constrain(BPM, 0, 200);


    // Print results
    Serial.print("🌡 Temp: ");
    Serial.print(temp);
    Serial.print(" °C, 💧 Hum: ");
    Serial.print(hum);
    Serial.print(" %, 💓 Pulse: ");
    Serial.print(digitalPulse ? "HIGH" : "LOW");
    Serial.print(", BPM: ");
    Serial.print(BPM);

    if (BPM > 120) {
      Serial.print(" 🚨 ALERT: BPM too high!");
    }
    Serial.println();

    // Reset for next window
    beatCount = 0;
    startTime = currentTime;
  }
}
