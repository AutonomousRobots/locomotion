#include "terasic_os.h"
#include <pthread.h>
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include "CSpider.h"
#include "CSpiderLeg.h"
#include "CMotor.h"
#include "BtSppCommand.h"
#include "mmap.h"
#include "QueueCommand.h"
#include "PIO_LED.h"
#include "PIO_BUTTON.h"
#include <time.h>


typedef enum{
	CMD_AT,
	CMD_FORDWARD,
	CMD_BACKWARD,
	CMD_TURN_RIHGT,
	CMD_TURN_LEFT,
	CMD_TURN_RIHGT_DGREE,
	CMD_TURN_LEFT_DGREE,
	CMD_STOP,
	CMD_SPPED,
	CMD_TILTL,
	CMD_TILTR,
	CMD_TILTF,
	CMD_TILTB,
	CMD_TILTN,
	CMD_Query_Version,
	CMD_JOYSTICK,
	CMD_ALL,
	CMD_IDLE,
}COMMAND_ID;


static void *bluetooth_spp_thread(void *ptr)
{

	CBtSppCommand BtSppCommand;
	CQueueCommand *pQueueCommand;
	int Command, Param;
	pQueueCommand = (CQueueCommand *)ptr;
	printf("[BT]Start Service\r\n");
	BtSppCommand.RegisterService();
	while(1){
		printf("[BT]Lisen...\r\n");
		BtSppCommand.RfcommOpen();
		printf("[BT]Connected...\r\n");
		while(1){
			Command = BtSppCommand.CommandPolling(&Param);
			if (Command != CMD_IDLE){
				// push command to command queue
				if (Command == CMD_STOP)
				   pQueueCommand->Clear();
				// push command to command queue 
				if (!pQueueCommand->IsFull()){
				   pQueueCommand->Push(Command, Param);
				    }
				/*if (!pQueueCommand->IsFull()){
					pQueueCommand->Push(Command, Param);
				}*/
			}
		}
		printf("[BT]Disconneected...\r\n");
		BtSppCommand.RfcommClose();
	}

//	pthread_exit(0); /* exit */
	return 0;
}

int main(int argc, char *argv[]){

    CSpider Spider;
    CQueueCommand QueueCommand;
    int Command, Param;
    bool bSleep = false;
    CPIO_LED LED_PIO;
    CPIO_BUTTON BUTTON_PIO;
    pthread_t id0;
    int ret0;
    uint32_t LastActionTime;
    const uint32_t MaxIdleTime = 10*60*OS_TicksPerSecond(); // spider go to sleep when exceed this time
//    const uint32_t MaxIdleTime = 10*60*OS_TicksPerSecond(); // spider go to sleep when exceed this time

	printf("===== Spider Demo =====\r\n"); 
 

	printf("Spider Init & Standup\r\n");
	if (!Spider.Init()){
		printf("Spilder Init failed\r\n");
	}else{
		if (!Spider.Standup())
			printf("Spilder Standup failed\r\n");
	}
	Spider.SetSpeed(50);

	// 
	printf("Create Bluetooth Thread\r\n");
	ret0=pthread_create(&id0,NULL,bluetooth_spp_thread, (void *)&QueueCommand);
	if(ret0!=0){
		printf("Creat pthread-0 error!\n");
		//exit(1);	
	}	
	printf("Listen Command...\r\n");
	  LED_PIO.SetLED(0x7f);
	  LastActionTime = OS_GetTickCount();
		while(1)
		{	
			//sleep mode detect.
			if(!bSleep && ((OS_GetTickCount()-LastActionTime) > MaxIdleTime))
			{
				bSleep = true;
				LastActionTime = OS_GetTickCount();
				Spider.Sleep();
				LED_PIO.SetLED(0x1);
			}
			 
			if(BUTTON_PIO.GetBUTTON()==0x2)
			{
				if (bSleep){
					bSleep = false;
					Spider.WakeUp();
					LED_PIO.SetLED(0x7f);
				}	
				Spider.Reset();
				LastActionTime = OS_GetTickCount();
			}
			else if (BUTTON_PIO.GetBUTTON()==0x1)
			{
				if (bSleep){
					bSleep = false;
					Spider.WakeUp();
					LED_PIO.SetLED(0x7f);
				}	
				Spider.DEMO_Dance(1);
				LastActionTime = OS_GetTickCount();
			}

			if(!QueueCommand.IsEmpty() && QueueCommand.Pop(&Command, &Param) ){
 				 if (bSleep){
	 					bSleep = false;
				    Spider.WakeUp();
				    LED_PIO.SetLED(0x7f);
				  }  

				switch(Command){
					case CMD_FORDWARD:
						printf("CMD_FORDWARD\n");
						Spider.MoveForward(1);
						break;
					case CMD_BACKWARD:
						printf("CMD_BACKWARD\n");
						Spider.MoveBackward(1);
						break;
					case CMD_TURN_RIHGT:
						printf("CMD_TURN_RIHGT\n");
						Spider.RotatelRight(1);
						break;
					case CMD_TURN_LEFT:
						printf("CMD_TURN_LEFT\n");
						Spider.RotatelLeft(1);
						break;
					case CMD_TILTL:
						printf("CMD_TILTL\n");
						Spider.TiltLeft();
						break;
					case CMD_TILTR:
						printf("CMD_TILTR\n");
						Spider.TiltRight();
						break;
					case CMD_TILTF:
						printf("CMD_TILTF\n");
						Spider.TiltForward();
						break;
					case CMD_TILTB:
						printf("CMD_TILTB\n");
						Spider.TiltBackward();
						break;
					case CMD_TILTN:
						printf("CMD_TILTN\n");
						Spider.TiltNone();
						break;
					case CMD_STOP:
						printf("CMD_STOP\n");
						Spider.Reset();
						break;
					case CMD_SPPED:
						printf("CMD_SPPED %d \n",Param);
						Spider.SetSpeed(Param);
						break;
					case CMD_TURN_LEFT_DGREE:
						printf("CMD_TURN_LEFT_DGREE %d \n",Param);
						break;
					case CMD_TURN_RIHGT_DGREE:
						printf("CMD_TURN_RIHGT_DGREE %d \n",Param);
						break;
					case CMD_Query_Version:
						printf("CMD_Query_Version\n");
						break;
					case CMD_JOYSTICK:
						printf("CMD_JOYSTICK (Param=%d)\n",Param);
						break;
					case CMD_ALL:
						printf("CMD_ALL\n");
						Spider.DEMO_Dance(1);
						QueueCommand.Clear();
						break;
					default:printf("Nothing\n");
						break;
				} // switch
				LastActionTime = OS_GetTickCount();
			    //printf("rx:%xh\r\n", Data8);
			}

		}



	return 0;
}	
