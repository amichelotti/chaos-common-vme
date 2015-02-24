#ifndef _CAEN_QDC_H_
#define  _CAEN_QDC_H_
#include "vmewrap.h"
// Register map

#define FW_REVISION_REG(base) REG16(base,0x1000)
#define BOARD_ID_REG(base) REG16(base,0x803A)
#define BOARD_ID_LSB_REG(base) REG16(base,0x803E)
#define BOARD_ID_MSB_REG(base) REG16(base,0x8036)
#define OUI_REG(base) REG16(base,0x802A)

#define THRS_CHANNEL_REG(base,chan,lo) REG16(base,0x1080 + 4*chan + 2*lo)
#define GEO_ADDRESS_REG(base) REG16(base,0x1002)
#define BITSET_REG(base) REG16(base,0x1006)
#define BITCLR_REG(base) REG16(base,0x1008)
#define STATUS_REG(base) REG16(base,0x100E)
#define CTRL_REG(base) REG16(base,0x1010)
#define SSRESET_REG(base) REG16(base,0x1016)
#define EVT_TRG_REG(base) REG16(base,0x1020)
#define STATUS2_REG(base) REG16(base,0x1022)
#define EVT_CNT_LOW_REG(base) REG16(base,0x1024)
#define EVT_CNT_HI_REG(base) REG16(base,0x1026)
#define EVT_CNTRESET_REG(base) REG16(base,0x1040)
#define IPED_REG(base) REG16(base,0x1060)
#define AAD_REG(base) REG16(base,0x1070)
#define BAD_REG(base) REG16(base,0x1072)
#define CRATE_SEL_REG(base) REG16(base,0x103C)
#define SLIDE_CONST_REG(base) REG16(base,0x106A)

#define BITSET2_REG(base) REG16(base,0x1032)
#define BITCLR2_REG(base) REG16(base,0x1034)

/* BITSET1 */
#define SOFTRESET_BIT (1<<7)

/*** BITSET/CLR 2  */
#define TEST_MEM_BIT 0x1 /* test memory, enable direct writes */
#define OFFLINE_BIT 0x2 /* adc offine */
#define CLEARDATA_BIT 0x4 /* clear data (data reset signal), write&read pointers */
#define OVERRANGE_EN_BIT 0x8 /*no overflow suppression*/
#define LOWTHR_EN_BIT 0x10 /* no zero suppression */
#define TESTAQ_BIT 0x40 /* acquisition test mode, the data are taken from an internal fifo */
#define SLIDE_EN_BIT 0x80 /* slide scale enable*/
#define STEP_TH_BIT 0x100 /* set the zero suppression resolution*/
#define AUTOINCR_BIT 0x800 /* enable read pointer autoincrement */
#define EMPTY_EN_BIT 0x1000 /* write the header and EOB when there are not accepted data*/
#define SLIDESUB_EN_BIT 0x2000 /* allow to change oepration mode */
#define ALLTRG_EN_BIT 0x4000 /* enable the event counter to increment also to not accepted trigger*/ 
/****** */
/* 
   status bits
*/
#define CAEN_QDC_STATUS_DREADY 0x1 /* data ready */
#define CAEN_QDC_STATUS_GDREADY 0x2 /* global data ready, at least one board has data ready */

#define CAEN_QDC_STATUS_BUSY 0x4 /* board busy */
#define CAEN_QDC_STATUS_GBUSY 0x8 /* global busy */

#define CAEN_QDC_STATUS_PURGE 0x20 /* board purged */
#define CAEN_QDC_STATUS_EVRDY 0x100 /* event ready */

/* 
   buffer status bits
*/
#define CAEN_QDC_STATUS_BUFFER_EMPTY 0x2
#define CAEN_QDC_STATUS_BUFFER_FULL 0x4




typedef struct _header {
  uint32_t unused:8;
  uint32_t cnt:6;
  uint32_t pad:2;
  uint32_t crate:8;
  uint32_t signature:3;
  uint32_t geo:5;
} header_t;


#ifdef CAEN792

typedef struct _data {
  uint32_t adc:12;
  uint32_t ov:1;
  uint32_t un:1;
  uint32_t pad:2;
  uint32_t channel:5;
  uint32_t pad2:3;
  uint32_t signature:3;
  uint32_t geo:5;
} data_t;
#define NCHANNELS 32
#define DEV_BOARD "CAEN792"
#define BOARD_ID 0x3
#define MANUFACTURE_ID 0x40
#define VERSION_ID 0x11
#else

#ifdef CAEN965
typedef struct _data {
  uint32_t adc:12;
  uint32_t ov:1;
  uint32_t un:1;
  uint32_t pad:2;
  uint32_t rg:1;
  uint32_t channel:4;
  uint32_t pad2:3;
  uint32_t signature:3;
  uint32_t geo:5;
} data_t;
#define DEV_BOARD "CAEN965"
#define NCHANNELS 16
#define BOARD_ID 0x3
#define MANUFACTURE_ID 0x40
#define VERSION_ID 0

#else
#error "UKNWON CAEN BOARD"
#endif
#endif

typedef struct _eob {
  uint32_t ev_counter:24;
  uint32_t signature:3;
  uint32_t geo:5;
} eob_t;

typedef struct __evt_buffer {
    
  union {
    header_t h;
    data_t d;
    eob_t e;
    uint32_t data;
  };
} evt_buffer_t;



/** 
    private date,
    handle
*/
typedef struct __vme_handle__ {
 vmewrap_vme_handle_t vme;
  uint32_t mapped_address;
  uint32_t event_counter;
  uint64_t cycle;
  int boardid;
  int version;
  int manufactureid;
} _caen_qdc_handle_t ;


#endif
