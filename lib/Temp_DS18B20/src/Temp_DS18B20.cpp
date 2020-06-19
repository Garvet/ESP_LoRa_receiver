#include "Temp_DS18B20.h"

uint8_t Temp_DS18B20::check_addr() {
    return check_addr(this->addr);
}

uint8_t Temp_DS18B20::check_addr(uint8_t *addr) {
    if (OneWire::crc8(addr, 7) != addr[7])
        return INVALID_CRC;
    if (addr[0] != FIRST_BIT_SENSOR)
        return SENSOR_NOT_DS18B20;
    return NO_ERROR;
}

Temp_DS18B20::Temp_DS18B20(uint8_t pin) {
    this->pin = pin;
    request_time = 0;
    ds = new OneWire(this->pin);
}

Temp_DS18B20::Temp_DS18B20(uint8_t pin, uint8_t *addr) {
    this->pin = pin;
    request_time = 0;
    ds = new OneWire(this->pin);
    for (uint8_t i = 0; i < 8; ++i)
        this->addr[i] = addr[i];
}
Temp_DS18B20::~Temp_DS18B20() {
    delete (ds);
}

/// ---OneWire---

// Считывание температуры датчиком
uint8_t Temp_DS18B20::request() {
    if (addr[0] != FIRST_BIT_SENSOR)
        return NO_ADDR;
    if ((millis() - request_time) > 1000U)
        response();
    ds->reset();
    ds->select(addr);
    ds->write(0x44);
    request_time = millis();
    return NO_ERROR;
}

// Получение t с датчика, расчёт в C
uint8_t Temp_DS18B20::response(bool ignore_timer) {
    uint8_t err;
    if (!ignore_timer)
        if ((millis() - request_time) < 1000U)
            return EARLY_READING;
    err = ds->reset();
    if (err == 0)
        return NO_SENSOR;
    ds->select(addr);    
    ds->write(0xBE);
    for ( uint8_t i = 0; i < 9; i++) {           
        data[i] = ds->read();
    }
    if (OneWire::crc8(data, 8) != data[8])
        return INVALID_CRC;
    celsius = (float)((data[1] << 8) | data[0]) / 16.0;
    return NO_ERROR;
}

// request и через секунду response
uint8_t Temp_DS18B20::take() {
    uint8_t err;
    err = request();
    if (err != NO_ERROR) 
        return err;
    delay(1000);
    err = response();
    if (err != NO_ERROR)
        return err;
    return NO_ERROR;
}

// Поиск устройства
uint8_t Temp_DS18B20::search() {
    uint8_t search_addr[8];
    bool all = false;
    while(1){
        if ( ds->search(search_addr) != 0) {
            if (check_addr(search_addr) == NO_ERROR) {
                set_addr(search_addr);
                return NO_ERROR;
            }
        }
        else {
            if (all)
                return NO_SENSOR;
            all = true;
        }
    }
}

/// ---Set&Get---

// Получение температуры C
float Temp_DS18B20::get_temp_c(){
    return celsius;
}
// Получение температуры F
float Temp_DS18B20::get_temp_f(){
    return (celsius * 1.8 + 32.0);
}

// Задать адрес датчика
uint8_t Temp_DS18B20::set_addr(uint8_t *new_addr) {
    uint8_t err;
    err = check_addr(new_addr);
    if(err != 0) 
        return err;
    for (uint8_t i = 0; i < 8; ++i)
        addr[i] = new_addr[i];
    return NO_ERROR;
}

// Считать адрес датчика
void Temp_DS18B20::get_addr(uint8_t *addr) {
    for (uint8_t i = 0; i < 8; ++i)
        addr[i] = this->addr[i];
}

// Считать номер линии
uint8_t Temp_DS18B20::get_pin() {
    return pin;
}