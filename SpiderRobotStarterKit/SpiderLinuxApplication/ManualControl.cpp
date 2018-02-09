#include <iostream>
#include <string.h>
#include "terasic_os.h"
#include "CSpider.h"
#include "CSpiderLeg.h"
#include "CMotor.h"
#include "mmap.h"
using namespace std;


bool stringContains(string str, string subStr)
{
  if(str.find(subStr) != string::npos)
    return true;
  else
    return false;
}

int main(int argc, char *argv[])
{
  CSpider Spider;

	printf("Spider Init & Standup\r\n");

	if (!Spider.Init()){
		printf("Spider Init failed\r\n");
	}
  else{
		if (!Spider.Standup())
			printf("Spider Standup failed\r\n");
	}

	Spider.SetSpeed(50);

  printf("\r\n");
	printf("===== Spider Controller =====\r\n");
	printf("Manual Spider Control\r\n");

  printf("Commands:\r\n");
  printf("\tCommand the spider to perform specific action:\r\n");
  printf("\tspider <action>\r\n");
  printf("\t\tActions: reset, fold, extend, grab\r\n");
  printf("\tSet a specific joint:\r\n");
  printf("\tset <joint> <leg> <position>\r\n");
  printf("\t\tJoints: hip, knee, ankle\r\n");
  printf("\t\tLegs: RF=0, RM=1, RB=2, LF=3, LM=4, LB=5\r\n");
  printf("\t\tPosition: Must be within the range (-90, 90)\r\n");
  printf("\r\n");

  string command = "";
	int leg = 0; // Must be 0 to 5
  int joint = 0; // Hip = 0, Knee = 1, Ankle = 2
  int jointDegrees = 0; // Must be -90 to 90

	printf("SpiderController# ");
	cin >> command;
  printf("\r\n");

	while(command != "exit")
	{
    // Spider commands
    if(stringContains(command, "spider"))
    {
      // Get spider action
      cin >> command;

      // Reset - sets the legs to base position
      if(stringContains(command, "reset"))
      {
        printf("\tResetting legs...");
        Spider.SetLegsBase();
        printf("DONE\r\n");
      }

      // Extend - extends knees and ankles
      else if(stringContains(command, "extend"))
      {
        printf("\tExtending legs...");
        Spider.Extend();
        printf("DONE\r\n");
      }

      // Fold - Compactly folds legs for easy storage
      else if(stringContains(command, "fold"))
      {
        printf("\tFolding legs...");
        Spider.Fold();
        printf("DONE\r\n");
      }

      // Grab - Bring together fingertips
      else if(stringContains(command, "grab"))
      {
        printf("\tGrabbing with legs...");
        Spider.Grab();
        printf("DONE\r\n");
      }

      // Invalid
      else{ printf("ERROR - Invalid spider command: %s\r\n", command.c_str()); }
    }


    // Single joint commands
    else if(stringContains(command, "set"))
    {
      // Expecting joint (hip, knee, or ankle)
      cin >> command;

      if(stringContains(command, "hip"))
        joint = 0;
      else if(stringContains(command, "knee"))
        joint = 1;
      else if(stringContains(command, "ankle"))
        joint = 2;


      // Expecting leg ID
      cin >> command;
      leg = atoi(command.c_str());

      if(leg >= 0 && leg <= 5)
      {
        // Expecting degrees (-90 to 90)
        cin >> command;
        jointDegrees = atoi(command.c_str());

        if(jointDegrees >= -90 && jointDegrees <= 90)
        {
          printf("\tSetting leg %i, joint %i to position %i...", leg, joint, jointDegrees);
          Spider.SetJointPosition(leg, joint, jointDegrees);
          printf("DONE\r\n");
        }
        else{ printf("ERROR - Invalid joint position: %i\r\n", jointDegrees); }
      }
      else{ printf("ERROR - Invalid leg: %i\r\n", leg); }
    }

    // Get the next command
    if(command != "exit")
      printf("SpiderController# ");
    cin >> command;
    if(command != "exit")
       printf("\r\n");
	}

	return 0;
}
