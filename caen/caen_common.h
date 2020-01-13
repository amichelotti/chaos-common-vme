#ifndef _CAEN_COMMON_H_
#define  _CAEN_COMMON_H_


#include <common/vme/core/vmewrap.h>
// Register map
#define MANUFACTURE_ID 0x40
#define FW_REVISION_OFF 0x1000
#define BOARD_ID_OFF 0x803A
#define BOARD_ID_LSB_OFF 0x803E
#define BOARD_ID_MSB_OFF 0x8036
#define BOARD_VERSION_OFF 0x8032
#define OUI_OFF 0x802A
#define THRS_CHANNEL_OFF 0x1080
#define GEO_ADDRESS_OFF 0x1002
#define BITSET_OFF 0x1006
#define BITCLR_OFF 0x1008
#define STATUS_OFF 0x100E
#define CTRL_OFF 0x1010
#define SSRESET_OFF 0x1016
#define FSR_OFF 0x1060
/**interrupt*/
#define IVR_STATUS 0x100C
#define IVR_LEVEL 0x100A
#define EVT_TRG_OFF 0x1020
/***/
#define STATUS2_OFF 0x1022
#define EVT_CNT_LOW_OFF 0x1024
#define EVT_CNT_HI_OFF 0x1026
#define EVT_CNTRESET_OFF 0x1040
#define IPED_OFF 0x1060
#define AAD_OFF 0x1070
#define BAD_OFF 0x1072
#define SLIDE_CONST_OFF 0x106A
#define CRATE_SEL_OFF 0x103C
#define BITSET2_OFF 0x1032
#define BITCLR2_OFF 0x1034
#define FW_REVISION_REG(base) REG16(base,FW_REVISION_OFF)
#define BOARD_ID_REG(base) REG16(base,BOARD_ID_OFF)
#define BOARD_ID_LSB_REG(base) REG16(base,BOARD_ID_LSB_OFF)
#define BOARD_ID_MSB_REG(base) REG16(base,BOARD_ID_MSB_OFF)
#define BOARD_VERSION_REG(base) REG16(base,BOARD_VERSION_OFF)
#define OUI_REG(base) REG16(base,OUI_OFF)
#define THRS_CHANNEL_REG(base,chan,lo) REG16(base,THRS_CHANNEL_OFF + (4*chan) + (2*lo))
#define GEO_ADDRESS_REG(base) REG16(base,GEO_ADDRESS_OFF)
#define BITSET_REG(base) REG16(base,BITSET_OFF)
#define BITCLR_REG(base) REG16(base,BITCLR_OFF)
#define STATUS_REG(base) REG16(base,STATUS_OFF)
#define CTRL_REG(base) REG16(base,CTRL_OFF)
#define SSRESET_REG(base) REG16(base,SSRESET_OFF)
#define EVT_TRG_REG(base) REG16(base,EVT_TRG_OFF)
#define STATUS2_REG(base) REG16(base,STATUS2_OFF)
#define EVT_CNT_LOW_REG(base) REG16(base,EVT_CNT_LOW_OFF)
#define EVT_CNT_HI_REG(base) REG16(base,EVT_CNT_HI_OFF)
#define EVT_CNTRESET_REG(base) REG16(base,EVT_CNTRESET_OFF)
#define IPED_REG(base) REG16(base,IPED_OFF)
#define AAD_REG(base) REG16(base,AAD_OFF)
#define BAD_REG(base) REG16(base,BAD_OFF)
#define CRATE_SEL_REG(base) REG16(base,CRATE_SEL_OFF)
#define SLIDE_CONST_REG(base) REG16(base,SLIDE_CONST_OFF)

#define BITSET2_REG(base) REG16(base,BITSET2_OFF)
#define BITCLR2_REG(base) REG16(base,BITCLR2_OFF)

/* BITSET1 */
#define SOFTRESET_BIT (1<<7)

