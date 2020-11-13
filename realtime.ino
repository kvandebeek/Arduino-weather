#include <Ciao.h>
#include <Adafruit_MCP9808.h>
#include <Adafruit_MPL3115A2.h>
#include <Adafruit_SHT31.h>
#include "Wire.h"

Adafruit_SHT31 sht31 = Adafruit_SHT31();
Adafruit_MCP9808 tempsensor = Adafruit_MCP9808();

#define TCAADDR 0x70

void setup()
{
  Ciao.begin();
  Wire.begin();
  sht31.begin();
  // baro.begin();
  tempsensor.begin();
  delay(5000);
}

void loop()
{ 

  int delaytime = 15000;
  delay(delaytime);
  
  //float baro1=0.00;
  float temp1=0.00;
  //float temp2=0.00;
  float temp3=0.00;
  float humi1=0.00;
  //float baroread=0.00;
  float tempread=0.00;

    tcaselect(5);
    
    delay(delaytime);
    temp1 = sht31.readTemperature();
    delay(delaytime);
    humi1 = sht31.readHumidity();
    delay(delaytime);

    updateDatabase(String(sht31.readTemperature()), "tb1");
    updateDatabase(String(sht31.readHumidity()), "hb1");

        
    //tcaselect(2);
    //delay(delaytime);
    //baroread = (baro.getPressure() / 100) + 10;
        
    //delay(delaytime);
    //tempread = baro.getTemperature();
    
//    if (baroread < 800)
//    {
//      baro1 = 0;
//    }
//    else
//    {
//      temp2 = tempread;
//      baro1 = baroread;
//
//      updateDatabase(String(tempread), "tb2");
//      updateDatabase(String(baroread), "bar");
//    }

    tcaselect(0);
    delay(delaytime);    
    temp3 =  tempsensor.readTempC();
    delay(delaytime);
    updateDatabase(String(tempsensor.readTempC()), "tb3");
 
  updateDatabase(String(temp1), "b_tmp1");
  //updateDatabase(String(temp2), "b_tmp2");
  updateDatabase(String(temp3), "b_tmp3");    
  updateDatabase(String(humi1), "b_hum");
  //updateDatabase(String(baro1), "b_bar1");

  delay(delaytime);
  float avgTemp = (temp1 + temp3) / 2.0000; 
  delay(delaytime); 
  updateDatabase(String(avgTemp), "b_tmpAVG");
}

void tcaselect(uint8_t i) {
  if (i > 7) return;

  Wire.beginTransmission(TCAADDR);
  Wire.write(1 << i);
  Wire.endTransmission();
}

void updateDatabase(String value, String sensor)
{
  char* CONNECTOR_TS PROGMEM = (char*) "rest"; //connector type
  char* SERVER_ADDR_DB PROGMEM = (char*) "8.8.8.8"; //insert correct IP address of server here
  char* METHOD PROGMEM = (char*) "POST";

  String uri = "/url.php?val=";
  uri += value;
  uri += "&sen=";
  uri += String(sensor);

  CiaoData data = Ciao.write(CONNECTOR_TS, SERVER_ADDR_DB, uri, METHOD);
  delay(3000);
}
