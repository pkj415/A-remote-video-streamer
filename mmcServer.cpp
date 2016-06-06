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
#include <sys/types.h>

using namespace std;
using namespace cv;

int filter=0;
int stepSize=40;
int start = 0;
void Die(const char *str){
   perror(str);
   exit(0);
}

Mat applyFilter(Mat frame){
	Mat modifiedFrame;
    resize(frame, modifiedFrame, cv::Size(100, 100));
    Mat edges;
	resize(modifiedFrame, edges, cv::Size(100, 100));
    

	if(filter==0){
		return edges;
	}
	else if(filter==1){
		cvtColor(modifiedFrame, edges, CV_BGR2GRAY);
	    GaussianBlur(edges, edges,cv::Size(7,7), 1.5, 1.5);
	    Canny(edges, edges, 0, 30, 3);
	    return edges;
	}
	else if(filter==2){
		int arr[4][4]={{0.25,0.5,0.75,1},{0.25,0.5,0.75,1},{0.25,0.5,0.75,1},{0.25,0.5,0.75,1}};
		for (int i = 0; i < modifiedFrame.cols; i++) {
			for (int j = 0; j < modifiedFrame.rows; j++) {
				
				edges.at<Vec3b>(cv::Point(i, j))[0] = arr[j%4][i%4] * edges.at<Vec3b>(cv::Point(i, j))[0];
				edges.at<Vec3b>(cv::Point(i, j))[1] = arr[j%4][i%4] * edges.at<Vec3b>(cv::Point(i, j))[1];
				edges.at<Vec3b>(cv::Point(i, j))[2] = arr[j%4][i%4] * edges.at<Vec3b>(cv::Point(i, j))[2]; 
			}
		}
		return edges;
	}
	else if(filter==3){
		for (int i = 0; i < modifiedFrame.cols; i++) {
			for (int j = 0; j < modifiedFrame.rows; j++) {
				edges.at<Vec3b>(cv::Point(i, j))[0] = 0;
			}
		}
		return edges;
	}
	else{
		int end = (start + stepSize)%modifiedFrame.rows;
		for (int i = 0; i < modifiedFrame.cols; i++) {
			for (int j = start; j < end; j++) {
				edges.at<Vec3b>(cv::Point(i, j))[0] = 0;
				edges.at<Vec3b>(cv::Point(i, j))[1] = 0;
				edges.at<Vec3b>(cv::Point(i, j))[2] = 0;
			}
		}
		start = end;
		return edges;
	}
    /*switch(filter){
    	case 0:
	    
    	case 1:
        int arr[4][4]={{0.25,0.5,0.75,1},{0.25,0.5,0.75,1},{0.25,0.5,0.75,1},{0.25,0.5,0.75,1}};
		for (int i = 0; i < modifiedFrame.cols; i++) {
			for (int j = 0; j < modifiedFrame.rows; j++) {
				
				edges.at<Vec3b>(cv::Point(i, j))[0] = arr[j%4][i%4] * edges.at<Vec3b>(cv::Point(i, j))[0];
				edges.at<Vec3b>(cv::Point(i, j))[1] = arr[j%4][i%4] * edges.at<Vec3b>(cv::Point(i, j))[1];
				edges.at<Vec3b>(cv::Point(i, j))[2] = arr[j%4][i%4] * edges.at<Vec3b>(cv::Point(i, j))[2]; 
			}
		}
		return edges;    	
    	case 2:
    	break;
    	case 3:
    	break;
    	case 4:
    	break;
    }*/

    return edges;
}
int main(int argc, char *argv[])
{
   if(argc!=2){
      printf("Format %s <PortNumber>\n",argv[0]);
      exit(0);
   }
   char buff[1024];

   int sd;
   struct sockaddr_in serverAddr,clientAddr;
   socklen_t clientAddrLen = sizeof(clientAddr);

   sd = socket (AF_INET,SOCK_DGRAM,0);
   serverAddr.sin_family = AF_INET;
   serverAddr.sin_addr.s_addr = htonl(INADDR_ANY);
   serverAddr.sin_port = htons(atoi(argv[1]));

   if(bind( sd, (struct sockaddr*) &serverAddr, sizeof(serverAddr))==-1){
      Die("Bind failed.\n");
   }
   int numOfBytesSent, numOfBytesRcvd;
   memset(&clientAddr, 0 , sizeof(clientAddr));
   if ((numOfBytesRcvd = recvfrom(sd, buff, sizeof(buff), 0, (struct sockaddr*)&clientAddr, &clientAddrLen)) == -1)
   {
       perror("talker: sendto");
       exit(1);
   }
   char fileName[50];
   memset(fileName, 0, sizeof(fileName));
   strcpy(fileName, buff);

   VideoCapture capture;
   Mat frame;
   capture.open(buff);
   if ( ! capture.isOpened() ) 
   {
       Die("--(!)Error opening video capture\n");
   }
   //printf("MSG RCVD : %s\n", buff);
   while(1){
      capture >> frame;
      //frame = imread("Chief Guest_Dhiti.jpg", CV_LOAD_IMAGE_COLOR);
      //Mat smallFrame;
      //resize(frame, smallFrame, cv::Size(100, 100));
      Mat smallFrame = applyFilter(frame);
      imshow("SENT DATA", smallFrame);
      waitKey(10);
      unsigned char *input = (unsigned char*)(smallFrame.data);
      int  smallImgSize = smallFrame.total()*smallFrame.elemSize();
      int  imgSize = frame.total()*frame.elemSize();
      //printf("IMG SIZE TOTAL %d ELEMSIZE %d = %d\n",smallFrame.total(),smallFrame.elemSize(),smallImgSize);
      int i;
      /*for(i=0;i<imgSize;i++){
               printf("%c",input[i]);
      }*/
      memset(buff, 0, sizeof(buff));
      if ((numOfBytesRcvd = recvfrom(sd, buff, sizeof(buff), MSG_DONTWAIT, (struct sockaddr*)&clientAddr, &clientAddrLen)) > 0)
      {
          if(strcmp(buff, "PLAYDEFAULT")==0){
            printf("PLAYED\n");
          }
          else if(strcmp(buff, "PLAYFILTER1")==0){
            printf("PLAYEDFILTER1\n");
          }
          else if(strcmp(buff, "PLAYFILTER2")==0){
            printf("PLAYEDFILTER2\n");
          }
          else if(strcmp(buff, "PLAYFILTER3")==0){
            printf("PLAYEDFILTER3\n");
          }
          else if(strcmp(buff, "PLAYFILTER4")==0){
            printf("PLAYEDFILTER4\n");
          }
          else if(strcmp(buff, "PAUSE")==0){
            memset(buff, 0, sizeof(buff));
            printf("PAUSED\n");
            while(recvfrom(sd, buff, sizeof(buff), 0, (struct sockaddr*)&clientAddr, &clientAddrLen) > 0){
               if(strcmp(buff, "PLAYDEFAULT")==0){
                  printf("PLAYING DEFAULT AFTER PAUSED\n");
                  filter = 0;
                  break;
               }
               if(strcmp(buff, "PLAYFILTER1")==0){
                  printf("PLAYING FILTER1 AFTER PAUSED\n");
                  filter = 1;
                  break;
               }
               if(strcmp(buff, "PLAYFILTER2")==0){
                  printf("PLAYING FILTER2 AFTER PAUSED\n");
                  filter = 2;
                  break;
               }
               if(strcmp(buff, "PLAYFILTER3")==0){
                  printf("PLAYING FILTER3 AFTER PAUSED\n");
                  filter = 3;
                  break;
               }
               if(strcmp(buff, "PLAYFILTER4")==0){
                  printf("PLAYING FILTER4 AFTER PAUSED\n");
                  filter = 4;
                  break;
               }
               memset(buff, 0, sizeof(buff));   
            }
          }
          else{

          }
      }
      fflush(stdout);
      if((numOfBytesSent = sendto(sd, smallFrame.data, smallImgSize, 0, (struct sockaddr*)&clientAddr, sizeof(clientAddr)))==-1){
         Die("Send failed\n");
      }
      //printf("Frame Sent\n");
      //printf("Ended transmission!.\n");
      //imshow("SENT DATA", frame);
      //waitKey(100000);
   }
   
   
   //ipaddress=inet_ntop(AF_INET,&client.sin_addr, ipaddress, clilen);
   //ipaddress=inet_ntoa(client.sin_addr);
   //printf("client address :%s\n",ipaddress);
   close(sd);

   
}

