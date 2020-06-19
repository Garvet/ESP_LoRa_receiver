#ifndef TEMP_DS18B20_H
#define TEMP_DS18B20_H
#include <Arduino.h>
#include <OneWire.h>

#define FIRST_BIT_SENSOR 0x28

enum temp_error {
    NO_ERROR,
    NO_SENSOR,
    NO_ADDR,
    EARLY_READING,
    INVALID_CRC,
    SENSOR_NOT_DS18B20
};

class Temp_DS18B20 {
    uint8_t pin;
    uint8_t addr[8];
    uint8_t data[9];
    OneWire  *ds;
    float celsius;
    unsigned long request_time;

    uint8_t check_addr();
    uint8_t check_addr(uint8_t *addr);
public:
    Temp_DS18B20(uint8_t pin);
    Temp_DS18B20(uint8_t pin, uint8_t *addr);
    ~Temp_DS18B20();

    /// ---OneWire---

    uint8_t request();   // Считывание температуры датчиком
    uint8_t response(bool ignore_timer = false);  // Получение t с датчика, расчёт в C
    uint8_t take();      // request и через секунду response
    uint8_t search();    // Поиск устройства

    /// ---Set&Get---
    
    float get_temp_c(); // Получение температуры C
    float get_temp_f(); // Получение температуры F
    uint8_t set_addr(uint8_t *new_addr); // Задать адрес датчика
    void get_addr(uint8_t *addr);        // Считать адрес датчика
    uint8_t get_pin();  // Считать номер линии
};

#endif