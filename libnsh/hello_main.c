#include <nuttx/config.h>

#include <sys/types.h>
#include <stdint.h>
#include <stdbool.h>
#include "../arch/arm/src/stm32/stm32_gpio.h"
#if CONFIG_NFILE_DESCRIPTORS > 0
# include <sys/stat.h>
# include <fcntl.h>
# if !defined(CONFIG_DISABLE_MOUNTPOINT)
#   ifdef CONFIG_FS_READABLE /* Need at least one filesytem in configuration */
#     include <sys/mount.h>
#     include <nuttx/fs/ramdisk.h>
#   endif
#   ifdef CONFIG_FS_FAT
#     include <nuttx/fs/mkfatfs.h>
#   endif
#   ifdef CONFIG_FS_SMARTFS
#     include <nuttx/fs/mksmartfs.h>
#   endif
#   ifdef CONFIG_NFS
#     include <sys/socket.h>
#     include <netinet/in.h>
#     include <nuttx/fs/nfs.h>
#   endif
#   ifdef CONFIG_RAMLOG_SYSLOG
#     include <nuttx/syslog/ramlog.h>
#   endif
#endif
#endif

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <dirent.h>
#include <limits.h>
#include <libgen.h>
#include <errno.h>
#include <debug.h>

#include "nsh.h"
#include "nsh_console.h"
//LEDs
//STLED0		: pin 4 - PG4
#define MY_LED						(GPIO_OUTPUT|GPIO_PUSHPULL|GPIO_SPEED_50MHz|\
                         GPIO_OUTPUT_CLEAR|GPIO_PORTG|GPIO_PIN4)
//FPGA pins
//INIT_B		: pin 10 - PF0 - I/P
#define INIT_B		(GPIO_INPUT|GPIO_PUSHPULL|GPIO_SPEED_50MHz|\
									GPIO_OUTPUT_CLEAR|GPIO_PORTF|GPIO_PIN0)
//DONE 			: pin 11 - PF1 - I/P
#define DONE			(GPIO_INPUT|GPIO_PUSHPULL|GPIO_SPEED_50MHz|\
									GPIO_OUTPUT_CLEAR|GPIO_PORTF|GPIO_PIN1)
//PROGRAM_B	: pin 12 - PF2 - O/P
#define PROGRAM_B	(GPIO_OUTPUT|GPIO_PUSHPULL|GPIO_SPEED_50MHz|\
									GPIO_OUTPUT_CLEAR|GPIO_PORTF|GPIO_PIN2)
//CCLK			: pin 13 - PF3 - O/P
#define CCLK			(GPIO_OUTPUT|GPIO_PUSHPULL|GPIO_SPEED_50MHz|\
									GPIO_OUTPUT_CLEAR|GPIO_PORTF|GPIO_PIN3)
//DIN				: pin 14 - PF4 - O/P
#define DIN				(GPIO_OUTPUT|GPIO_PUSHPULL|GPIO_SPEED_50MHz|\
									GPIO_OUTPUT_CLEAR|GPIO_PORTF|GPIO_PIN4)
//DOUT			: pin 15 - PF5 - I/P
#define DOUT			(GPIO_INPUT|GPIO_PUSHPULL|GPIO_SPEED_50MHz|\
									GPIO_OUTPUT_CLEAR|GPIO_PORTF|GPIO_PIN5)

#define low 	false
#define high 	true

# ifndef CONFIG_NSH_DISABLE_HELLO
int cmd_hello(FAR struct nsh_vtbl_s *vtbl,int argc, char *argv[]) 
{
	int ii = 0;  
	printf("Hello, World from Elena!!\n"); 
	stm32_configgpio(MY_LED);
	printf("Here\n");

	FILE *fp;
	fp=fopen("/mnt/sdcard/nsh_test.txt", "w");
	printf("fp=%d\n",fp);
	char x[10]="ABCDEFGHIJ";
	//fwrite(x, sizeof(x[0]), sizeof(x)/sizeof(x[0]), fp);
	fprintf(fp, "This is testing for fprintf...\n");
  fputs("This is testing for fputs...\n", fp);
	fclose(fp);

	char buff[255];

   fp = fopen("/mnt/sdcard/nsh_test.txt", "r");
   //fscanf(fp, "%s", buff);
   //printf("1 : %s\n", buff );

   fgets(buff, 255, (FILE*)fp);
   printf("2: %s\n", buff );
   
   fgets(buff, 255, (FILE*)fp);
   printf("3: %s\n", buff );
   fclose(fp);

	for(ii=0;ii<5;ii++){
		stm32_gpiowrite(MY_LED, false);
		printf("Is the led on?\n");
		up_mdelay(1000);
		stm32_gpiowrite(MY_LED, true);
		printf("Is the led off?\n");
		up_mdelay(1000);
	}

  return 0;

}
# endif

#  ifndef CONFIG_NSH_DISABLE_FPGA_PROG
		int cmd_fpga_prog(FAR struct nsh_vtbl_s *vtbl, int argc, char **argv)
{
	bool bit = 0;
	unsigned char byte = 0x00;
	int ii = 0, jj = 0;  
	int nbytes = 0;
	FILE *fp;
//FPGA Programming

	fp = fopen("/mnt/sdcard/top.bin", "rb");

	stm32_gpiowrite(DIN, low);
	stm32_gpiowrite(PROGRAM_B, low);
	//wait for INIT_B to go low
	while(high == stm32_gpioread(INIT_B));
	stm32_gpiowrite(PROGRAM_B, high);

	//get total number of bytes
	for(nbytes = 0; getc(fp) != EOF; ++nbytes);

	for(jj=0;jj<nbytes;jj++){
	//get_byte
		byte = getc(fp);
		for(ii=0;ii<8;ii++){//little endian
			//set DIN
			bit = (byte >> ii) & 0x01;
			stm32_gpiowrite(DIN, bit);
			//cycle clk
			stm32_gpiowrite(CCLK, low);		
			up_udelay(1);//some delay
			stm32_gpiowrite(CCLK, high);
			up_udelay(1);//some delay
		}
	}
	fclose(fp);
	while(low == stm32_gpioread(DONE)){
		//cycle clk
			stm32_gpiowrite(CCLK, low);		
			up_udelay(1);//some delay
			stm32_gpiowrite(CCLK, high);
			up_udelay(1);//some delay
	}
	stm32_gpiowrite(DIN, low);
	
}
#  endif

#  ifndef CONFIG_NSH_DISABLE_DUMPGPIO
int cmd_dumpgpio(FAR struct nsh_vtbl_s *vtbl, int argc, char **argv)
{
	int i = 0;	
	//int stm32_dumpgpio(uint32_t pinset, const char *msg)
	i = atoi(argv);
	stm32_dumpgpio(i, "test");
	return 0;
}
#  endif
