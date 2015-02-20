
test_adc:
	gcc -Wall -o ./test_adc ./source/test_adc.c source/hiadc_drv.c -Isource/ -I/usr/local/lv70/cintools/ -I/u2/dcs/source_linux/3_hell/drivers/ddlib/ -L/u2/dcs/source_linux/3_hell/drivers/ddlib/ -ldd -lvme 

adclibrary:
	gcc -Wall -o ./libhiadc.so.1.3 ./source/hiadc_1.c -I/usr/local/lv70/cintools/ -shared

all: adclibrary test_adc

