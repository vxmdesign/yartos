#include <nuttx/config.h>

#include <nuttx/binfmt/binfmt.h>
#include <nuttx/binfmt/elf.h>
#include <nuttx/binfmt/symtab.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/mount.h>
#include <stdint.h>
#include <stdbool.h>
#include "../arch/arm/src/stm32/stm32_gpio.h"
#include "../arch/arm/src/stm32/stm32_fpga.h"

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
#include <math.h>

#include "nsh.h"
#include "nsh_console.h"
//LEDs
//STLED0		: pin 4 - PG4
#define MY_LED        (GPIO_OUTPUT|GPIO_PUSHPULL|GPIO_SPEED_50MHz|GPIO_OUTPUT_CLEAR|GPIO_PORTG|GPIO_PIN4)
//FPGA pins
//INIT_B		: pin 10 - PF0 - I/P
#define INIT_B        (GPIO_INPUT|GPIO_PUSHPULL|GPIO_SPEED_50MHz|GPIO_OUTPUT_CLEAR|GPIO_PORTF|GPIO_PIN0)
//DONE 			: pin 11 - PF1 - I/P
#define DONE          (GPIO_INPUT|GPIO_PUSHPULL|GPIO_SPEED_50MHz|GPIO_OUTPUT_CLEAR|GPIO_PORTF|GPIO_PIN1)
//PROGRAM_B	: pin 12 - PF2 - O/P
#define PROGRAM_B     (GPIO_OUTPUT|GPIO_PUSHPULL|GPIO_SPEED_50MHz|GPIO_OUTPUT_CLEAR|GPIO_PORTF|GPIO_PIN2)
//CCLK			: pin 13 - PF3 - O/P
#define CCLK          (GPIO_OUTPUT|GPIO_PUSHPULL|GPIO_SPEED_50MHz|GPIO_OUTPUT_CLEAR|GPIO_PORTF|GPIO_PIN3)
//DIN				: pin 14 - PF4 - O/P
#define DIN           (GPIO_OUTPUT|GPIO_PUSHPULL|GPIO_SPEED_50MHz|GPIO_OUTPUT_CLEAR|GPIO_PORTF|GPIO_PIN4)
//DOUT			: pin 15 - PF5 - I/P
#define DOUT          (GPIO_INPUT|GPIO_PUSHPULL|GPIO_SPEED_50MHz|GPIO_OUTPUT_CLEAR|GPIO_PORTF|GPIO_PIN5)
//FSMC_CLK			: pin 117 - PD3 - O/P
#define FSMC_CLK          (GPIO_OUTPUT|GPIO_PUSHPULL|GPIO_SPEED_100MHz|GPIO_OUTPUT_CLEAR|GPIO_PORTD|GPIO_PIN3)
//FPGA_INTR0		: pin  46 -PB0 - I/P
#define FPGA_INTR0 		(GPIO_INPUT|GPIO_SPEED_50MHz|GPIO_PORTB|GPIO_PIN0)



#define low 	false
#define high 	true

//extern const struct symtab_s exports[];
//extern const int nexports;

extern unsigned int __start_symtab;
extern unsigned int __stop_symtab;


