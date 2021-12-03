#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <ArduinoJson.h>
#define SERVER_IP "192.168.43.205:8000"
#ifndef STASSID
#define STASSID "LOPEZ"
#define STAPSK  "12404250"
#endif
#include <math.h>
byte trig_a = 16;// D0;//salida agua (sensor ultrasonico)
byte echo_a = 5;//D1;//entrada agua (sensor ultrasonico)
byte trig_j = 4;//D2;//salida jabon (sensor ultrasonico)
byte echo_j = 0;//D3;//entrada jabon (sensor ultrasonico)
byte se_a = 2;//D4;//entrada agua (sensor infrarojo)
byte se_j = 14;//D5;//entrada jabon (sensor infrarojo) 
byte sa_a = 12;//D6;//salida agua(motor minibomba)
byte sa_j = 13;//D7;//salida jabon(motor minibomba)
byte s_e_a = HIGH;//declaramos variable y lo inicializamos en alto
byte s_e_j = HIGH;//declaramos variable y lo inicializamos en alto
void setup() {
  Serial.begin(115200);//definimos la comunicacion serial a 115200 baudios
  //conexion a wi-fi
  WiFi.begin(STASSID, STAPSK);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.print("Connected! IP address: ");
  Serial.println(WiFi.localIP());
  
  pinMode(echo_a,INPUT);//configuramos el pin como entrada
  pinMode(trig_a,OUTPUT);//configuramos el pin como salida
  
  pinMode(echo_j,INPUT);//configuramos el pin como entrada
  pinMode(trig_j,OUTPUT);//configuramos el pin como salida
  
  pinMode(se_a,INPUT);//configuramos el pin como entrada
  pinMode(se_j,INPUT);//configuramos el pin como entrada
  
  pinMode(sa_a,OUTPUT);//configuramos el pin como salida
  pinMode(sa_j,OUTPUT);//configuramos el pin como salida
}
void loop() {
  if ((WiFi.status() == WL_CONNECTED)) { 

    WiFiClient client;
    HTTPClient http;
    
    int porcentaje_a = f_porcentaje(trig_a,echo_a);//ejecutamos nuestra funcion que devuelve la distancia y guardamos su valor
    int porcentaje_j = f_porcentaje(trig_j,echo_j);//ejecutamos nuestra funcion que devuelve la distancia y guardamos su valor
    
    /*Serial.print("porcentaje agua : ");
    Serial.println(porcentaje_a);
    Serial.print("porcentaje jabon : ");
    Serial.println(porcentaje_j);
    Serial.println("");
    delay(2000);*/

    s_e_a = digitalRead(se_a);//resivimos su valor del sensor de agua y guardamos en una variable 
    s_e_j = digitalRead(se_j);//resivimos su valor del sensor de jabon y guardamos en una variable
  
    if (s_e_a == LOW & porcentaje_a >0 & porcentaje_a <100){//condicion para la agua
      digitalWrite(sa_a, HIGH);//ejecutamos una sentencia
      //Serial.println("todo esta bien con la agua");//imprimimos un texto
      delay(2000);//tenemos un retardo de 6 a 25 segundos
      digitalWrite(sa_a, LOW);
      int contador = contador+1;
      http.begin(client, "http://" SERVER_IP "/api/agua"); //HTTP//peticion post para agua
      http.addHeader("Content-Type", "application/json");

      //Serial.print("[HTTP] POST...\n");
      // tart conexión y enviar encabezado HTTP y bodys

      int contador_a = contador_a+1;
      String datos_a_enviar = "";
      StaticJsonDocument<200> doc;
      doc["cantidad"] = porcentaje_a;
      doc["contador"] = contador;
      serializeJson(doc, datos_a_enviar);
      
      int httpCode = http.POST(datos_a_enviar);

      // El código http será negativo en caso de error
      if (httpCode > 0) {
        // Se ha enviado el encabezado HTTP y se ha manejado el encabezado de respuesta del servidor
        Serial.printf("[HTTP] POST agua code: %d\n", httpCode);

        // archivo encontrado en el servidor
        if (httpCode == HTTP_CODE_OK) {
          const String& payload = http.getString();
          Serial.println("received payload:\n<<");
          Serial.println(payload);
          Serial.println(">>");
        }
      }else {
        Serial.printf("[HTTP] POST... failed, error: %s\n", http.errorToString(httpCode).c_str());
      }
      http.end();
      Serial.print("Nivel Agua ");
      Serial.print(porcentaje_a);
      delay(100);
    }
    //condicion para el jabon
    if (s_e_j == LOW  & porcentaje_j >0 & porcentaje_j <100){
      digitalWrite(sa_j, HIGH);
      //Serial.println("todo esta bien con el jabon");
      delay(2000);
      digitalWrite(sa_j, LOW);
      http.begin(client, "http://" SERVER_IP "/api/jabon"); //HTTP
      http.addHeader("Content-Type", "application/json");

      Serial.print("[HTTP] POST...\n");
      // tart conexión y enviar encabezado HTTP y bodys
      int contador_j = contador_j+1;
      String datos_a_enviar = "";
      StaticJsonDocument<200> doc;
      doc["cantidad"] = porcentaje_j;
      doc["contador"] = contador_j;
      serializeJson(doc, datos_a_enviar);
      
      int httpCode = http.POST(datos_a_enviar);

      // El código http será negativo en caso de error
      if (httpCode > 0) {
        // Se ha enviado el encabezado HTTP y se ha manejado el encabezado de respuesta del servidor
        Serial.printf("[HTTP] POST... code: %d\n", httpCode);

        // archivo encontrado en el servidor
        if (httpCode == HTTP_CODE_OK) {
          const String& payload = http.getString();
          Serial.println("received payload:\n<<");
          Serial.println(payload);
          Serial.println(">>");
        }
      } else {
        Serial.printf("[HTTP] POST... failed, error: %s\n", http.errorToString(httpCode).c_str());
      }
      http.end();
      Serial.print("Nivel Jabon ");
      Serial.print(porcentaje_j);
      delay(100);
    }
  }
}
//funcion que retorna la cantidad en porcentaje
int f_porcentaje(byte Trig, byte Echo){
  int duracion;
  float distancia;
  float cantidad;
  int porcentaje;

  digitalWrite(Trig,LOW);
  delayMicroseconds(4);

  digitalWrite(Trig,HIGH);
  delayMicroseconds(10);
  digitalWrite(Trig,LOW);

  duracion = pulseIn(Echo,HIGH);
  duracion = duracion/2;
  distancia = (duracion/29)-5;
  cantidad = 18-distancia;
  porcentaje = lround((cantidad*100)/18);
  
  return porcentaje;
}
