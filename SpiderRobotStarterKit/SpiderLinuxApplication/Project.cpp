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
	Spider.SetSpeed(10);

  //####################################
	
	
	// Spider.sixLeggedStatic(4);
	// Spider.MoveForward(5);
	// Uncomment to use the ADC class for reading IR sensor
	// ADC adc;
	
	Spider.sixLeggedDynamic(20);
	Spider.Reset();
	// Spider.sixLeggedDynamic(10);
	// Spider.reset();
	// Spider.raiseLegs();
	// Spider.fourLeggedStatic(10);
	// Spider.reset();
	// Spider.fourLeggedDynamic(10);
	// Spider.reset();
	

	return 0;
}




