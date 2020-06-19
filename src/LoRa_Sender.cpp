#include <Arduino.h>
#include <heltec.h>
#include <Wire.h>
#include <ArduinoJson.h>

// Настройки LoRa-модуля
#define BAND 4330E5 // Частота
// Настройки отправки пакета
#define TIME_SEND 5000U  // Период отправки (мс)

// Типы подключенных датчиков
#define DS18B20
#define DHT_sensor
// #define HW_666
// #define BMP280_I2C
// #define CJMCU_811
// #define HTU21D
// #define TSL2561

// Настройки подключения датчиков
#define PIN_DS18B20 GPIO_NUM_4
#define PIN_DHT     GPIO_NUM_2
#define DHTTYPE     DHT22
#define PIN_HW_666  GPIO_NUM_15

// ----- ----- ----- программный код ----- ----- -----

#define COUNTOF(a) (sizeof(a)/sizeof(a[0]))

void create_packet();
void send_packet();
void create_sensor();

uint64_t chipid;
StaticJsonDocument<1024> json;
char buf[1024];
char str[12];

struct {
    bool ds18b20 = false;
    bool dht     = false;
    bool hw666   = false;
    bool bmp280  = false;
    bool CCS811  = false;
    bool htu21d  = false;
    bool tsl2561 = false;
}imput_modyle;

#ifdef DS18B20
#include <Temp_DS18B20.h>
Temp_DS18B20 ds(PIN_DS18B20);
bool include_DS18B20();
void add_in_DS18B20();
#endif // DS18B20

#ifdef DHT_sensor
#include <DHT.h>
#include <DHT_U.h>
DHT_Unified dht(PIN_DHT, DHTTYPE);
bool include_DHT();
void add_in_DHT();
#endif // DHT_sensor

#ifdef HW_666
float getVPP();
bool include_HW_666();
void add_in_HW_666();
#endif // HW_666

#ifdef BMP280_I2C
#include <Adafruit_BMP280.h>
Adafruit_BMP280 bmp;
bool include_BMP280_I2C();
void add_in_BMP280_I2C();
#endif // BMP280_I2C

#ifdef CJMCU_811
#include <Adafruit_CCS811.h>
Adafruit_CCS811 ccs;
bool include_CJMCU_811();
void add_in_CJMCU_811();
#endif // CJMCU_811

#ifdef HTU21D
#include <Adafruit_HTU21DF.h>
Adafruit_HTU21DF htu;
bool include_HTU21D();
void add_in_HTU21D();
#endif // HTU21D

#ifdef TSL2561
#include <Adafruit_TSL2561_U.h>
Adafruit_TSL2561_Unified tsl(TSL2561_ADDR_FLOAT, 12345);
bool include_TSL2561();
void add_in_TSL2561();
#endif // TSL2561



void setup() {
    Heltec.begin(false /*DisplayEnable Disable*/, true /*Heltec.LoRa Enable*/, false /*Serial Enable*/, false /*PABOOST Enable*/, BAND /*long BAND*/);
    chipid = ESP.getEfuseMac(); // MAC адрес ESP32 (длина: 6 байтов)

    Wire.begin(21, 22);
    //* Вывод Serial
    Serial.begin(9600);
    while(!Serial) {}
#if defined( WIFI_LoRa_32 )
    Serial.println("Hi, WIFI_LoRa_32!");
#endif
#if defined( WIFI_LoRa_32_V2 )
    Serial.println("Hi, WIFI_LoRa_32_V2!");
#endif
    //*/

    // Инициализация датчиков
#ifdef DS18B20
    imput_modyle.ds18b20 = include_DS18B20();
#endif // DS18B20

#ifdef DHT_sensor
    imput_modyle.dht = include_DHT();
#endif // DHT_sensor

#ifdef HW_666
    imput_modyle.hw666 = include_HW_666();
#endif // HW_666

#ifdef BMP280_I2C
    imput_modyle.bmp280 = include_BMP280_I2C();
#endif // BMP280_I2C

#ifdef CJMCU_811
    imput_modyle.CCS811 = include_CJMCU_811();
#endif // CJMCU_811

#ifdef HTU21D
    imput_modyle.htu21d = include_HTU21D();
#endif // HTU21D

#ifdef TSL2561
    imput_modyle.tsl2561 = include_TSL2561();
#endif // TSL2561

    delay(1000);
    Serial.println("Start!");

    // Отправка пакета добавления датчика на облако (5 раз из-за возможных потерь)
    for (int i = 0; i < 5; ++i) {
        create_sensor();
    }
}






uint16_t num = 0;

void loop() {
    static unsigned long timepoint = millis();
	if ((millis() - timepoint) > TIME_SEND) {
        // Периодическая часть алгоритма (раз в TIME_SEND мс)
		timepoint = millis();

        // Считывание и отправка данных модуля
#ifdef DS18B20
        if(imput_modyle.ds18b20)
            add_in_DS18B20();
#endif // DS18B20

#ifdef DHT_sensor
        if(imput_modyle.dht)
            add_in_DHT();
#endif // DHT_sensor

#ifdef HW_666
        if(imput_modyle.hw666)
            add_in_HW_666();
#endif // HW_666

#ifdef BMP280_I2C
        if(imput_modyle.bmp280)
            add_in_BMP280_I2C();
#endif // BMP280_I2C

#ifdef CJMCU_811
        if(imput_modyle.CCS811)
            add_in_CJMCU_811();
#endif // CJMCU_811

#ifdef HTU21D
        if(imput_modyle.htu21d)
            add_in_HTU21D();
#endif // HTU21D

#ifdef TSL2561
        if(imput_modyle.tsl2561)
            add_in_TSL2561();
#endif // TSL2561

    }
}







