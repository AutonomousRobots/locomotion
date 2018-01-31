#include "ADC.h"

CMMap* ADC::m_mmap=NULL;

ADC::ADC()
{
	// Setup MMAP
	m_mmap = new CMMap(CMMap::MAP_H2F_LW_BRIDGE);

	// Triggers the first update cycle
	m_mmap->Reg32_Write(ADC_0_BASE, 0, 0x0);

	// Enable auto-update at bit 16 in register 1
	m_mmap->Reg32_Write(ADC_0_BASE, 4, 0x00010000);
};

ADC::~ADC() {
	// TODO Auto-generated destructor stub
		delete m_mmap;
}

uint32_t ADC::GetChannel(int channel)
{
	return m_mmap->Reg32_Read(ADC_0_BASE, channel*4);
}
