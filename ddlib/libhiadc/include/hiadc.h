#ifndef ADC_H
#define ADC_H

#ifdef __cplusplus
extern "C" {
#endif

/* typedefs */
typedef struct {
  long dimSize;
  double arg1[1];
} TD1;
typedef TD1 **TD1Hdl;

/* prototypes */
void readADC(unsigned long *, long *, TD1Hdl, long *, long *);
void readADC_dummy(unsigned long *, long *, TD1Hdl, long *, long *);
void convertADC(TD1Hdl, long *);

#ifdef __cplusplus
}
#endif

#endif
