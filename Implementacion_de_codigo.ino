#include <ESP8266WiFi.h>
#include "sensores.h"                                               

//---------------- DECLARACIÓN SENSORES ----------------------------------------------
sensores sens;      //Sensores

//------------ WIFI ------------------------------------------------------------------
// Comentar/Descomentar para ver mensajes de depuracion en monitor serie y/o respuesta del HTTP server
#define PRINT_DEBUG_MESSAGES
//#define PRINT_HTTP_RESPONSE

// Comentar/Descomentar para conexion Fuera/Dentro de UPV
#define WiFi_CONNECTION_UPV

// Selecciona que servidor REST quieres utilizar entre ThingSpeak y Dweet
#define REST_SERVER_THINGSPEAK //Selecciona tu canal para ver los datos en la web (https://thingspeak.com/channels/1935509)
//#define REST_SERVER_DWEET //Selecciona tu canal para ver los datos en la web (http://dweet.io/follow/PruebaGTI)

///////////////////////////////////////////////////////
/////////////// WiFi Definitions /////////////////////
//////////////////////////////////////////////////////

#ifdef WiFi_CONNECTION_UPV //Conexion UPV
  //const char WiFiSSID[] = "GTI1";
  //const char WiFiPSK[] = "1PV.arduino.Toledo";
  const char WiFiSSID[] = "TP-LINK_6CAE";
  const char WiFiPSK[] = "41422915";
#else //Conexion fuera de la UPV
  const char WiFiSSID[] = "TP-LINK_6CAE";             
  const char WiFiPSK[] = "41422915";        
#endif



///////////////////////////////////////////////////////
/////////////// SERVER Definitions /////////////////////
//////////////////////////////////////////////////////

#if defined(WiFi_CONNECTION_UPV) //Conexion UPV
  const char Server_Host[] = "proxy.upv.es";
  const int Server_HttpPort = 8080;
#elif defined(REST_SERVER_THINGSPEAK) //Conexion fuera de la UPV
  const char Server_Host[] = "api.thingspeak.com";
  const int Server_HttpPort = 80;
#else
  const char Server_Host[] = "dweet.io";
  const int Server_HttpPort = 80;
#endif

WiFiClient client;

///////////////////////////////////////////////////////
/////////////// HTTP REST Connection ////////////////
//////////////////////////////////////////////////////

#ifdef REST_SERVER_THINGSPEAK 
  const char Rest_Host[] = "api.thingspeak.com";
  String MyWriteAPIKey="3KJQOODMGWA4S8WL"; //"H2A0VRM033LN1XUX" Escribe la clave de tu canal ThingSpeak   98S01TPNU1JL3IM7    Q4KUZ1HQEV21LEIG 
#else 
  const char Rest_Host[] = "dweet.io";
  String MyWriteAPIKey="PruebaGTI"; // Escribe la clave de tu canal Dweet
#endif

#define NUM_FIELDS_TO_SEND 5 //Numero de medidas a enviar al servidor REST (Entre 1 y 8)

/////////////////////////////////////////////////////
/////////////// Pin Definitions ////////////////
//////////////////////////////////////////////////////

const int LED_PIN = 5; // Thing's onboard, green LED

/////////////////////////////////////////////////////
/////////////// WiFi Connection ////////////////
//////////////////////////////////////////////////////

void connectWiFi()
{
  byte ledStatus = LOW;

  #ifdef PRINT_DEBUG_MESSAGES
    Serial.print("MAC: ");
    Serial.println(WiFi.macAddress());
  #endif
  
  WiFi.begin(WiFiSSID, WiFiPSK);

  while (WiFi.status() != WL_CONNECTED)
  {
    // Blink the LED
    digitalWrite(LED_PIN, ledStatus); // Write LED high/low
    ledStatus = (ledStatus == HIGH) ? LOW : HIGH;
    #ifdef PRINT_DEBUG_MESSAGES
       Serial.println(".");
    #endif
    delay(500);
  }
  #ifdef PRINT_DEBUG_MESSAGES
     Serial.println( "WiFi Connected" );
     Serial.println(WiFi.localIP()); // Print the IP address
  #endif
}

/////////////////////////////////////////////////////
/////////////// HTTP POST  ThingSpeak////////////////
//////////////////////////////////////////////////////

