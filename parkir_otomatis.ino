#include<ESP8266WiFi.h>
#include<PubSubClient.h>
const char* ssid = "Fujasera";
const char* password = "murahenak";

#define mqttserver "broker.hivemq.com"
#define mqttport 1883


WiFiClient espClient;
PubSubClient client(espClient);

const int buzz = 5 ;
float jarakb;
int trigg = 15;
int echo =  13;
int jarbuz;
int led = 4;
char hasiljarak [4];
char hasilled [2];
int kondisiled;
bool mobilparkir, konBuzzer;

//Jadi sub
void callback(char* topic, byte* payload, unsigned int leght) {
  Serial.print ("Ada pesan : ");
  Serial.println ((char)payload[0]);
  if (mobilparkir==false) { 
    if(((char)payload[0]=='0')){
      konBuzzer = false;
    }
    else{
      konBuzzer = true;      
    }
  }
}

void setup() {
  // put your setup code here, to run once:
  
  pinMode(buzz, OUTPUT);
  pinMode(trigg, OUTPUT);    
  pinMode(echo, INPUT); 
  pinMode (led, OUTPUT);
  Serial.begin(115200);

  WiFi.begin(ssid,password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.println ("Sedang mengkoneksikan WiFi...");
  }

  Serial.println ("WiFi terkonek...");
  client.setServer(mqttserver,mqttport);
  client.setCallback(callback);

}

void reconnect () {
  while(!client.connected()) {
    Serial.print("Sedang mengkoneksi ke MQTT...");
    if (client.connect("monitoring data")) {
      Serial.println("connected");
      client.subscribe("Buzzer13");
    }
    else {
      Serial.print("Filed....");
      Serial.print("Harap menunggu sampai terkoneksi");
      delay(5000);
    }
  }
}



void loop() {
  // put your main code here, to run repeatedly:
    ultra();
    float durasib = pulseIn(echo, HIGH); 
    jarakb = (durasib*0.034) / 2   ;
    Serial.print("Jarak Mobil = "); 
    Serial.print(jarakb);
    Serial.print("cm");
    jarbuz= 10000-jarakb;
    //jarbuz = jarakb+75;
    //tone(buzz,jarbuz);
    
    // Mobil Keluar
      if ( jarakb > 5.00){
        /*if (mobilparkir==true) {
          konBuzzer = true;
          digitalWrite(buzz, HIGH);
        }*/
          Serial.println(" ( Mobil Tidak Terdeteksi )");
          //tone(buzz,100);
          delay(100);
          //noTone(buzz);
          //digitalWrite(buzz,HIGH);
          //konBuzzer = true;
          digitalWrite(led,LOW);
          kondisiled = 0;
          mobilparkir = false;
               
        //Mengatur button hp
        if(konBuzzer == true){
          digitalWrite(buzz,HIGH);
          tone(buzz,jarbuz);
        }
        else{
          digitalWrite(buzz, LOW);
          noTone(buzz);
          Serial.print ("Buzzer Mati");  
        }

    
    }
    
    else{
      //noTone(buzz);
      Serial.println(" ( Mobil Tedeteksi ) ");
      digitalWrite(buzz,LOW);
      noTone(buzz);
      digitalWrite(led,HIGH);
      kondisiled = 1;
      mobilparkir = true;
    }
    
    delay(100);
    Serial.println();
    dtostrf (jarakb,1,2,hasiljarak);
    dtostrf (kondisiled,1,0,hasilled);
    client.publish("LED",hasilled);
    client.publish("Jarak_Mobil",hasiljarak);
    if(!client.connected()){
      reconnect();
    }
    client.loop();
    
}
  //Ngetriger 
  void ultra()
  {
  digitalWrite(trigg, LOW);
  delayMicroseconds(8);
  digitalWrite(trigg, HIGH);
  delayMicroseconds(8);
  digitalWrite(trigg, LOW);
  delayMicroseconds(8);
  }
