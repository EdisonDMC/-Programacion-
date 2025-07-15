include <ESP32Servo.h> 
Servo myServo; 
 
unsigned long startTime; 
unsigned long delayTime = 10000; // 10 segundos 
bool moved = false; 
 
void setup() { 
  Serial.begin(115200); 
   
  myServo.setPeriodHertz(150);  // Establece la frecuencia típica de servo 
(50 Hz) 
  myServo.attach(16);          // Conecta el servo al pin D16 
 
  myServo.write(0);            // Posición inicial 
  startTime = millis();        // Inicia temporizador 
} 
 
void loop() { 
  unsigned long currentTime = millis(); 
 
  if (!moved && (currentTime - startTime >= delayTime)) { 
    myServo.write(90); // Mover el servo a 90 grados 
    Serial.println("¡Temporizador cumplido! Servo activado."); 
    moved = true; 
  } 
 
  // Opcional: volver a 0 después de 5 segundos 
  if (moved && (currentTime - startTime >= delayTime + 5000)) { 
    myServo.write(0); 
    Serial.println("Servo regresado a posición inicial."); 
    moved = false; 
    startTime = millis(); // Reinicia temporizador 
  } 
} 
