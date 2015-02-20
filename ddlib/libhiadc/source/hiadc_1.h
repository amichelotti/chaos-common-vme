
void hiadc_init(unsigned long baseAddress);
void hiadc_acquire(unsigned long baseAddress, unsigned short* channels, int maxWait,long *to, long *or);

void readADC(unsigned long *baseAddress, long *maxWait, TD1Hdl rawData, long *to, long *or);
