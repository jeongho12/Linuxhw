#include <stdio.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <fcntl.h>
#include <sys/socket.h>
#include <netdb.h>

#define MAXSIZE 2048

int main(int argc,char *argv[])
{
	struct sockaddr_in server,client;
	int sd,client_len,len,fd,newsd;
	char buf[MAXSIZE];
	off_t offset;
	long total;

	if(argc !=2)
	{
		printf("사용법 %s 전송할파일이름\n",argv[0]);
		exit(0);
	}

	if((sd=socket(PF_INET,SOCK_STREAM,0)) < 0)
	{
		printf("소켓 개설 실패\n");
		exit(1);
	}

	bzero((char *)&server,sizeof(server));

	server.sin_family=AF_INET;
	server.sin_port=htons(4001);
	server.sin_addr.s_addr=htonl(INADDR_ANY);

	if(bind(sd,(struct sockaddr *)&server,sizeof(server)) < 0)
	{
		printf("bind error\n");
		exit(0);
	}

	listen(sd,5);
	
	client_len=sizeof(client);
	newsd=accept(sd,(struct sockaddr *)&client,&client_len);

	fd=open(argv[1],O_RDONLY,0); // 파일 이름 argv[1]
	total=lseek(fd,0,SEEK_END); // 전송할 파일 크기

	printf("전송할 파일 크기 %ld \n",total);
	sprintf(buf,"%ld",total);
	send(newsd,buf,MAXSIZE,0);

	lseek(fd,0,SEEK_SET);
	while((len=read(fd,buf,MAXSIZE)) > 0)
	{
		len=send(newsd,buf,len,0);

		if(len == MAXSIZE)
		{
			total-=(long)len;
			if(total <= 0)
			{
				printf("성공적으로 보내기 완료\n");
				len=0;
				break;
			}
			else
			{
//				printf("성공 적으로 보내서 %ld 가 남았음\n",total);
				continue;
			}
		}
		else if(len < MAXSIZE)
		{
			total-=(long)len;
			if(total <= 0)
			{
				printf("성공 완료\n");
				break;
			}
			printf("비 성공적으로 %d 보냇당 그러나 %ld 만큼 남았다\n",len,total);
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
