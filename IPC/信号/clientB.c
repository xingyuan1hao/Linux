#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <errno.h>
#include <string.h>
#include <fcntl.h>

#define MAX 100

void signal_handler(int signum)
{
    static int flag = 0;

    switch(signum)
    {
        case SIGALRM:
            if(flag == 0)
            {
                printf("The people is leaving,the system is closed in 10 seconds \
                        and you can input 'ctrl + c' cancel.\n");
                alarm(10);
            }else{
                
                kill(getppid(),SIGKILL);
                usleep(500);
                exit(EXIT_SUCCESS);
            }

            flag = 1;            
            break;

        case SIGINT:
            printf("The alarm is cancel.\n");
            alarm(0);
            break;
    }

}

int child_recv_fifo(char *fifo_name)
{
    int n,fd;
    char buf[MAX];

    if((fd = open(fifo_name,O_RDONLY)) < 0)
    {
        fprintf(stderr,"fail to open %s : %s.\n",fifo_name,strerror(errno));
        return -1;
    }

    signal(SIGALRM,signal_handler);
    signal(SIGINT,signal_handler);
    alarm(15);
    while(1)
    {
        n = read(fd,buf,sizeof(buf));
        buf[n] = '\0';

        printf("Read %d bytes : %s.\n",n,buf);

        if(strncmp(buf,"quit",4) == 0 || n == 0)
        {
            kill(getppid(),SIGKILL);
            usleep(500);
            exit(EXIT_SUCCESS);
        }

        alarm(15);
    }

    return 0;
}

int father_send_fifo(char *fifo_name,int pid)
{
    int n,fd;
    char buf[MAX];

    if((fd = open(fifo_name,O_WRONLY)) < 0)
    {
        fprintf(stderr,"Fail to open %s : %s.\n",fifo_name,strerror(errno));
        return -1;
    }

    signal(SIGINT,SIG_IGN);

    while(1)
    {
        getchar();
        printf(">");

        fgets(buf,sizeof(buf),stdin);
        buf[strlen(buf)-1] = '\0';

        write(fd,buf,strlen(buf));

        if(strncmp(buf,"quit",4) == 0)
        {
            kill(pid,SIGKILL);
            usleep(500);
            exit(EXIT_SUCCESS);
        }
    }

    return 0;
}

int main(int argc,char *argv[])
{
    int pid;

    if(argc < 3)
    {
        fprintf(stderr,"usage %s argv[1].\n",argv[0]);
        exit(EXIT_FAILURE);
    }

    if(mkfifo(argv[1],0666) < 0 && errno != EEXIST)
    {
        perror("Fail to mkfifo");
        exit(EXIT_FAILURE);
    }

    if(mkfifo(argv[2],0666) < 0 && errno != EEXIST)
    {
        perror("Fail to mkfifo");
        exit(EXIT_FAILURE);
    }
    
    if((pid = fork()) < 0)
    {
    
        perror("Fail to fork");
        exit(EXIT_FAILURE);
    
    }else if(pid == 0){
        
        child_recv_fifo(argv[1]);
    
    }else{

        father_send_fifo(argv[2],pid);
    }

    exit(EXIT_SUCCESS);
}