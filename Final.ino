
#include <WiFi.h>
#include <FirebaseESP32.h>

//digital sensors
#include "DHT.h"

#define DHTPIN 4
#define DHTTYPE DHT11   
DHT dht(DHTPIN, DHTTYPE);


int Signal;   
int PulseSensorPurplePin = 32;  
#define FIREBASE_HOST "https://teamrio-backend.firebaseio.com/"
#define FIREBASE_AUTH "aR1Iykct0lM5mZWRzhS7IeqBx8CUggXI2vgkWYWC"
#define WIFI_SSID "GUSEC"
#define WIFI_PASSWORD "asdfg@54321"
#include "time.h"
const char* ntpServer = "pool.ntp.org";
const long  gmtOffset_sec = 0;
const int   daylightOffset_sec = 3600;
FirebaseJsonArray arr;

void printResult(FirebaseData &data);


unsigned long sendDataPrevMillis = 0;
//Define FirebaseESP32 data object
FirebaseData firebaseData;
FirebaseJson json;

//Firebase URI Path
String path2 = "IVYHacksSensorData";

//setup firebase
void setup()
{
  /*Connect to Wifi*/
  Serial.begin(9600);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  while (WiFi.status() != WL_CONNECTED)
  {
    Serial.print(".");
    delay(300);
  }

  //miscellaneous sensor setup
  dht.begin();

  /*Connect to firebase*/
  Firebase.begin(FIREBASE_HOST, FIREBASE_AUTH);
  Firebase.reconnectWiFi(true);
  //Set database read timeout to 1 minute (max 15 minutes)
  Firebase.setReadTimeout(firebaseData, 1000 * 60);
  //tiny, small, medium, large and unlimited.
  //Size and its write timeout e.g. tiny (1s), small (10s), medium (30s) and large (60s).
  Firebase.setwriteSizeLimit(firebaseData, "small");
  
 configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);
}

