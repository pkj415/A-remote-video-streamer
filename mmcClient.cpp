#include <stdio.h>
#include <stdlib.h>
#include <netdb.h>
#include <string.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <strings.h>
#include <sys/socket.h>
#include <unistd.h>

#include "opencv2/objdetect.hpp"
#include "opencv2/imgcodecs.hpp"
#include "opencv2/videoio.hpp"
#include "opencv2/highgui.hpp"
#include "opencv2/imgproc.hpp"
#include "opencv2/core/utility.hpp"

#include "opencv2/videoio/videoio_c.h"
#include "opencv2/highgui/highgui_c.h"

#include <cctype>
#include <iostream>
#include <iterator>
#include <stdio.h>
#include <signal.h>
#include <sys/types.h>

using namespace std;
using namespace cv;
int flag = 0;
int toggle = 0;
int filter =0;

void Die(const char *str){
   perror(str);
   exit(0);
}

void SIGINTHandler(int signo){
   if(flag==0)
   {
      //Pause
      flag = 1;
   }
   else{
      //Play
      flag=0;
   }
   toggle=1;
}

void SIGQUITHandler(int signo){
   filter = (filter+1)%5;
   pause();
}

int main(int argc, char *argv[])
{
   if (argc != 4)
          Die("Parameter(s) : <Server Address/Name> <Server Port/Service> <Name Of Video File in Server's System>\n");
   char buff[1024];
   Mat frame;

   int cd;
   struct sockaddr_in serverAddr,clientAddr;
   socklen_t serverAddrLen = sizeof(serverAddr);
   signal(SIGINT, SIGINTHandler);
   signal(SIGQUIT, SIGQUITHandler);

   //cd = socket (AF_INET,SOCK_DGRAM,0);
   /*clientAddr.sin_family = AF_INET;
   clientAddr.sin_addr.s_addr = htonl(INADDR_ANY);
   clientAddr.sin_port = htons(atoi(argv[1]));

   bind( sd, (struct sockaddr*) &serverAddr, sizeof(serverAddr));*/



        char *server = argv[1];     // First arg: server address/name


        // Third arg (optional): server port/service
        char *servPort = argv[2];

        struct addrinfo addrCriteria;                   // Criteria for address match
        memset(&addrCriteria, 0, sizeof(addrCriteria)); // Zero out structure
        addrCriteria.ai_family = AF_UNSPEC;             // Any address family
        // For the following fields, a zero value means "don't care"
        addrCriteria.ai_socktype = SOCK_DGRAM;          // Only datagram sockets
        addrCriteria.ai_protocol = IPPROTO_UDP;         // Only UDP protocol

        // Get address(es)
        struct addrinfo *servAddr; // List of server addresses
        int rtnVal = getaddrinfo(server, servPort, &addrCriteria, &servAddr);
        if (rtnVal != 0)
          Die("getaddrinfo() failed");

        // Create a datagram/UDP socket
        cd = socket(servAddr->ai_family, servAddr->ai_socktype, servAddr->ai_protocol); // Socket descriptor for client
        if (cd < 0)
          Die("socket() failed");

        int numOfBytesSent, numOfBytesRcvd;
        strcpy(buff, argv[3]);
        // Send the string to the server
        numOfBytesSent = sendto(cd, buff, strlen(buff)+1, 0, servAddr->ai_addr, servAddr->ai_addrlen);
        if (numOfBytesSent < 0)
          Die("sendto() failed");
        while(1){
           int  imgSize = frame.total()*frame.elemSize();
           unsigned char image_data[30000];
           if(toggle==1){
                  if(flag==0){
                     memset(buff,0,sizeof(buff));
                     strcpy(buff, "PLAY");
                     sendto(cd, buff, strlen(buff)+1, 0, servAddr->ai_addr, servAddr->ai_addrlen);
                     printf("PLAYED\n");
                     toggle=0;
                  }
                  else{
                     memset(buff,0,sizeof(buff));
                     strcpy(buff, "PAUSE");
                     sendto(cd, buff, strlen(buff)+1, 0, servAddr->ai_addr, servAddr->ai_addrlen);
                     printf("PAUSED\n");
                     toggle=0;
                     pause();
                     printf("PAUSE CLEARED\n");
                     memset(buff,0,sizeof(buff));
                     if(filter==0){
                        strcpy(buff, "PLAYDEFAULT");
                        sendto(cd, buff, strlen(buff)+1, 0, servAddr->ai_addr, servAddr->ai_addrlen);
                     }
                     else if(filter==1){
                        strcpy(buff, "PLAYFILTER1");
                        sendto(cd, buff, strlen(buff)+1, 0, servAddr->ai_addr, servAddr->ai_addrlen);
                     }
                     else if(filter==2){
                        strcpy(buff, "PLAYFILTER2");
                        sendto(cd, buff, strlen(buff)+1, 0, servAddr->ai_addr, servAddr->ai_addrlen);
                     }
                     else if(filter==3){
                        strcpy(buff, "PLAYFILTER3");
                        sendto(cd, buff, strlen(buff)+1, 0, servAddr->ai_addr, servAddr->ai_addrlen);
                     }
                     else{
                        strcpy(buff, "PLAYFILTER4");
                        sendto(cd, buff, strlen(buff)+1, 0, servAddr->ai_addr, servAddr->ai_addrlen);
                     }
                     printf("%s\n",buff);  
                  }
                  
           }
           if ((numOfBytesRcvd = recvfrom(cd, image_data, 30000, 0, servAddr->ai_addr, &servAddr->ai_addrlen)) != -1)
            {
                cv::Mat TempMat = cv::Mat(100, 100, CV_8UC3, image_data);
                  if(!TempMat.empty()){
                    printf("Showing image\n");
                    imshow("DATA RCVD", TempMat);
                    waitKey(10);
                  }
                  
                  //printf("IMSHOW DONE \n");
            }
            if(numOfBytesRcvd==-1)
               printf("Reading Error\n");
            //printf("Frame Rcvd\n");
            //printf("FRAME SIZE %d\n",sizeof(frame.data));
            /*int i;
            for(i=0;i<19100;i++){
               printf("%c",image_data[i]);
            }*/
            
       }
   
   //ipaddress=inet_ntop(AF_INET,&client.sin_addr, ipaddress, clilen);
   //ipaddress=inet_ntoa(client.sin_addr);
   //printf("client address :%s\n",ipaddress);
   close(cd);
   
}

