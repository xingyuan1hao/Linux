#include <signal.h>  
#include <stdio.h>  
#include <unistd.h>  
  
void ouch(int sig)  
{  
    printf("\nOUCH! - I got signal %d\n", sig);  
    //恢复终端中断信号SIGINT的默认行为  
    (void) signal(SIGINT, SIG_DFL);  
}  
  
int test()  
{  
    //改变终端中断信号SIGINT的默认行为，使之执行ouch函数  
    //而不是终止程序的执行  
    (void) signal(SIGINT, ouch);//执行新的信号处理函数 
    while(1)  
    {  
        printf("Hello World!\n");  
        sleep(1);  
    }  
    return 0;  
}  

  
void ouch1(int sig)  
{  
    printf("\nOUCH! - I got signal %d\n", sig);  
}  
  
int test1()  
{  
    struct sigaction act;  
    act.sa_handler = ouch1;  
    //创建空的信号屏蔽字，即不屏蔽任何信息  
    sigemptyset(&act.sa_mask);  
    //使sigaction函数重置为默认行为  
    act.sa_flags = SA_RESETHAND;  
  
    sigaction(SIGINT, &act, 0);  
  
    while(1)  
    {  
        printf("Hello World!\n");  
        sleep(1);  
    }  
    return 0;  
} 


int main()  
{ 
	//test();
	test1();
}