void loop()
{
  struct tm timeinfo;
  if(!getLocalTime(&timeinfo)){
    Serial.println("Failed to obtain time");
    return;
  }
  Serial.println(&timeinfo, "%A, %B %d %Y %H:%M:%S");
  Serial.print("Day of week: ");
  Serial.println(&timeinfo, "%A");
  Serial.print("Month: ");
  Serial.println(&timeinfo, "%B");
  Serial.print("Day of Month: ");
  Serial.println(&timeinfo, "%d");
  Serial.print("Year: ");
  Serial.println(&timeinfo, "%Y");
  Serial.print("Hour: ");
  Serial.println(&timeinfo, "%H");
  Serial.print("Hour (12 hour format): ");
  Serial.println(&timeinfo, "%I");
  Serial.print("Minute: ");
  Serial.println(&timeinfo, "%M");
  Serial.print("Second: ");
  Serial.println(&timeinfo, "%S");

  Serial.println("Time variables");
  char timeHour[3];
  strftime(timeHour,3, "%H", &timeinfo);
    char timeMinute[3];
  strftime(timeMinute,3, "%M", &timeinfo);
    char timeSec[3];
  strftime(timeSec,3, "%S", &timeinfo);
  Serial.println(timeHour);
  char timeWeekDay[10];
  strftime(timeWeekDay,10, "%A", &timeinfo);
  Serial.println(timeWeekDay);
  Serial.println();
  String path = "/80655ec-04f2-11eb-adc1-0242ac120002";
  path += timeHour ;
   path += timeMinute ;
    path += timeSec ;
  Serial.println(path2); 
  //--------------------------------------------------------------
  //DHT TemperatureSensor
    // Reading temperature or humidity takes about 250 milliseconds!
  // Sensor readings may also be up to 2 seconds 'old' (its a very slow sensor)
  //float h = dht.readHumidity();
  // Read temperature as Celsius (the default)
  float t = dht.readTemperature();
  // Read temperature as Fahrenheit (isFahrenheit = true)
  //float f = dht.readTemperature(true);
  Serial.println(t);
  // Check if any reads failed and exit early (to try again).
  if ( isnan(t)) {
    Serial.println(F("Failed to read from DHT sensor!"));
    return;
  }

  // Compute heat index in Fahrenheit (the default)
 // float hif = dht.computeHeatIndex(f, h);
  // Compute heat index in Celsius (isFahreheit = false)
//  float hic = dht.computeHeatIndex(t, h, false);


  //-------------------------------------------------------------
  //send the data to firebase
  //this may need revision
  //in example.ino not sure how firebseData gets initialized


   // if (millis() - sendDataPrevMillis > 15000)
    //{
       // sendDataPrevMillis = millis();
       // count++;

        Serial.println("------------------------------------");
        Serial.println("Set Array...");

        arr.clear();

       
        for (int i =0 ;i<1000;i++)
        {
          String str = "/[";
          str+=String(i);
          str+="]";
           Signal = analogRead(PulseSensorPurplePin);
          arr.set(str,Signal);
           delay(10);
        }  
        if (Firebase.set(firebaseData, path + "/Data", arr))
        {
            Serial.println("PASSED");
            Serial.println("PATH: " + firebaseData.dataPath());
            Serial.println("TYPE: " + firebaseData.dataType());
            Serial.print("VALUE: ");
            printResult(firebaseData);
            Serial.println("------------------------------------");
            Serial.println();
        }
        else
        {
            Serial.println("FAILED");
            Serial.println("REASON: " + firebaseData.errorReason());
            Serial.println("------------------------------------");
            Serial.println();
        }

        Serial.println("------------------------------------");
        Serial.println("Get Array...");
        if (Firebase.get(firebaseData,  path + "/Data"))
        {   
            Serial.println("PASSED");
            Serial.println("PATH: " + firebaseData.dataPath());
            Serial.println("TYPE: " + firebaseData.dataType());
            Serial.print("VALUE: ");
            printResult(firebaseData);               
            Serial.println("------------------------------------");
            Serial.println();
        }
        else
        {
            Serial.println("FAILED");
            Serial.println("REASON: " + firebaseData.errorReason());
            Serial.println("------------------------------------");
            Serial.println();
        }
    //}

//    if (!Firebase.readStream(firebaseData))
//    {
//        Serial.println("------------------------------------");
//        Serial.println("Can't read stream data...");
//        Serial.println("REASON: " + firebaseData.errorReason());
//        Serial.println("------------------------------------");
//        Serial.println();
//    }

    if (firebaseData.streamTimeout())
    {
        Serial.println("Stream timeout, resume streaming...");
        Serial.println();
    }

    if (firebaseData.streamAvailable())
    {
        Serial.println("------------------------------------");
        Serial.println("Stream Data available...");
        Serial.println("STREAM PATH: " + firebaseData.streamPath());
        Serial.println("EVENT PATH: " + firebaseData.dataPath());
        Serial.println("DATA TYPE: " + firebaseData.dataType());
        Serial.println("EVENT TYPE: " + firebaseData.eventType());
        Serial.print("VALUE: ");
        printResult(firebaseData);
        Serial.println("------------------------------------");
        Serial.println();
    }
  try{
     
    Firebase.setFloat(firebaseData, path + "/Temperature", t); //TEMPERATURE data in Celcius
  

  }
   catch(int e){
    Serial.println("ERROR: Exception no: " + e );
  }
}
void printResult(FirebaseData &data)
{

    if (data.dataType() == "int")
        Serial.println(data.intData());
    else if (data.dataType() == "float")
        Serial.println(data.floatData(), 5);
    else if (data.dataType() == "double")
        printf("%.9lf\n", data.doubleData());
    else if (data.dataType() == "boolean")
        Serial.println(data.boolData() == 1 ? "true" : "false");
    else if (data.dataType() == "string")
        Serial.println(data.stringData());
    else if (data.dataType() == "json")
    {
        Serial.println();
        FirebaseJson &json = data.jsonObject();
        //Print all object data
        Serial.println("Pretty printed JSON data:");
        String jsonStr;
        json.toString(jsonStr, true);
        Serial.println(jsonStr);
        Serial.println();
        Serial.println("Iterate JSON data:");
        Serial.println();
        size_t len = json.iteratorBegin();
        String key, value = "";
        int type = 0;
        for (size_t i = 0; i < len; i++)
        {
            json.iteratorGet(i, type, key, value);
            Serial.print(i);
            Serial.print(", ");
            Serial.print("Type: ");
            Serial.print(type == FirebaseJson::JSON_OBJECT ? "object" : "array");
            if (type == FirebaseJson::JSON_OBJECT)
            {
                Serial.print(", Key: ");
                Serial.print(key);
            }
            Serial.print(", Value: ");
            Serial.println(value);
        }
        json.iteratorEnd();
    }
    else if (data.dataType() == "array")
    {
        Serial.println();
        //get array data from FirebaseData using FirebaseJsonArray object
        FirebaseJsonArray &arr = data.jsonArray();
        //Print all array values
        Serial.println("Pretty printed Array:");
        String arrStr;
        arr.toString(arrStr, true);
        Serial.println(arrStr);
        Serial.println();
        Serial.println("Iterate array values:");
        Serial.println();
        for (size_t i = 0; i < arr.size(); i++)
        {
            Serial.print(i);
            Serial.print(", Value: ");

            FirebaseJsonData &jsonData = data.jsonData();
            //Get the result data from FirebaseJsonArray object
            arr.get(jsonData, i);
            if (jsonData.typeNum == FirebaseJson::JSON_BOOL)
                Serial.println(jsonData.boolValue ? "true" : "false");
            else if (jsonData.typeNum == FirebaseJson::JSON_INT)
                Serial.println(jsonData.intValue);
            else if (jsonData.typeNum == FirebaseJson::JSON_FLOAT)
                Serial.println(jsonData.floatValue);
            else if (jsonData.typeNum == FirebaseJson::JSON_DOUBLE)
                printf("%.9lf\n", jsonData.doubleValue);
            else if (jsonData.typeNum == FirebaseJson::JSON_STRING ||
                     jsonData.typeNum == FirebaseJson::JSON_NULL ||
                     jsonData.typeNum == FirebaseJson::JSON_OBJECT ||
                     jsonData.typeNum == FirebaseJson::JSON_ARRAY)
                Serial.println(jsonData.stringValue);
        }
    }
    else
    {
        Serial.println(data.payload());
    }
}





  



  
  

 




 