/*** BITSET/CLR 2  */
#define TEST_MEM_BIT 0x1 /* test memory, enable direct writes */
#define OFFLINE_BIT 0x2 /* adc offine */
#define CLEARDATA_BIT 0x4 /* clear data (data reset signal), write&read pointers */
#define OVERRANGE_EN_BIT 0x8 /*no overflow suppression*/
#define LOWTHR_EN_BIT 0x10 /* no zero suppression */
#define VALID_CONTROL 0x20 /* 775 disable memorization of data when in common stop data)*/
#define TESTAQ_BIT 0x40 /* acquisition test mode, the data are taken from an internal fifo */
#define SLIDE_EN_BIT 0x80 /* slide scale enable*/
#define STEP_TH_BIT 0x100 /* set the zero suppression resolution*/
#define COMMON_STOP_MODE 0x200 /*775 select common stop mode*/
#define RESERVED_1 0x400
#define AUTOINCR_BIT 0x800 /* enable read pointer autoincrement */
#define EMPTY_EN_BIT 0x1000 /* write the header and EOB when there are not accepted data*/
#define SLIDESUB_EN_BIT 0x2000 /* allow to change oepration mode */
#define ALLTRG_EN_BIT 0x4000 /* enable the event counter to increment also to not accepted trigger*/ 

typedef enum caen_modes {
	CAEN_MEMORY_TEST=0x1,
	CAEN_ADC_OFFLINE=(1<<1),
	CAEN_CLEAR_DATA=(1<<2),
	CAEN_ACCEPT_OVER_RANGE=(1<<3),
	CAEN_THRESHOLD_DISABLE=(1<<4),
	CAEN_VALID_DISABLE=(1<<5),
	CAEN_TEST_ACQ=(1<<6),
	CAEN_SLIDE_ENABLE=(1<<7),
	CAEN_TH_RESOLUTION=(1<<8),
	CAEN_COMMON_STOP=(1<<10),
	CAEN_AUTO_INCR=(1<<11),
	CAEN_EMPTY_PROG=(1<<12),
	CAEN_ALL_TRIGGER=(1<<14)
} caen_modes_t;
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

typedef struct _eob {
  uint32_t ev_counter:24;
  uint32_t signature:3;
  uint32_t geo:5;
} eob_t;

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
#define VERSION_ID 0

#else
#ifdef CAEN775N
#define DEV_BOARD "CAEN775N"
#define NCHANNELS 16
#define BOARD_ID 0x3
#define VERSION_ID 0x13

typedef struct _data {
  uint32_t adc:12;
  uint32_t ov:1;
  uint32_t un:1;
  uint32_t vd:1;
  uint32_t pad:2;
  uint32_t channel:4;
  uint32_t pad2:3;
  uint32_t signature:3;
  uint32_t geo:5;
} data_t;
#else
#ifdef CAEN775
#define DEV_BOARD "CAEN775"
#define NCHANNELS 32
#define BOARD_ID 0x3
#define VERSION_ID 0x13

typedef struct _data {
  uint32_t adc:12;
  uint32_t ov:1;
  uint32_t un:1;
  uint32_t vd:1;
  uint32_t pad:1;
  uint32_t channel:5;
  uint32_t pad2:3;
  uint32_t signature:3;
  uint32_t geo:5;
} data_t;
#else
#ifdef CAEN_GENERIC

typedef union {
	eob_t ddata;
	uint32_t data;
} common_footer_t;

typedef union {
	header_t ddata;
	uint32_t data;
} common_header_t;
typedef struct _data {
	uint32_t adc:12;
	uint32_t ov:1;
	uint32_t un:1;
	uint32_t vd:1;
	uint32_t pad:1;
	uint32_t channel:5;
	uint32_t pad2:3;
	uint32_t signature:3;
	uint32_t geo:5;
} data_t;
typedef union {
	data_t ddata;
	uint32_t data;
} common_data_t;

#else
#error "UKNWON CAEN BOARD"
#endif
#endif
#endif
#endif
#endif


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
  void* mapped_address;
  uint32_t event_counter;
  uint64_t cycle;
  int boardid;
  int version;
  int manufactureid;
} _caen_common_handle_t ;

typedef void* caen_handle_t;


#endif
