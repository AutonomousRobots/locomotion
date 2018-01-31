#
SPIDER = AutoProject1
CONTROLLER = Controller
ADC = ADC
ES_FINAL_PROJECT = CustomSpiderProject

# add 'export SOCEDS_DEST_ROOT=~/altera/14.0/embedded' in /etc/profile
#
 
ALT_DEVICE_FAMILY       = soc_cv_av

CROSS_COMPILE = arm-linux-gnueabihf-
#CFLAGS = -g -Wall -static -std=gnu++11 -I ${SOCEDS_DEST_ROOT}/ip/altera/hps/altera_hps/hwlib/include -I./bt/inc
CFLAGS = -g -Wall -std=gnu++11 -I ${SOCEDS_DEST_ROOT}/ip/altera/hps/altera_hps/hwlib/include -I./bt/inc -I${SOCEDS_DEST_ROOT}/ip/altera/hps/altera_hps/hwlib/include/$(ALT_DEVICE_FAMILY) -D$(ALT_DEVICE_FAMILY)
LDFLAGS =  -g -Wall  -lstdc++ -L./bt/lib -lbluetooth -lrt -lpthread
#CC = $(CROSS_COMPILE)gcc
CC = $(CROSS_COMPILE)g++
ARCH= arm

all: $(SPIDER) $(CONTROLLER) $(ADC) $(CUSTOM_SPIDER_PROJECT)

controller:

$(SPIDER): Project.o CSpider.o CSpiderLeg.o CMotor.o terasic_os.o mmap.o BtSppCommand.o BtSPP.o  Queue.o QueueCommand.o PIO_LED.o PIO_BUTTON.o
	$(CC) $(LDFLAGS)   $^ -o $@

$(CONTROLLER): ManualControl.o CSpider.o CSpiderLeg.o CMotor.o terasic_os.o mmap.o BtSppCommand.o BtSPP.o  Queue.o QueueCommand.o PIO_LED.o PIO_BUTTON.o
	$(CC) $(LDFLAGS)   $^ -o $@

$(ADC): ADCTest.o ADC.o CSpider.o CSpiderLeg.o CMotor.o terasic_os.o mmap.o BtSppCommand.o BtSPP.o  Queue.o QueueCommand.o PIO_LED.o PIO_BUTTON.o
	$(CC) $(LDFLAGS)   $^ -o $@

$(CUSTOM_SPIDER_PROJECT): Project.o ADC.o CSpider.o CSpiderLeg.o CMotor.o terasic_os.o mmap.o BtSppCommand.o BtSPP.o  Queue.o QueueCommand.o PIO_LED.o PIO_BUTTON.o
	$(CC) $(LDFLAGS)   $^ -o $@

%.o : %.cpp
	$(CC) $(CFLAGS) -c $< -o $@

.PHONY: clean
clean:
	rm -f $(SPIDER) $(CONTROLLER) $(ADC) $(CUSTOM_SPIDER_PROJECT)  *.a *.o *~
