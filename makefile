all:
	g++ -I/usr/local/include/opencv -I/usr/local/include -o cli mmcClient.cpp -L/usr/local/lib -lopencv_highgui -lopencv_videoio -lopencv_video -lopencv_photo -lopencv_imgproc -lopencv_core 
	g++ -I/usr/local/include/opencv -I/usr/local/include -o serv mmcServer.cpp -L/usr/local/lib -lopencv_highgui -lopencv_videoio -lopencv_video -lopencv_photo -lopencv_imgproc -lopencv_core