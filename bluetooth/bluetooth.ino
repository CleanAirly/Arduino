/**
 * @file Bluetooth.ino
 *
 * @mainpage BLE Documentation
 *
 * @section description Description
 * Documentación generada de arduino para poryecto biometría.
 *
 * @section circuit Circuit
 * - Vout -> pin 25.
 * - Vtemp -> pin 26.
 * - Vref -> pin 27.
 * - Vin -> 3.3v pin.
 * - gnd -> gnd pin.
 *
 * @section libraries Libraries
 * - Adafruit_bluefruit (https://github.com/adafruit/Adafruit_BluefruitLE_nRF52.git)
 *   - Interacts with on-board IMU.
 *
 * @section notes Notes
 * - Esto es una nota.
 *
 * @section todo TODO
 * - Esto es algo que queda por hacer? como un roadmap?.
 *
 * @section author Author
 * - Created by Cristian Borgoñoz Mas on 06/10/2023.
 * - Modified by Cristian Borgoñoz Mas on 06/10/2023.
 *
 * Copyright (c) 2023 Cristian Borgoñoz Mas.  All rights reserved. (Es broma :))
 */

 // Libraries
#include <bluefruit.h>
#include <Adafruit_TinyUSB.h>
/**
 * The standard Arduino setup function used for setup and configuration tasks.
 */

#define PIN_Vgas 5
#define PIN_Vref 28
#define PIN_Vtemp 29

double Vgas;
double Vref; 
double Vtemp;
int valorPpm;


void setup() {
   
   // Inicializamos puerto serie y le asignamos un retardo
   Serial.begin(115200);
   pinMode(PIN_Vgas, INPUT);
   pinMode(PIN_Vref, INPUT);
   pinMode(PIN_Vtemp, INPUT);

   while ( !Serial ) delay(10);

   Serial.println("-----------------------\n");
   Serial.println(" PRUEBAS iBeacon  ");
   Serial.println("-----------------------\n");

   
   // Initialise the Bluefruit module
   Serial.println("Initialise the Bluefruit nRF52 module");
   Bluefruit.begin();

   // Set the advertised device name
   Serial.println("Setting Device Name to GTI-3A-CRISTIAN " );

   Bluefruit.setName("GTI-3A-CRISTIAN");
   Bluefruit.ScanResponse.addName();



   // Mostramos que la inicialización ha funcionado
   Serial.println("\nAdvertising");

}
 



/**
 * Función para publicar beacons
 */
void startAdvertising() {

   Serial.println( " startAdvertising() " );

   Serial.println( " Bluefruit.Advertising.stop(); ");
   Bluefruit.Advertising.stop(); // ya lo enchufo luego

   
   // Advertising packet
   Bluefruit.Advertising.addFlags(BLE_GAP_ADV_FLAGS_LE_ONLY_GENERAL_DISC_MODE);
   Bluefruit.Advertising.addTxPower();


   // Include Name
   Serial.println( " Bluefruit.Advertising.addName(); " );
   Bluefruit.Advertising.addName();

   
   Serial.println( " Bluefruit.Advertising.setBeacon( elBeacon ); ");

   // El UUID que meteremos en el beacon
   uint8_t beaconUUID[16] = {
     'E', 'P', 'S', 'G', 'G', 'T', 'I', '-',
     'C', 'R', 'I', 'S', 'T', 'I', 'A', 'N'
     };

   /**
    * Objeto BLEBeacon.
    * UUID, major, minor, 73 -> elBeacon()
    * Y publica el beacon
    */
  Serial.print("Temperatura (ºC): ");
  Serial.println(leerTemperatura());
  Serial.print("Concentracion (ppm): ");
  Serial.println(correcion());
   BLEBeacon elBeacon( beaconUUID, correcion()+100, leerTemperatura(), 73 );
   elBeacon.setManufacturer( 0x004c ); // aple id
   Bluefruit.Advertising.setBeacon( elBeacon );

   // Restart
   Bluefruit.Advertising.restartOnDisconnect(true);
   // Intervalo en el que está publicando
   Bluefruit.Advertising.setInterval(32, 1000);    // in unit of 0.625 ms
   // Cada cuanto publica
   Bluefruit.Advertising.setFastTimeout(10000);      // number of seconds 

   Bluefruit.Advertising.start(0);                // 0 = Don't stop 


   Serial.println( " Bluefruit.Advertising.start(0);" );

} 
 



/**
 * The standard Arduino loop function used for repeating tasks.
 */
namespace Loop {
   int cont = 0;
};
// ....................................................
void loop() {

   using namespace Loop;

   cont++;

  
    // Inicializamos la publicación de beacons
   startAdvertising();

   delay(1000);

   Serial.print( " ** loop cont=" );
   Serial.println( cont );
} // ()

double leerGas(){
  Vgas = (analogRead(PIN_Vgas)*3.3)/4096;
  Vref = (analogRead(PIN_Vref)*3.3)/4096;

  double medida = Vgas-Vref;
  return medida;
}

double leerTemperatura(){
  Vtemp = (analogRead(PIN_Vtemp)*3.3)/4096; 
  double Temp = 87*Vtemp-18;
  return Temp;
}

double concentracion(){
  double resultado = leerGas()/(4.51*499*0.000001);
  return resultado;
}

double correcion(){
  double incrementoT = leerTemperatura() - 20;
  double corregida = incrementoT * 0.003;
  double resultado = concentracion() - corregida;
  return resultado;
}
