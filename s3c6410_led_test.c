#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

int main(int argc,char *argv[]){
	int leds_fd;
	int led_num,on_off;
	if(argc!=3)
	{
		printf("please enter right paramter!\n");
		printf("./s3c6410_led_test led_num on_off\n");
		return -1;
	}
	led_num = atoi(argv[1]);
	on_off = atoi(argv[2]);
	leds_fd = open("/dev/s3c6410_led", 0);
	if (leds_fd < 0) 
	{
		perror("open device leds");
		exit(1);
	}
	ioctl(leds_fd,on_off,led_num);
	close(leds_fd);
	return 0;
}
