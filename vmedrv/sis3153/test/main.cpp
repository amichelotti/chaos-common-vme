//============================================================================
// Name        : TEST sis3153eth
// Author      : Andrea Michelotti
// Version     :
// Copyright   : Your copyright notice
// Description : Access SIS3153, do basic 32 read/write access, do basic loop single write/read tests
//============================================================================
#include "project_system_define.h"		//define LINUX or WINDOWS
#include "project_interface_define.h"   //define Interface (sis1100/sis310x, sis3150usb or Ethnernet UDP)


#include <iostream>
using namespace std;

//#include <iostream>
//#include <string.h>
#include <stdio.h>
#include <stdlib.h>




#include "sis3153usb.h"
#include "vme_interface_class.h"



#ifdef ETHERNET_VME_INTERFACE
	#include "sis3153ETH_vme_class.h"
	sis3153eth *gl_vme_crate;
	char  gl_sis3153_ip_addr_string[32] = "212.60.16.200";
	//	char  gl_sis3153_ip_addr_string[32] = "192.168.1.11";

	#ifdef LINUX
		#include <sys/types.h>
		#include <sys/socket.h>
	#endif

	#ifdef WINDOWS
	#include <winsock2.h>
	#pragma comment(lib, "ws2_32.lib")
	//#pragma comment(lib, "wsock32.lib")
	typedef int socklen_t;

/*	long WinsockStartup()
	{
	  long rc;

	  WORD wVersionRequested;
	  WSADATA wsaData;
	  wVersionRequested = MAKEWORD(2, 1);

	  rc = WSAStartup( wVersionRequested, &wsaData );
	  return rc;
	}
	*/
	#endif
#endif




vme_interface_class *intf;
//s3316_adc *adc;


int main(int argc, char *argv[])
{

	unsigned int addr;
	unsigned int data ;
	unsigned short ushort_data ;
	cout << "sis3153eth access test" << endl; // prints sis3316_access_test_sis3153eth

	   //char char_command[256];
	char  ip_addr_string[32] ;
	unsigned int vme_base_address,wdata,rdata ;
	char ch_string[64] ;
	int int_ch ;
	int return_code ;
	bool vmewrite=false; 
	bool vmeop=false;
	unsigned int test_cycles=0;
#ifdef ETHERNET_UDP_INTERFACE

	char  pc_ip_addr_string[32] ;

	//strcpy(sis3316_ip_addr_string, gl_sis3316_ip_addr_string) ; // SIS3316 IP address
	//strcpy(sis3316_ip_addr_string,"212.60.16.200") ; // SIS3316 IP address
	strcpy(sis3316_ip_addr_string,"192.168.1.100") ; // SIS3316 IP address
#endif


	// default
	vme_base_address = 0x30000000 ;
	if(argc<2){
		std::cout<<"## you must specify a valid IP address for the SIS3153"<<std::endl;
		return 1;
	}
	strcpy(ip_addr_string,argv[1]) ; // SIS3153 IP address

	   if (argc > 1) {
	#ifdef raus
		   /* Save command line into string "command" */
		   memset(char_command,0,sizeof(char_command));
		   for (i=1;i<argc;i++) {
				strcat(char_command,argv[i]);
				strcat(char_command," ");
			}
			printf("gl_command %s    \n", char_command);
	#endif


			while ((int_ch = getopt(argc, argv, "?hi:a:w:t:")) != -1)
				switch (int_ch) {
				  case 'i':
						sscanf(optarg,"%s", ch_string) ;
						printf("-I %s    \n", ch_string );
						strcpy(ip_addr_string,ch_string) ;
						break;
				  case 'a':
					sscanf(optarg,"%x", &vme_base_address) ;
					vmeop=true;

					break;
				 case 'w':
					sscanf(optarg,"%x", &wdata) ;
					vmewrite=true;
					break;
				 case 't':
					sscanf(optarg,"%x", &test_cycles) ;
					vmewrite=true;
					break;
				  case '?':
				  case 'h':
				  default:
						printf("   \n");
					printf("Usage: %s  [-?h] [-i <sis ip] [ -a <vme address>] [-w <data>] [-t <test cycles>] ", argv[0]);
					printf("   \n");
					printf("   \n");
				    printf("   -I string     SIS3153 IP Address       	Default = %s\n", ip_addr_string);
					printf("   \n");
					printf("   -h            Print this message\n");
					printf("   \n");
					exit(1);
				  }

		 } // if (argc > 1)
		printf("\n");




#ifdef ETHERNET_VME_INTERFACE
	sis3153eth *vme_crate;
	sis3153eth(&vme_crate, ip_addr_string);
#endif


	char char_messages[128] ;
	unsigned int nof_found_devices ;

	// open Vme Interface device
	return_code = vme_crate->vmeopen ();  // open Vme interface
	vme_crate->get_vmeopen_messages (char_messages, &nof_found_devices);  // open Vme interface
    printf("get_vmeopen_messages = %s , number of devices %d \n",char_messages, nof_found_devices);



	if(vmeop){
			struct timeval  tv1, tv2;
			gettimeofday(&tv1, NULL);
		uint32_t cycles=test_cycles;	
		if(test_cycles>0){
/* Program code to execute here */
			while(test_cycles--){
				if(vmewrite){
					return_code = vme_crate->vme_A32D32_write (vme_base_address, wdata); //
				} else {
					return_code = vme_crate->vme_A32D32_read (vme_base_address, &data); //

				}

			}
		
		} else if(vmewrite){
			return_code = vme_crate->vme_A32D32_write (vme_base_address, wdata); //
		  	 printf("vme_A32D32_write: addr = 0x%08X    data = 0x%08X    return_code = 0x%08X \n", vme_base_address, wdata,return_code);

		} else {
			  return_code = vme_crate->vme_A32D32_read (vme_base_address, &data); //
	   		printf("vme_A32D32_read: addr = 0x%08X    data = 0x%08X    return_code = 0x%08X \n", vme_base_address, data,return_code);
		}

			gettimeofday(&tv2, NULL);
			double time_taken=(double)(tv2.tv_usec - tv1.tv_usec) / 1000000 +(double) (tv2.tv_sec - tv1.tv_sec);
			double average=(cycles>0)?time_taken/((double)cycles):time_taken;

			printf("Time taken in execution = %lf s, average per op =%lf s\n",time_taken,average );

	} else {
	return_code = vme_crate->udp_sis3153_register_read (SIS3153USB_CONTROL_STATUS, &data); //
	printf("udp_sis3153_register_read: addr = 0x%08X    data = 0x%08X    return_code = 0x%08X \n", SIS3153USB_CONTROL_STATUS, data,return_code);
	return_code = vme_crate->udp_sis3153_register_read (SIS3153USB_MODID_VERSION, &data); //
	printf("udp_sis3153_register_read: addr = 0x%08X    data = 0x%08X    return_code = 0x%08X \n", SIS3153USB_MODID_VERSION, data,return_code);
	return_code = vme_crate->udp_sis3153_register_read (SIS3153USB_SERIAL_NUMBER_REG, &data); //
	printf("udp_sis3153_register_read: addr = 0x%08X    data = 0x%08X    return_code = 0x%08X \n", SIS3153USB_SERIAL_NUMBER_REG, data,return_code);
	return_code = vme_crate->udp_sis3153_register_read (SIS3153USB_LEMO_IO_CTRL_REG, &data); //
	printf("udp_sis3153_register_read: addr = 0x%08X    data = 0x%08X    return_code = 0x%08X \n", SIS3153USB_LEMO_IO_CTRL_REG, data,return_code);

	printf("\n");
	}
  



	return 0;
}

