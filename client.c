#include <stdio.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <fcntl.h>
#include <sys/socket.h>
#include <netdb.h>
#define MAXSIZE 2048


int main(int argc,char *argv[])
{
	struct sockaddr_in client;
	int sd,len,fd;
	char buf[MAXSIZE];
	long total;
	char a;

	if(argc !=2)
	{
		printf("사용법 %s 받을 이름\n",argv[0]);
		exit(0);
	}

	if((sd=socket(PF_INET,SOCK_STREAM,0)) < 0)
	{
		printf("소켓 개설 실패\n");
		exit(1);
	}

	bzero((char *)&client,sizeof(client));

	client.sin_family=AF_INET;
	client.sin_port=htons(4001);
	client.sin_addr.s_addr=htonl(INADDR_ANY);

	if(connect(sd,(struct sockaddr *)&client,sizeof(client)) < 0)
	{
		printf("error\n");
		exit(0);
	}

	fd=open(argv[1],O_WRONLY|O_CREAT|O_TRUNC,0644); // 파일 이름 argv[1]

	recv(sd,buf,MAXSIZE,0);
	total=atol(buf);
	printf("전송 받을 크기%ld\n",total);

	while((len=recv(sd,buf,MAXSIZE,0)) > 0)
	{
		len=write(fd,buf,len); 

		if(len == MAXSIZE)
		{
			total-=(long)len;
			continue;
//			printf("성공 적으로 받아와서 %ld 가 남았음\n",total);
		}
		else if(len < MAXSIZE)
		{
			total-=(long)len;
			if(total <= 0)
			{
				printf("성공 완료\n");
				break;
			}
			printf("비 성공적으로 %d 받아왓당 그러나 %ld 만큼 남았다\n",len,total);
		}
		else if(len == -1)
		{
			printf("전송 실패\n");
			exit(0);
		}
	}
	close(sd);
	close(fd);
	return 0;
}
