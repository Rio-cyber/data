#include <WiFi.h>
#include <FirebaseESP32.h>

//digital sensors
#include "DHT.h"

#define DHTPIN 4
#define DHTTYPE DHT11   
DHT dht(DHTPIN, DHTTYPE);


#define FIREBASE_HOST "https://teamrio-backend.firebaseio.com/"
#define FIREBASE_AUTH "aR1Iykct0lM5mZWRzhS7IeqBx8CUggXI2vgkWYWC"
#define WIFI_SSID "GUSEC"
#define WIFI_PASSWORD "asdfg@54321"
#include "time.h"
const char* ntpServer = "pool.ntp.org";
const long  gmtOffset_sec = 0;
const int   daylightOffset_sec = 3600;
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
  String path = "";
  path += timeHour ;
   path += timeMinute ;
    path += timeSec ;
  Serial.println(path2); 
  //--------------------------------------------------------------
  //DHT Temperature Sensor
    // Reading temperature or humidity takes about 250 milliseconds!
  // Sensor readings may also be up to 2 seconds 'old' (its a very slow sensor)
  float h = dht.readHumidity();
  // Read temperature as Celsius (the default)
  float t = dht.readTemperature();
  // Read temperature as Fahrenheit (isFahrenheit = true)
  float f = dht.readTemperature(true);
  Serial.println(t);
  // Check if any reads failed and exit early (to try again).
  if (isnan(h) || isnan(t) || isnan(f)) {
    Serial.println(F("Failed to read from DHT sensor!"));
    return;
  }

  // Compute heat index in Fahrenheit (the default)
  float hif = dht.computeHeatIndex(f, h);
  // Compute heat index in Celsius (isFahreheit = false)
  float hic = dht.computeHeatIndex(t, h, false);


  //-------------------------------------------------------------
  //send the data to firebase
  //this may need revision
  //in example.ino not sure how firebseData gets initialized


  try{
    Firebase.setFloat(firebaseData, path + "/Temperature", t); //TEMPERATURE data in Celcius
  }catch(int e){
    Serial.println("TEMPERATURE ERROR: Exception no: " + e );
  }
  
  try{
    Firebase.setString(firebaseData, path + "/day of the week", timeWeekDay); 
  }catch(int e){
    Serial.println("dayweek ERROR: Exception no: " + e );
  }
  try{
    Firebase.setString(firebaseData, path + "/timehour", timeHour); 
  }catch(int e){
    Serial.println("hour ERROR: Exception no: " + e );
  }
   try{
    Firebase.setString(firebaseData, path + "/timeminute", timeMinute); 
  }catch(int e){
    Serial.println("hour ERROR: Exception no: " + e );
  }
   try{
    Firebase.setString(firebaseData, path + "/timesec", timeSec); 
  }catch(int e){
    Serial.println("hour ERROR: Exception no: " + e );
  }
}
