#include <bitset>
#include <iostream>
#include "ADC.h"

int main(int argc, char *argv[]){

	ADC adc;

	printf("===== Spider ADC Test =====\r\n");

	uint32_t sensorReading0 = 0;
  uint32_t sensorReading1 = 0;
  uint32_t sensorReading2 = 0;
  uint32_t sensorReading3 = 0;
  uint32_t sensorReading4 = 0;
  uint32_t sensorReading5 = 0;
  uint32_t sensorReading6 = 0;
  uint32_t sensorReading7 = 0;

	while(1)
	{
		sensorReading0 = adc.GetChannel(0);
		printf("Ch0 Sensor Reading: %u\r\n", sensorReading0);
    // std::bitset<32> sensorReadingBinary0(sensorReading0);
    // std::cout << "Register0: " << sensorReadingBinary0 << "\n";

		sensorReading1 = adc.GetChannel(1);
		printf("Ch1 Sensor Reading: %u\r\n", sensorReading1);
    // std::bitset<32> sensorReadingBinary1(sensorReading1);
    // std::cout << "Register1: " << sensorReadingBinary1 << "\n";

		sensorReading2 = adc.GetChannel(2);
		printf("Ch2 Sensor Reading: %u\r\n", sensorReading2);
    // std::bitset<32> sensorReadingBinary2(sensorReading2);
    // std::cout << "Register2: " << sensorReadingBinary2 << "\n";

		sensorReading3 = adc.GetChannel(3);
		printf("Ch3 Sensor Reading: %u\r\n", sensorReading3);
    // std::bitset<32> sensorReadingBinary3(sensorReading3);
    // std::cout << "Register3: " << sensorReadingBinary3 << "\n";

		sensorReading4 = adc.GetChannel(4);
		printf("Ch4 Sensor Reading: %u\r\n", sensorReading4);
    // std::bitset<32> sensorReadingBinary4(sensorReading4);
    // std::cout << "Register4: " << sensorReadingBinary4 << "\n";

		sensorReading5 = adc.GetChannel(5);
		printf("Ch5 Sensor Reading: %u\r/\n", sensorReading5);
    // std::bitset<32> sensorReadingBinary5(sensorReading5);
    // std::cout << "Register5: " << sensorReadingBinary5 << "\n";

		sensorReading6 = adc.GetChannel(6);
		printf("Ch6 Sensor Reading: %u\r\n", sensorReading6);
    // std::bitset<32> sensorReadingBinary6(sensorReading6);
    // std::cout << "Register6: " << sensorReadingBinary6 << "\n";

		sensorReading7 = adc.GetChannel(7);
		printf("Ch7 Sensor Reading: %u\r\n", sensorReading7);
    // std::bitset<32> sensorReadingBinary7(sensorReading7);
    // std::cout << "Register7: " << sensorReadingBinary7 << "\n";

    printf("\r\n");
    printf("\r\n");

    sleep(1);
	}

	return 0;
}
