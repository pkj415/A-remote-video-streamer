# A-remote-video-streamer
##A simple program in C to stream video from one node to another on the same network using the UDP protocol

**Server** has the video file locally. 
**Client** reequests the file from the server.

###Server
Format : ./serv <Port_Number>

###Client
Format : ./cli <IP_of_Server> <Port_of_Server> <name_of_video_file_in_server_system>

OpenCV is required for proper compilation and execution.

This has been tested for mp4 files only. This can be used to stream live from the webcam by changing the input to the opencv in line 143 - `capture.open(buff)`. Change `buff` to `0` for streaming from webcam. The video is transferred as one packet-per-frame. This restricts the resolution of the video that can be transferred. To avoid this modifications need to be made. 


