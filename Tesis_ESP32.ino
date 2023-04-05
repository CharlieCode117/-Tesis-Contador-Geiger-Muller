    // Proyecto de Tesis //
    // Monitor de Radiación Nuclear con Detectores Gaseosos (Geiger-Müller) - Código Final. //
    // Carlos Eduardo Castillo Godínez. //
    // Ingeniería en Telecomunicaciones, Sistemas y Electrónica.
    // 10 - Marzo - 2023 //

        //Inclusiones de Librerías y parámetros de operación
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <Adafruit_Sensor.h>
#include <DHT.h>
#include <SPI.h>

#define SCREEN_WIDTH 128      // Ancho de la pantalla OLED en píxeles
#define SCREEN_HEIGHT 64      // Altura de la pantalla OLED en píxeles
#define OLED_RESET -1         // Pin RESET de la pantalla OLED, -1 si no está conectado
#define DHTTYPE DHT22         // DHT 22 (AM2302)
#define DHTPIN 5              // Digital pin connected to the DHT sensor.

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET); //Objeto de la pantalla OLED

// Definición de variables globales 
int hora = 0;              // Variable Temporal horas transcurridas.
int minuto = 0;            // Variable Temporal minutos transcurridos.
int segundo = 0;           // Variable Temporal segundos transcurridos.
DHT dht(DHTPIN, DHTTYPE);  // Función de tipo de sensor.

const int pulsePin = 4;       // Pin GPIO utilizado para leer la salida de pulso del contador Geiger
volatile long pulseCount = 0; // Variable Volátil de inicio de pulsos.
// FIN de Definición de variables globales 


void IRAM_ATTR handleInterrupt() { //Función de interrupción que se ejecuta cuando se detecta un pulso en el pin de entrada
  pulseCount++;
}

void setup() {
  Serial.begin(9600);              // Inicializar la comunicación serial
  dht.begin();
  pinMode(pulsePin, INPUT_PULLUP); // Configurar el pin de entrada como una entrada digital con pull-up
  digitalWrite(pulsePin, HIGH);    // Inicia el digitalWrite en ALTO.
  attachInterrupt(digitalPinToInterrupt(pulsePin), handleInterrupt, FALLING); //Configurar una interrupción en el pin de entrada para detectar flancos de bajada
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C); // Inicializar la pantalla OLED con la dirección I2C 0x3C
  }

void loop() {
  delay(1000); //Esperar un segundo para acumular pulsos
  detachInterrupt(digitalPinToInterrupt(pulsePin)); //Desconectar la interrupción para leer el contador
  Serial.print("Conteo de pulsos: ");
  Serial.println(pulseCount); //Imprimir el número de pulsos detectados en un segundo
    display.setTextColor(WHITE); //Configurar el color de texto en blanco
    display.clearDisplay();
  
   //Título de la Interfaz
    display.setTextSize(1);
    display.setCursor(0, 0);
    display.print("Contador GeigerMuller");
    display.setTextSize(1);
    display.setCursor(0, 8);
    display.print("[Proyecto de Tesis]");
    //Fin de Título de Interfaz 
 attachInterrupt(digitalPinToInterrupt(pulsePin), handleInterrupt, FALLING); //Volver a conectar la interrupción para detectar el siguiente pulso
  // FIN de configuración primoridial de arranque.

// INICIO DEL CONTADOR
  display.setCursor(80,55);
  if(hora>=10){
    display.print(hora);}
  else{
    display.print("0");
    display.print(hora);
  }
  display.print(":");
   if(minuto>=10){
    display.print(minuto);
  }
  else{
   display.print("0");
  display.print(minuto);
  }
  display.print(":");
    if(segundo>=10){
   display.print(segundo);
  }
  else{
    display.print("0");
    display.print(segundo);
  }
   display.print("    ");
  segundo++;
  delay(10);
  if (segundo==60){ 
    segundo=0;
    minuto++;
    if (minuto==60){
     minuto=0;
      hora++;
      if(hora==24) {
        hora=0;
   	  } //Fin Ciclo HH
 	  } //Fin Ciclo Primario
  } // Fin del CONTADOR

  //Configuración CPM:
float MillisTranscurridos = millis();           //Crea una lectura continua promedio para CPM basada en las cuentas y el tiempo transcurrido
float convertCPM = (60000)/MillisTranscurridos;  
long cpm = round((((pulseCount-1)/2)*convertCPM));
   display.setTextSize(1);
   display.setCursor(73, 45);
   display.print("CPM:");
   display.setTextSize(1);
   display.setCursor(100, 45);
   display.print(cpm);
  //FIN de Configuración CPM.
   
  // Configuración de Dosis Radiactiva: 
float dosis = cpm*0.0057;          //Convierte CPM a microSieverts por hora, basada en el factor de conversion del tubo Geiger-Müller
      display.setTextSize(1);
      display.setCursor(0, 55);
      display.print("uSv/Hr:");
      display.setTextSize(1);
      display.setCursor(42, 55);
      display.print(dosis);
  // FIN de Configuración de Dosis Radiactiva. 
    
      if (pulseCount <99999){                              
      display.setTextSize(1);
      display.setCursor(0, 45);
      display.print("CT:");
      display.setTextSize(1);
      display.setCursor(25, 45);
      display.print(pulseCount);   
    }
  
    else if (pulseCount > 100000){            // Si el tiempo total es mayor que 1000, las cuentas totales se dividen para convertilo a "Kilo"
      volatile long pulseKCount = pulseCount/1000; // Ecuación de definición condicional que hasta llegar a 10,000 se utilizará el símbolo "k".
      display.setTextSize(1);
      display.setCursor(0, 45);
      display.print("CTk:");
      display.setTextSize(1);
      display.setCursor(25, 45);
      display.print(pulseKCount);           // "k" es usado para referirse a un numero igual o mayor a 1000 conteos
      display.setTextSize(1);
      display.setCursor(61, 45);
      display.print("k");
    }

 //Variables de lectura del sensor
  float t = dht.readTemperature();
  float h = dht.readHumidity();
  if (isnan(h) || isnan(t)) {
    Serial.println("Failed to read from DHT sensor!");
  }

  // display Temperatura
      display.setTextSize(1);
      display.setCursor(0,20);
      display.print("Temperatura:");
      display.setTextSize(1);
      display.setCursor(73,20);
      display.print(t);
      display.print(" ");
      display.setTextSize(1);
      display.cp437(true);
      display.write(167);
      display.setTextSize(1);
      display.print("C");
  
   // Display Humedad
      display.setTextSize(1);
      display.setCursor(0, 30);
      display.print("Humedad:");
      display.setTextSize(1);
      display.setCursor(73, 30);
      display.print(h);
      display.print(" %"); 
display.display(); //Mostrar el contenido de la pantalla OLED
}