void HTTPPost(String fieldData[], int numFields){

// Esta funcion construye el string de datos a enviar a ThingSpeak mediante el metodo HTTP POST
// La funcion envia "numFields" datos, del array fieldData.
// Asegurate de ajustar numFields al número adecuado de datos que necesitas enviar y activa los campos en tu canal web
  
    if (client.connect( Server_Host , Server_HttpPort )){
       
        // Construimos el string de datos. Si tienes multiples campos asegurate de no pasarte de 1440 caracteres
   
        String PostData= "api_key=" + MyWriteAPIKey ;
        for ( int field = 1; field < (numFields + 1); field++ ){
            PostData += "&field" + String( field ) + "=" + fieldData[ field ];
        }     
        
        // POST data via HTTP
        #ifdef PRINT_DEBUG_MESSAGES
            Serial.println( "Connecting to ThingSpeak for update..." );
        #endif
        client.println( "POST http://" + String(Rest_Host) + "/update HTTP/1.1" );
        client.println( "Host: " + String(Rest_Host) );
        client.println( "Connection: close" );
        client.println( "Content-Type: application/x-www-form-urlencoded" );
        client.println( "Content-Length: " + String( PostData.length() ) );
        client.println();
        client.println( PostData );
        #ifdef PRINT_DEBUG_MESSAGES
            Serial.println( PostData );
            Serial.println();
            //Para ver la respuesta del servidor
            #ifdef PRINT_HTTP_RESPONSE
              delay(500);
              Serial.println();
              while(client.available()){String line = client.readStringUntil('\r');Serial.print(line); }
              Serial.println();
              Serial.println();
            #endif
        #endif
    }
}

////////////////////////////////////////////////////
/////////////// HTTP GET  ////////////////
//////////////////////////////////////////////////////

void HTTPGet(String fieldData[], int numFields){
  
// Esta funcion construye el string de datos a enviar a ThingSpeak o Dweet mediante el metodo HTTP GET
// La funcion envia "numFields" datos, del array fieldData.
// Asegurate de ajustar "numFields" al número adecuado de datos que necesitas enviar y activa los campos en tu canal web
  
    if (client.connect( Server_Host , Server_HttpPort )){
           #ifdef REST_SERVER_THINGSPEAK 
              String PostData= "GET https://api.thingspeak.com/update?api_key=";
              PostData= PostData + MyWriteAPIKey ;
           #else 
              String PostData= "GET http://dweet.io/dweet/for/";
              PostData= PostData + MyWriteAPIKey +"?" ;
           #endif
           
           for ( int field = 1; field < (numFields + 1); field++ ){
              PostData += "&field" + String( field ) + "=" + fieldData[ field ];
           }
          
           
           #ifdef PRINT_DEBUG_MESSAGES
              Serial.println( "Connecting to Server for update..." );
           #endif
           client.print(PostData);         
           client.println(" HTTP/1.1");
           client.println("Host: " + String(Rest_Host)); 
           client.println("Connection: close");
           client.println();
           #ifdef PRINT_DEBUG_MESSAGES
              Serial.println( PostData );
              Serial.println();
              //Para ver la respuesta del servidor
              #ifdef PRINT_HTTP_RESPONSE
                delay(500);
                Serial.println();
                while(client.available()){String line = client.readStringUntil('\r');Serial.print(line); }
                Serial.println();
                Serial.println();
              #endif
           #endif  
    }
}

void setup() {
  Serial.begin(115200);
  Serial.println("Iniciando:");
  ads1115.begin();                                    //Inicializa ads1115
  ads1115.setGain(GAIN_ONE);
  Serial.println("Estamos tomando medidas."); 

 #ifdef PRINT_DEBUG_MESSAGES
    Serial.begin(115200);
  #endif
  
  connectWiFi();
  digitalWrite(LED_PIN, HIGH);

  #ifdef PRINT_DEBUG_MESSAGES
      Serial.print("Server_Host: ");
      Serial.println(Server_Host);
      Serial.print("Port: ");
      Serial.println(String( Server_HttpPort ));
      Serial.print("Server_Rest: ");
      Serial.println(Rest_Host);
  #endif

  
}

void loop(){

//humedad = sens.medida_humedad();
//salinidad = sens.medida_salinidad();
//temperatura = sens.medida_temperatura();
//ph = sens.medida_ph();
//luminosidad = sens.medida_luminosidad();

String data[ 5 ];

    
    data[ 1 ] = String(1); //Escribimos el dato de pH
    #ifdef PRINT_DEBUG_MESSAGES
        Serial.print( "Humedad = " );
        Serial.println( data[ 1 ] );
    #endif

    data[ 2 ] = String(2); //Escribimos el dato de humedad
    #ifdef PRINT_DEBUG_MESSAGES
        Serial.print( "Salinidad = " );
        Serial.println( data[ 2 ] );
    #endif

    data[ 3 ] = String(3);
    #ifdef PRINT_DEBUG_MESSAGES
        Serial.print( "Tempreatura = " );
        Serial.println( data[ 3 ] );
    #endif

    data[ 4 ] = String(4);
    #ifdef PRINT_DEBUG_MESSAGES
        Serial.print( "PH = " );
        Serial.println( data[ 4 ] );
    #endif

     data[ 5 ] = String(5);
    #ifdef PRINT_DEBUG_MESSAGES
        Serial.print( "Luminosidad = " );
        Serial.println( data[ 5 ] );
    #endif
    
    HTTPPost( data, NUM_FIELDS_TO_SEND );
	
	delay(10000);

}
