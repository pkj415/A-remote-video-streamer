# A-remote-video-streamer
##A simple program in C to stream video from one node to another on the same LAN using the UDP protocol

**Server** has the video file locally. <br />
**Client** reequests the file from the server.

###Server
Format : ./serv \<Port_Number\>

###Client
Format : ./cli \<IP_of_Server\> \<Port_of_Server\> \<name_of_video_file_in_server_system\>

OpenCV is required for proper compilation and execution.

This has been tested for mp4 files only. This can be used to stream live from the webcam by changing the input to the opencv in line 143 - `capture.open(buff)`. Change `buff` to `0` for streaming from webcam. The video is transferred as one packet-per-frame. This restricts the resolution of the video that can be transferred. To avoid this modifications need to be made. 

**A few bonus interactive user controls :-**

1. Play and Pause functionality using Unix signal handling.
2. A toggle facility to switch between a set of run-time video filters (filters - implemented using OpenCV)

Please refer the design document for more information.


