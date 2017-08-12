#include <ESP8266WiFi.h>
#include <WebSocketsServer.h>

// WebSockets library : https://github.com/Links2004/arduinoWebSockets
WebSocketsServer webSocket = WebSocketsServer(81);

const char* ssid     = "SINGTEL-7FBB";
const char* password = "*****";
const int m1Pin1 = 14;
const int m1Pin2 = 12;
const int m1PWM = 5;
const int m2Pin1 = 13;
const int m2Pin2 = 15;
const int m2PWM = 4;

void webSocketEvent(uint8_t num, WStype_t type, uint8_t * payload, size_t length) {
int pwmL;
int pwmR;  
Serial.begin(115200);
    switch(type) {
        case WStype_DISCONNECTED:
            digitalWrite(m1Pin1, LOW);
            digitalWrite(m1Pin2, LOW);
            digitalWrite(m2Pin1, LOW);
            digitalWrite(m2Pin2, LOW);           
            break;
        case WStype_CONNECTED:
            {
            IPAddress ip = webSocket.remoteIP(num);   
            Serial.print("Remote IP : "); 
            Serial.println(ip);           
            }
            break;
        case WStype_TEXT:
            {
             String text = String((char *) &payload[0]);
             Serial.print("The text is ");
             Serial.println(text);

             int dashIndex = text.indexOf('-');
             String textL = text.substring(0, dashIndex);
             String textR = text.substring(dashIndex+1);
             int valueL = textL.toInt();
             int valueR = textR.toInt();
             //leftmotor
             if (valueL >= 512){
               digitalWrite(m1Pin1, HIGH);
               digitalWrite(m1Pin2, LOW);
               int pwmL = (int)(((float(valueL)-512.0) / 512.0)*1024.0);
               Serial.print("The pwmL is ");
               Serial.println(pwmL);
               analogWrite(m1PWM, pwmL);
             } else if (valueL < 512) {
               digitalWrite(m1Pin2, HIGH);
               digitalWrite(m1Pin1, LOW);
               int pwmL = (int)(((512.0-float(valueL)) / 512.0)*1024.0);
               Serial.print("The pwmL is ");
               Serial.println(pwmL); 
               analogWrite(m1PWM, pwmL);
             }
             //right motor
             if (valueR >= 512){
               digitalWrite(m2Pin1, HIGH);
               digitalWrite(m2Pin2, LOW);
               int pwmR = (int)(((float(valueR)-512.0) / 512.0) * 1024.0);
               Serial.print("The pwmR is ");
               Serial.println(pwmR); 
               analogWrite(m2PWM, pwmR);
             } else if (valueR < 512) {
               digitalWrite(m2Pin2, HIGH);
               digitalWrite(m2Pin1, LOW);
               int pwmR = (int)(((512.0-float(valueR)) / 512.0) * 1024.0);
               Serial.print("The pwmR is ");
               Serial.println(pwmR); 
               analogWrite(m2PWM, pwmR);
             }
             analogWrite(16, valueL);            
            }                 
           break;
        
           case WStype_BIN:
                hexdump(payload, length);
                // echo data back to browser
                webSocket.sendBIN(num, payload, length);
                break;
    }
}

void setup() { 
    pinMode(12,OUTPUT);
    pinMode(13,OUTPUT);
    pinMode(14,OUTPUT);
    pinMode(15,OUTPUT);
    pinMode(16,OUTPUT);
    pinMode(5,OUTPUT);
    pinMode(4,OUTPUT);
    digitalWrite(16, HIGH);  // turn red led off (inverted)
    digitalWrite(16, LOW);
    delay(100);
    digitalWrite(16, HIGH);
    delay(100);
    digitalWrite(16, LOW);
    delay(100);
    digitalWrite(16, HIGH);
    WiFi.begin(ssid, password);

    while(WiFi.status() != WL_CONNECTED) {
        delay(100);
    }

    webSocket.begin();
    webSocket.onEvent(webSocketEvent);
    
    Serial.begin(115200);
    Serial.println();
    Serial.print("My IP : ");
    Serial.println(WiFi.localIP());
}

void loop() {
    webSocket.loop();
}