void create_sensor() {
    create_packet();
    int sensor_count = 0;
    if(imput_modyle.ds18b20) {
        json["sensor"].addElement();
        json["sensor"][sensor_count] = "DS18B20";
        ++sensor_count;
    }
    if(imput_modyle.dht) {
        json["sensor"].addElement();
        json["sensor"][sensor_count] = "DHT";
        ++sensor_count;
    }
    if(imput_modyle.hw666) {
        json["sensor"].addElement();
        json["sensor"][sensor_count] = "HW-666";
        ++sensor_count;
    }
    if(imput_modyle.bmp280) {
        json["sensor"].addElement();
        json["sensor"][sensor_count] = "GY-BMP280";
        ++sensor_count;
    }
    if(imput_modyle.CCS811) {
        json["sensor"].addElement();
        json["sensor"][sensor_count] = "CJMCU-811";
        ++sensor_count;
    }
    if( imput_modyle.htu21d) {
        json["sensor"].addElement();
        json["sensor"][sensor_count] = "HTU21D";
        ++sensor_count;
    }
    if(imput_modyle.tsl2561) {
        json["sensor"].addElement();
        json["sensor"][sensor_count] = "TSL2561";
        ++sensor_count;
    }
    send_packet();
}


void create_packet() {
    LoRa.beginPacket();
    LoRa.setTxPower(14, RF_PACONFIG_PASELECT_PABOOST);

    snprintf_P(str, COUNTOF(str), PSTR("%04X%08X"), (uint16_t)(chipid>>32), (uint32_t)chipid);
    str[0] = '4';
    json["MACaddr"] = str;
    json["num"] = num;
}

void send_packet() {
    serializeJsonPretty(json, buf);

    // int count = strlen(buf);
    // Serial.print('|');
    // Serial.print(count);
    // Serial.print('|');
    Serial.print(buf);

    LoRa.print(buf);
    LoRa.endPacket();

    json.clear();
    ++num;
    delay(150);
}





#ifdef DS18B20
bool include_DS18B20() {
    if(ds.search())
        return false;
    ds.request();
    return true;
}

void add_in_DS18B20() {
    create_packet();

    ds.response(true);
    json["DS18B20"]["temp"] = ds.get_temp_c();  //C
    ds.request();

    send_packet();
}
#endif // DS18B20


#ifdef DHT_sensor
bool include_DHT() {
    dht.begin();
    return true;
}

void add_in_DHT() {
    bool send = false;
    sensors_event_t event;
    dht.temperature().getEvent(&event);
    if (!isnan(event.temperature)) {
        json["DHT"]["temp"] = event.temperature; //C
        send = true;
    }
    dht.humidity().getEvent(&event);
    if (!isnan(event.relative_humidity)) {
        json["DHT"]["humidity"] = event.relative_humidity; //%
        send = true;
    }
    if(send) {
        create_packet();

        send_packet();
    }
}
#endif // DHT_sensor


#ifdef HW_666
bool include_HW_666() {
    pinMode(PIN_HW_666, INPUT);
    return true;
}

float getVPP() {
    float result;
    int readValue;
    int maxValue = 0;
    uint32_t start_time = millis();
    while ((millis() - start_time) < 1000)
    {
        readValue = analogRead(PIN_HW_666);
        if (readValue > maxValue) {
            maxValue = readValue;
        }
    }
    result = (maxValue * 3.3 * 1000) / 2048.0;
    return result;
};

void add_in_HW_666() {
    create_packet();

    json["HW-666"]["voltage"] = getVPP(); //V

    send_packet();
}
#endif // HW_666


#ifdef BMP280_I2C
bool include_BMP280_I2C() {
    if (!bmp.begin(0x76)) 
        return false;
    return true;
}

void add_in_BMP280_I2C() {
    create_packet();

    json["GY-BMP280"]["temp"] = bmp.readTemperature();  //C
    json["GY-BMP280"]["pressure"] = bmp.readPressure()/1000; //kPa

    send_packet();
}
#endif // BMP280_I2C


#ifdef CJMCU_811
bool include_CJMCU_811() {
    if(!ccs.begin())
        return false;
    while(!ccs.available());
    return true;
}

void add_in_CJMCU_811() {
    if(ccs.available()){
        if(!ccs.readData()){
            create_packet();

            json["CJMCU-811"]["eCO2"] = ccs.geteCO2();  // ppm
            json["CJMCU-811"]["nTVOC"] = ccs.getTVOC(); // ppb

            send_packet();
        }
    }

}
#endif // CJMCU_811


#ifdef HTU21D
bool include_HTU21D() {
    if (!htu.begin())
        return false;
    return true;
}

void add_in_HTU21D() {
    create_packet();

    json["HTU21D"]["temp"] = htu.readTemperature();  //C
    json["HTU21D"]["humidity"] = htu.readHumidity(); //%

    send_packet();
}
#endif // HTU21D


#ifdef TSL2561
bool include_TSL2561() {
    if(!tsl.begin())
        return false;
    return true;
}

void add_in_TSL2561() {
    sensors_event_t event;
    tsl.getEvent(&event);
    if (event.light) {
        create_packet();

        json["TSL2561"]["light"] = event.light;  //lux

        send_packet();
    }
}
#endif // TSL2561
