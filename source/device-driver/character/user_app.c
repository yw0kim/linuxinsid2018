#include <stdio.h>  
#include <sys/types.h>  
#include <sys/stat.h>  
#include <sys/ioctl.h>  
#include <fcntl.h>  
#include <unistd.h>  
#include <fcntl.h>
#include <stdlib.h>
    
#define DEVICE_FILENAME  "/dev/foo_cdev"  
    
int main()  
{  
    int dev, ret;  
    char buf[100], buf2[100];  
    
    memset( buf2, 0, 0 );  
    strcpy(buf, "life is good" );  
    int len = strlen( buf );  
    
    
    printf( "device file open\n");   
    dev = open("/dev/foo_cdev", O_RDWR);  
    
    if( dev >= 0 )  
    {  
        printf( "App : write something\n");  
        ret = write(dev, buf, len );  
        printf( "%s %dbytes\n", buf, ret );  
    
        printf( "App : read something\n");  
        ret = read(dev, buf2, 100 );  
        printf( "%s %dbytes\n", buf2, ret );  
    
        printf( "ioctl function call\n");  
        ret = ioctl(dev, 0x100, 30 );  
        printf( "ret = %d\n", ret );  
    
        printf( "device file close\n");  
        ret = close(dev);  
        printf( "ret = %d\n", ret );  
    }  
	else {
		printf("Cdev open failed %d\n", dev);
	}
    
    return 0;  
}  
