#include <stdio.h>  
#include <sys/types.h>  
#include <sys/socket.h>  
#include <netinet/in.h>  
#include <arpa/inet.h>  
#include <ros/ros.h>
#include <dji_sdk/dji_drone.h>

using namespace DJI::onboardSDK;


int main(int argc, char *argv[])  
{ 
	ros::init(argc,argv,"recVel");
	ros::NodeHandle n;
 
	dji_sdk::Velocity vel;
	int server_sockfd;//服务器端套接字  
	int client_sockfd;//客户端套接字  
	int len;  
	struct sockaddr_in my_addr;   //服务器网络地址结构体  
	struct sockaddr_in remote_addr; //客户端网络地址结构体  
	int sin_size;  
	char recvbuf[BUFSIZ];  //数据传送的缓冲区  
	memset(&my_addr,0,sizeof(my_addr)); //数据初始化--清零  
	my_addr.sin_family=AF_INET; //设置为IP通信  
	my_addr.sin_addr.s_addr=INADDR_ANY;//服务器IP地址--允许连接到所有本地地址上  
	my_addr.sin_port=htons(5656); //服务器端口号  

	/*创建服务器端套接字--IPv4协议，面向无连接通信，UDP协议*/  
	if((server_sockfd=socket(AF_INET,SOCK_DGRAM,0))<0)  
	{    
		perror("socket error");  
		return 1;  
	}  
	printf("创建socket成功\n");
	/*将套接字绑定到服务器的网络地址上*/  
	if (bind(server_sockfd,(struct sockaddr *)&my_addr,sizeof(struct sockaddr))<0)  
	{  
		perror("bind error\n");  
		return 1;  
	}  
	printf("绑定socket成功！");
	sin_size=sizeof(struct sockaddr_in);  
	ROS_INFO("waiting for a packet...\n");  

	do
	{
		/*接收客户端的数据并将其发送给客户端--recvfrom是无连接的*/  
		if((len=recvfrom(server_sockfd,recvbuf,sizeof(dji_sdk::Velocity),0,(struct sockaddr *)&remote_addr,(socklen_t *)&sin_size))<0)  
		{  
			perror("recvfrom error\n");   
			return 1;  
		}

		memcpy(&vel,recvbuf,sizeof(dji_sdk::Velocity));
		printf("received packet from %s:\n",inet_ntoa(remote_addr.sin_addr));  

		printf("the velocity of x: %f\n",vel.vx);
		printf("the velocity of y: %f\n",vel.vy);  
		printf("the velocity of z: %f\n",vel.vz);    
		/*关闭套接字*/
	}while(vel.vx!=0 || vel.vy!=0 || vel.vz!=0);

	close(server_sockfd);  

	return 0;  
}  
