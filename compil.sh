#!/bin/bash
g++  main.cpp recorder.cpp  functions.cpp -lopencv_core -lopencv_imgcodecs -lopencv_highgui -lopencv_videoio -lopencv_imgproc -lk4a -lstdc++fs -lturbojpeg -lsfml-audio -o recorder
g++ upload.cpp -lsfml-system -lsfml-network -lstdc++fs -o upload
