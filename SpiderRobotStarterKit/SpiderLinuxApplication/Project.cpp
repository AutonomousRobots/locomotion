#include "terasic_os.h"
#include <pthread.h>
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <time.h>
#include "CSpider.h"
#include "CSpiderLeg.h"
#include "CMotor.h"
#include "BtSppCommand.h"
#include "QueueCommand.h"
#include "PIO_LED.h"
#include "PIO_BUTTON.h"
#include "ADC.h"

#define LEG_RF 0
#define LEG_RM 1
#define LEG_RB 2
#define LEG_LF 3
#define LEG_LM 4
#define LEG_LB 5


int main(int argc, char *argv[]){
	
	printf("===== Group <X> Final Project =====\r\n");

  //####################################
  /*
    DO NOT TAKE THIS SECTION AWAY. SPIDER'S BEHAVIOR
    IS NOT AS EXPECTED WITHOUT THIS INITIALIZATION STEP
  */
  //####################################

  // Spider class
  CSpider Spider;

	printf("Spider Init & Standup\r\n");
	if (!Spider.Init()){
		printf("Spilder Init failed\r\n");
	}else{
		if (!Spider.Standup())
			printf("Spilder Standup failed\r\n");
	}
	Spider.SetSpeed(50);

  //####################################
	//raiseLegs(Spider);
	
	Spider.MoveForward(5);
  // Uncomment to use the ADC class for reading IR sensor
  // ADC adc;
  
  
	

	return 0;
}

void raiseLegs(CSpider Spider){
	Spider.SetJointPosition(LEG_RM,2,-90);
	Spider.SetJointPosition(LEG_LM,2,-90);
}

