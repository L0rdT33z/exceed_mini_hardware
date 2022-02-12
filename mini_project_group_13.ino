#include<WiFi.h>
#include<HTTPClient.h>
#include<ArduinoJson.h>


int th[3] = {30,30,30};
int touch_pin_1 = 2;
int touch_pin_2 = 13;
int touch_pin_3 = 12;


int debounce_pin1 = 0;
int debounce_pin2 = 0;
int debounce_pin3 = 0;


int led_1 = 5;
int led_2 = 33;
int led_3 = 32;

xTaskHandle TaskHandle_1;

void setup(){
  pinMode(touch_pin_1,INPUT);
  pinMode(touch_pin_2,INPUT);
  pinMode(touch_pin_3,INPUT);
  pinMode(led_1,OUTPUT);
  pinMode(led_2,OUTPUT);
  pinMode(led_3,OUTPUT);
  digitalWrite(led_1,HIGH);
  digitalWrite(led_2,LOW);
  digitalWrite(led_3,LOW);
  Serial.begin(115200);
  xTaskCreatePinnedToCore(Task1, "WifiTask", 10240, NULL, 1, &TaskHandle_1, 0); // stable
  delay(4000);
  WiFi.mode(WIFI_STA);
  WiFi_Connect();
}

int checkTouchPin(int pin,int thh){
  int readd = touchRead(pin);
  if(readd>th[thh-1]){
      return 0;
    
  }else{
      return 1;
  }
}

char str[50];
const int _size = 2*JSON_OBJECT_SIZE(2);
StaticJsonDocument<_size> JSONPost;


int room1 = 0;
int room2 = 0;
int room3 = 0;


const char* ssid="Tee";
const char* password = "11111111";
const char* url = "https://ecourse.cpe.ku.ac.th/exceed13/api/open-close";

void WiFi_Connect(){
  WiFi.disconnect();
  WiFi.begin(ssid,password);
  while(WiFi.status()!=WL_CONNECTED){
    delay(1000);
    Serial.println("Connecting to WiFi...");
  }
  Serial.println("Connected to the WiFi network");
  Serial.print("IP Address : ");
  Serial.println(WiFi.localIP());
}


void _post(int room, int statuss){
  if(WiFi.status() == WL_CONNECTED){
    HTTPClient http;

    http.begin(url);
    http.addHeader("Content-Type","application/json");
  
    JSONPost["room"] = room;
    JSONPost["close"] = statuss;
    serializeJson(JSONPost,str);
    int httpCode = http.POST(str);

    if(httpCode == HTTP_CODE_OK){
      String payload = http.getString();
      Serial.println(str);
      Serial.println(httpCode);
      Serial.println(payload);
      
    }else{
      Serial.println(httpCode);
      Serial.println("ERROR on HTTP Request");
    }
  }else{
    WiFi_Connect();
  }
  
}



//WIFI TASK
int oldroom1 = 0;
int oldroom2 = 0;
int oldroom3 = 0;

void Task1(void* parameter){
  while(1){
    if(oldroom1 != room1){
      oldroom1 = room1;
      _post(1,oldroom1);
    }
    if(oldroom2 != room2){
      oldroom2 = room2;
      _post(2,oldroom2);
    }
    if(oldroom3 != room3){
      oldroom3 = room3;
      _post(3,oldroom3);
    }
    vTaskDelay(50 / portTICK_PERIOD_MS);
  }
   
}
//

//CHECK PIN & LED
void loop(){


  int c1 = checkTouchPin(touch_pin_1,1);
  int c2 = checkTouchPin(touch_pin_2,2);
  int c3 = checkTouchPin(touch_pin_3,3);

  //Simple Debounce
  if(c1 == 1){
    debounce_pin1++;
    if(debounce_pin1 >= 20){
     debounce_pin1 = 20;
    } 
  }else{
    debounce_pin1--;
    if(debounce_pin1 <= 0){
     debounce_pin1 = 0;
    }   
  }

  if(c2 == 1){
    debounce_pin2++;
    if(debounce_pin2 >= 20){
     debounce_pin2 = 20;
    } 
  }else{
    debounce_pin2--;
    if(debounce_pin2 <= 0){
     debounce_pin2 = 0;
    }   
  }

  if(c3 == 1){
    debounce_pin3++;
    if(debounce_pin3 >= 20){
     debounce_pin3 = 20;
    } 
  }else{
    debounce_pin3--;
    if(debounce_pin3 <= 0){
     debounce_pin3 = 0;
    }   
  }

  if((debounce_pin1 == 20 && room1 == 0) || (debounce_pin1 == 0 && room1 == 1)){
      room1 = c1;
      digitalWrite(led_1,!c1);
      Serial.print("room1 changed to ");
      Serial.println(room1);
  }

  if((debounce_pin2 == 20 && room2 == 0) || (debounce_pin2 == 0 && room2 == 1)){
      room2 = c2;
      digitalWrite(led_2,c2);
      Serial.print("room2 changed to ");
      Serial.println(room2);
  }
  
  if((debounce_pin3 == 20 && room3 == 0) || (debounce_pin3 == 0 && room3 == 1)){
      room3 = c3;
      digitalWrite(led_3,c3);
      Serial.print("room3 changed to ");
      Serial.println(room3);
  }

  delay(10);
  
}
