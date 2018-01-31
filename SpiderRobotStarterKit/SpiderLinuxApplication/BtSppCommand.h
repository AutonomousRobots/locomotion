#ifndef BT_SPP_COMMAND_H_
#define BT_SPP_COMMAND_H_

#include "BtSPP.h"

class CBtSppCommand: public CBtSpp {
public:
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
		CMD_IDLE
	}COMMAND_ID;

protected:
	bool CommandParsing(char *pCommand, int *pCommandID, int *pParam);
	bool CommandRead(char *pBuffer, int nBufferSize, int *prx_lenmand);
	

public:
	CBtSppCommand();
	virtual ~CBtSppCommand();
	
	int CommandPolling(int *pParam);


};

#endif /* BT_SPP_COMMAND_H_ */