#ifndef CONFIG_NSH_DISABLE_HELLO
int cmd_hello(FAR struct nsh_vtbl_s *vtbl,int argc, char *argv[]) {
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
#endif

#ifndef CONFIG_NSH_DISABLE_FPGA_PROG
int cmd_fpga_prog(FAR struct nsh_vtbl_s *vtbl, int argc, char **argv){
  bool bit = 0;
  unsigned char byte = 0x00;
  int ii = 0, jj = 0;  
  int nbytes = 0;
  bool temp_initb;
  int fp;
  stm32_configgpio(INIT_B); 	//input
  stm32_configgpio(DONE);		//input
  stm32_configgpio(PROGRAM_B);	//output
  stm32_configgpio(CCLK);		//output
  stm32_configgpio(DIN);		//output
  stm32_configgpio(DOUT);		//input
  
  //FPGA Programming
  //Initial
  printf("initialize the pins...\n");
  stm32_gpiowrite(PROGRAM_B, high);
  stm32_gpiowrite(CCLK, low);
  stm32_gpiowrite(DIN, low);
  
  
  printf("Waiting\n");
  up_mdelay(100);
  temp_initb = stm32_gpioread(INIT_B);
  printf("the status of init b is %d\n",temp_initb);
  if(temp_initb == 0){
    printf("this is weird\n");
    stm32_gpiowrite(PROGRAM_B, low);
    return 0;
  }
  
  stm32_gpiowrite(PROGRAM_B, low);
  //wait for INIT_B to go low
  while(high == stm32_gpioread(INIT_B)){
    printf("Waiting for INIT_B to go low\n");
    up_mdelay(10);
  }
  printf("It went low!\n");
  
  stm32_gpiowrite(PROGRAM_B, high);
  while(low == stm32_gpioread(INIT_B)){
    printf("Waiting for INIT_B to go high\n");
    up_mdelay(10);
  }
  printf("It went high!\n");
  
  
  printf("Now open file\n");
  fp =open("/mnt/sdcard/top.bin", O_RDONLY);
  printf("Opened file\n");
  printf("starting...\n");
  
  while(read(fp,&byte ,1) == 1){
    //get_byte
    //byte = getc(fp);
    for(ii=7;ii>=0;ii--){//little endian
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
  printf("finished writing..\n");
  stm32_gpiowrite(CCLK, low);		
  up_udelay(1);//some delay
  stm32_gpiowrite(CCLK, high);
  up_udelay(1);//some delay
  close(fp);
  printf("closed the file\n");
  while(low == stm32_gpioread(DONE)){
    printf("Waiting for DONE pin\n");
    up_mdelay(10);		
  }
  stm32_gpiowrite(DIN, low);
  printf("I think I am all done\n");
  return 0;
}
#endif

#ifndef CONFIG_NSH_DISABLE_DUMPGPIO
int cmd_dumpgpio(FAR struct nsh_vtbl_s *vtbl, int argc, char **argv){
  int i = 0;	
  //int stm32_dumpgpio(uint32_t pinset, const char *msg)
  i = atoi(argv);
  stm32_dumpgpio(i, "test");
  return 0;
}
#endif

int cmd_startfsmc(FAR struct nsh_vtbl_s *vtbl, int argc, char **argv){
  int i = 0;
  stm32_enablefpga();
  i = atoi(argv[1]);
  printf("atoi(argv[1]) = %d\n",i);
  up_fpgainitialize(i);
  return 0;
}

int cmd_testfsmcclk(FAR struct nsh_vtbl_s *vtbl, int argc, char **argv){
  int ii = 0, d = 0;
  //Setup Pin
  stm32_configgpio(FSMC_CLK);
  //Initialize Pin
  stm32_gpiowrite(FSMC_CLK, high);
  
  for(ii=0;ii<1000;ii++){
    stm32_gpiowrite(FSMC_CLK, high);
    //up_mdelay(1);
    //d++;
    stm32_gpiowrite(FSMC_CLK, low);
    //d++;
    //up_mdelay(1);
  }
  
  return 0;
}

int cmd_fsmcwrite(FAR struct nsh_vtbl_s *vtbl, int argc, char **argv){
  volatile unsigned short *x;
	int i = atoi(argv[1]);
	int d = atoi(argv[2]);
	x = (unsigned short *) 0x60000000;
  x[i]=d;//0xbeef;
	printf("x[%d] = %d, %d\n",i,x[i],d);
  return 0;
}

int cmd_ledswrite(FAR struct nsh_vtbl_s *vtbl, int argc, char **argv){
  volatile unsigned short *x;
	int ii = 0;
	int r = atoi(argv[1]);
	int g = atoi(argv[2]);
	int b = atoi(argv[3]);
	x = (unsigned short *) 0x60000000;
	for(ii=0;ii<32;ii++){
			x[3*ii] = r;
			x[(3*ii)+1] = g;
			x[(3*ii)+2] = b;
	}
	x[96] = 0;
	printf("x[97] = %d\n",x[97]);
	up_mdelay(500);
	printf("x[97] = %d\n",x[97]);
	

  return 0;
}

struct binary_s bin_te;
int cmd_testelf(FAR struct nsh_vtbl_s *vtbl, int argc, char **argv){
  int ret;
  int prgret;
  ret = elf_initialize();
  printf("Elf test %s\n", argv[1]);
  memset(&bin_te, 0, sizeof(struct binary_s));
  bin_te.filename = argv[1];
  //bin.exports = exports;
  // bin.nexports = nexports;

  ret = load_module(&bin_te);

  if(ret < 0){
    printf("Error could not load module %d\n",ret);
    return 0;
  }

  ret = exec_module(&bin_te);  
  if(ret < 0){
    printf("Error could not execute module\n");
    return 0;
  }
  waitpid(ret, &prgret, 0);
  unload_module(&bin_te);
  return 0;  
}

int cmd_tbltest(FAR struct nsh_vtbl_s *vtbl, int argc, char **argv){
  int *d1;
  int *d2;
  int l;
  int c;
  void *ref;
  struct symtab_s *st;
  d1 = &(__start_symtab);
  d2 = &(__stop_symtab);
  l = ((d2 - d1) * 4) / sizeof(struct symtab_s);
  printf("Items: %d\n", l);  
  printf("addr %08x\n", (unsigned long) d1);
  st = (struct symtab_s *)d1;
  ref = &printf;
  printf("Ref: %08x\n", (unsigned long)ref);
  //printf("Symbol: %s\n", st[0].sym_name);
  for(c = 0; c < l; c++){
    printf("Symbol: %s at %08x\n", st[c].sym_name, st[c].sym_value);
  }
  //l = 2;
  
  return 0;
}


int cmd_fsmcread(FAR struct nsh_vtbl_s *vtbl, int argc, char **argv){
  volatile unsigned short *x;
  unsigned short t;
	int i;// = atoi(argv[1]);
//	i = 2 * i;
//  x = (unsigned short *) (0x60000000+i);
//  t = *x;
//  printf("Read at 0: %04x\n", t);

	int ii;
	for(ii=0;ii<(32);ii++){
		i = ii *2;
		x = (unsigned short *) (0x60000000+i);
  	t = *x;
  	printf("Read at %d: %04x\n",ii, t);
	}

  return 0;
}

int cmd_pwm(FAR struct nsh_vtbl_s *vtbl, int argc, char **argv){
	float v;
	int c,d;
	float step;
	volatile unsigned short *x;
	step = 3.141592654/500;
	x = (unsigned short *) (0x60000000);
	for(d = 0; d < 100; d++){
		for(c = 0; c < 1000; c ++){
			v = (sin(c * step) + 1) * 32760;
			x[0] = (unsigned short) v;
			v = (sin(c * step*2) + 1) * 32760;
			x[1] = (unsigned short) v;
			v = (sin(c * step*3) + 1) * 32760;
			x[2] = (unsigned short) v;
			v = (sin(c * step*4) + 1) * 32760;
			x[3] = (unsigned short) v;
			
			up_mdelay(1);
		}
	}	
	return 0;
}

int cmd_mem_test(FAR struct nsh_vtbl_s *vtbl, int argc, char **argv){	
	int ii = 0, jj = 0, n = 0;
	unsigned short t;
	volatile unsigned short *x;
	x = (unsigned short *) (0x60000000);
	//write
	for(ii=0;ii<32;ii++){
		if((ii%2) == 0){
			x[ii]=5;
		}else{
			x[ii]=10;
		}
	}
	//read
	for(ii=0;ii<32;ii++){
			n = ii *2;
			x = (unsigned short *) (0x60000000+n);
			t = *x;
			if((ii%2) == 0){
				if(t == 5){
					jj++;
				}else{
					printf("seems to be and error here\n");
				}
			}else{
				if(t == 10){
					jj++;
				}else{
					printf("seems to be and error here\n");
				}
			}
	}
	if(jj == 32){
		printf("Memory test 5's and A's was a success!!\n");
	}
	//write all 0
	for(ii=0;ii<32;ii++){
		x[ii]=0;
	}
	jj = 0;
	for(ii=0;ii<32;ii++){
		n = ii *2;
		x = (unsigned short *) (0x60000000+n);
		t = *x;
		if(t == 0){
			jj++;
		}else{
			printf("seems to be and error there\n");
		}	
	}
	if(jj == 32){
		printf("Memory test write all 0's was a success!!\n");
	}

	//write all 1
	for(ii=0;ii<32;ii++){
		x[ii]=1;
	}
	jj = 0;
	for(ii=0;ii<32;ii++){
		n = ii *2;
		x = (unsigned short *) (0x60000000+n);
		t = *x;
		if(t == 1){
			jj++;
		}else{
			printf("seems to be and error where\n");
		}	
	}
	if(jj == 32){
		printf("Memory test write all 1's was a success!!\n");
	}

	//write all 0 again
	for(ii=0;ii<32;ii++){
		x[ii]=0;
	}
	jj = 0;
	for(ii=0;ii<32;ii++){
		n = ii *2;
		x = (unsigned short *) (0x60000000+n);
		t = *x;
		if(t == 0){
			jj++;
		}else{
			printf("seems to be and error ere\n");
		}	
	}
	if(jj == 32){
		printf("Memory test write all 0's  again was a success!!\n");
	}
	return 0;
}

static int int_test(int irq,void* context){
	//printf("Test\n");
	volatile unsigned short *x;
	x = (unsigned short *) (0x60000000);
	x[0] = 55;
	return 0;
}

int cmd_config_int(FAR struct nsh_vtbl_s *vtbl, int argc, char **argv){
	//Setup Pin
  stm32_configgpio(FPGA_INTR0);
	stm32_gpiosetevent(FPGA_INTR0,true,true,true,int_test);

	return 0;
}

