# ESP_LoRa_sender

Программный код для отправки ESP32-heltec данных с датчиков через LoRa-модуль.  

Настройки (define на строках 6-24):  
- Частота LoRa-модуля (433МГц)
- Период отправки пакетов в мс (5 сек)
- Типы подключённых датчиков:
-- DS18B20 (+)  
-- DHT_sensor (+)  
-- HW_666  (-)  
-- BMP280_I2C (-)  
-- CJMCU_811  (-)  
-- HTU21D  (-)  
-- TSL2561 (-)  
- Параметры подключённых датчиков:  
-- Номер канала с DS18B20 (GPIO_NUM_4)  
-- Номер канала с DHT (GPIO_NUM_2)  
-- Тип DHT (DHT22)  
-- Номер канала с HW_666 (GPIO_NUM_15)  

Для работы программного кода необходимы следующие библиотеки:  
Heltec_ESP32 от Heltec Automation(TM)  
Adafruit Unified Sensor от Adafruit  
Adafruit BMP280 Library от Adafruit  
Adafruit CCS811 Library от Adafruit  
Adafruit HTU21DF Library от Adafruit  
Adafruit TSL2561 Library от Adafruit  
DHT sensor library от Adafruit  
OneWire от Paul Stoffregen  
ArduinoJson от Benoit Blanchon  
