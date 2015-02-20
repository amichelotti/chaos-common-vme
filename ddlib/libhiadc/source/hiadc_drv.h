
/**
   initialize hiadc
   @param baseaddress board base address
   @param [out] channels values
   @param time_ms time out in ms (0=no timeout)
   @param to timeout arised
   @param or oversampling arised
 */
void hiadc_init(unsigned long baseAddress);

/**
   acquire values
   @param baseaddress board base address
   @param [out] channels values
   @param time_ms time out in ms (0=no timeout)
   @param to timeout arised
   @param or oversampling arised
 */
void hiadc_acquire(unsigned long baseAddress, unsigned short* channels, int time_ms,long *to, long *or);

/**
   acquire values (LV version)
   @param baseaddress board base address
   @param [out] channels values
   @param time_ms time out in ms (0=no timeout)
   @param to timeout arised
   @param or oversampling arised
 */
void readADC(unsigned long *baseAddress, long *maxWait, void* rawData, long *to, long *or);
