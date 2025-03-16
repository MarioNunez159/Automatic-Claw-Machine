# Automatic-Claw-Machine
Includes the C++ program to detect colors of objects and retrieve the objects' locations using the OpenCV library. 

main.cpp: Color detection program. Outputs x and y coordinates of objects in camera frame. Requires installation of OpenCV library. Uses Windows for serial port access/coomunication.

camInit.ino: Camera start-up program. Creates server for camera feed to be viewed on https site. Requires installing "esp32" board package by Espressif Systems. 

DeliverCoords.ino: Coordinate delivery program. Waits for radio message from a specific pipeline to determine desired color of object. Read object coordinates made in main.cpp from serial port. Send appropriate coordinates out as radio message through another pipeline. Requires installing "RF24" library by TMRh20.
