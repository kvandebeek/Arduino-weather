#include <OneWire.h>
#include <DallasTemperature.h>
#include <TSL2561.h>
#include <Adafruit_SI1145.h>
#include <Adafruit_MCP9808.h>
#include <Adafruit_MPL3115A2.h>
#include <Adafruit_SHT31.h>
#include "Wire.h"
#include <Ciao.h>
#include "DHT.h"

Adafruit_SHT31 sht31 = Adafruit_SHT31();
Adafruit_MPL3115A2 baro = Adafruit_MPL3115A2();
Adafruit_MCP9808 tempsensor = Adafruit_MCP9808();
Adafruit_SI1145 uv = Adafruit_SI1145();
TSL2561 tsl(TSL2561_ADDR_FLOAT);

#define DHTPIN 3
#define DHTTYPE DHT22   // DHT 22  (AM2302), AM2321

#define TCAADDR 0x70
#define ONE_WIRE_BUS_A 4
OneWire oneWireA(ONE_WIRE_BUS_A);
DHT temp_serreC(DHTPIN, DHTTYPE);

void tcaselect(uint8_t i) {
  if (i > 7) return;

  Wire.beginTransmission(TCAADDR);
  Wire.write(1 << i);
  Wire.endTransmission();
}

void setup()
{
  Ciao.begin();
  Wire.begin();
  sht31.begin();
  baro.begin();
  uv.begin();
  tsl.begin();
  baro.getPressure();
  tempsensor.begin();
  temp_serreC.begin();
}

void loop()
{
  int delaytime = 2500;
  
  float temp1 = 0.00;
  float temp2 = 0.00;
  float temp3 = 0.00;
  float humi1 = 0.00;
  int baro1 = 0;
  float baroread = 0.00;
  float tempread = 0.00;

  //DAN DE DHT22 IN DE SERRE
  delay(delaytime);
  float humidity_serreC = temp_serreC.readHumidity();
  delay(delaytime);
  humidity_serreC = temp_serreC.readHumidity();
  delay(delaytime);
  float temperature_serreC  = temp_serreC.readTemperature();
  delay(delaytime);
  temperature_serreC  = temp_serreC.readTemperature();
  delay(delaytime);
  float heatindex_serreC = temp_serreC.computeHeatIndex(humidity_serreC, temperature_serreC, false);

  updateDatabase(String(humidity_serreC), "hs1");
  delay(delaytime);
  updateDatabase(String(temperature_serreC), "ts1");  

  for (int i = 0; i < 10; i++)
  {
    tcaselect(5);
    temp1 += sht31.readTemperature();
    delay(delaytime);
    humi1 += sht31.readHumidity();
    delay(delaytime);

    updateDatabase(String(sht31.readTemperature()), "tb1");
    delay(delaytime);
    updateDatabase(String(sht31.readHumidity()), "hb1");  
    delay(delaytime);
        
    tcaselect(2);
    baroread = (baro.getPressure() / 100) + 10;
    delay(delaytime);
    tempread = baro.getTemperature();
    delay(delaytime);
    
    if (baroread < 800)
    {
      baro1 += 0;
    }
    else
    {
      temp2 += tempread;
      baro1 += baroread;

      updateDatabase(String(tempread), "tb2");
      delay(delaytime);
      updateDatabase(String(baroread), "bar");
      delay(delaytime);
    }

    tcaselect(0);
    delay(delaytime);
    temp3 +=  tempsensor.readTempC();
    delay(delaytime);

    updateDatabase(String(tempsensor.readTempC()), "tb3");
    delay(delaytime);
 
    tcaselect(6);
    delay(delaytime);
  }

  temp1 /= 10.000;
  humi1 /= 10.000;
  temp2 /= 10.000;
  baro1 /= 10.000;
  temp3 /= 10.000;

  tcaselect(7);
  delay(delaytime);
  tsl.setGain(TSL2561_GAIN_0X);
  delay(delaytime);
  tsl.setTiming(TSL2561_INTEGRATIONTIME_13MS);
  delay(delaytime);
  uint32_t lum = tsl.getFullLuminosity();
  uint16_t ir, full;
  int lux;
  ir = lum >> 16;
  full = lum & 0xFFFF;
  lux = tsl.calculateLux(full, ir);
  delay(delaytime);

  float avgTemp = (5 * temp1 + (temp2) + (4.000 * temp3)) / 10.000;

  updateDatabase(String(avgTemp), "tba");  
  updateDatabase(String(temp1), "b_tmp1");
  updateDatabase(String(humi1), "b_hum");
  updateDatabase(String(temp2), "b_tmp2");
  updateDatabase(String(baro1), "b_bar1");
  updateDatabase(String(temp3), "b_tmp3");

  updateDatabase(String(avgTemp), "b_tmpAVG");
  updateDatabase(String(humidity_serreC), "muh");
  updateDatabase(String(temperature_serreC), "gva");
  updateDatabase(String(heatindex_serreC), "hi");
}

void updateDatabase(String value, String sensor)
{
  char* CONNECTOR_TS = (char*) "rest"; //connector type
  char* SERVER_ADDR_DB  = (char*) "8.8.8.8"; //change this to the correct URL
  char* METHOD = (char*) "POST";

  String uri = "/url.php?val=";
  uri += value;
  uri += "&sen=";
  uri += String(sensor);

  CiaoData data = Ciao.write(CONNECTOR_TS, SERVER_ADDR_DB, uri, METHOD);
  delay(2500);
}
