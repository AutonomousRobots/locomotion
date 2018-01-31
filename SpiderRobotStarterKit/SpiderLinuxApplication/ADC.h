#ifndef ADC_H_
#define ADC_H_

#include "terasic_os.h"
#include "mmap.h"
#include "hps_0.h"

class ADC{

public:
	static CMMap *m_mmap;
	ADC();
	virtual ~ADC();

	uint32_t GetChannel(int channel);
};



#endif /* ADC_H_ */
