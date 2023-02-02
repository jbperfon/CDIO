#include <Adafruit_ADS1X15.h>                   //Incluimos la librería
Adafruit_ADS1X15 ads1115;

double adc0;
double adc1;
double adc2;
double adc3;
double vo;
double ph;
double humedad;
double salinidad;
double temperatura;
double luminosidad;

class  sensores {

  private:

    const int seco = 30290;
    const int mojado = 17300;
    const int consal = 580;
    const int sinsal = 1024;
    int power_pin = 5;
    int Offset = 0;
    int samplingInterval = 20;
    int printInterval = 800;
    int ArrayLength = 40;
    int pHArray[40];
    int pHArrayIndex = 0;

  public:
   // void  sensores();
    double medida_humedad();
    double medida_salinidad();
    double medida_temperatura();
    double medida_ph();
    double medida_luminosidad();
    void configuracion();
};

//void sensores::sensores() {
//}

void sensores::configuracion() {
  ads1115.begin();                                    //Iniciamos ads1115
  ads1115.setGain(GAIN_ONE);
  pinMode(power_pin, OUTPUT);
}
/**********************************************************************************************************************************************************/
double sensores::medida_humedad() {

  int sensorValue = 0;

  int adc0 = ads1115.readADC_SingleEnded(0);

  Serial.print("Porcentaje:");
  int humedad = map(sensorValue, 30290, 17300, 0, 100); /* Porcentaje de medida*/
  if (humedad > 100) {
    humedad = 100;
  }
  if (humedad < 0) {
    humedad = 0;
  }
    return humedad;
     Serial.print ("%");
  Serial.print("Humedad:  ");


  Serial.println(humedad, DEC);
 

  delay(1000);
}
/**********************************************************************************************************************************************************/
double sensores::medida_salinidad() {

  digitalWrite (power_pin, HIGH);
  delay (500);
  adc0 = analogRead (adc0);
  digitalWrite (power_pin, LOW);

  int salinidad = map(adc0, 580, 1024, 0, 100);
  if (salinidad > 100) {
    salinidad = 100;
  }

  if (salinidad < 0) {
    salinidad = 0;
  }
    return salinidad;

     //Serial.print("Salinidad= ");

     //Serial.print(salinidad, DEC);

     Serial.println("%");
  
  delay(1000);
}
/**********************************************************************************************************************************************************/

double sensores::medida_temperatura() {

  int adc1 = ads1115.readADC_SingleEnded(1);
  float b = 0.79;
  float m = 0.033;
  float vo = (adc0 * 4.096) / 32767;

  float temperatura = ((vo - b) / m);
  //Serial.print("Temperatura:");
  //Serial.print(temperatura, 1);
  Serial.println("º");
  return temperatura;
  delay(1000);
}

/**********************************************************************************************************************************************************/
double sensores::medida_ph() {

  static float pHValue, adc2;
  adc2 = (ads1115.readADC_SingleEnded(2));
  double voltage= (adc2 * 4.096) / (pow(2, 15) - 1);
  double ph = (3.5 * voltage + 0);
  //Serial.print(" PH Value: ");
  //Serial.println(ph, 2);
  return ph;
  delay(1000);

  /**********************************************************************************************************************************************************/
}
/**********************************************************************************************************************************************************/

double sensores::medida_luminosidad() {

  int adc3 = ads1115.readADC_SingleEnded(3);

  double luminosidad = (adc3 * 4.096) / 32767;

  //Serial.print("Luminosidad: ");
  //Serial.println(luminosidad, 3);

  return luminosidad;

  //Serial.print("Luminosidad: ");
  delay(1000);
}

/**********************************************************************************************************************************************************/
