#include <iostream>                 //using OpenCV open source computer vision library for image processing - color detection
#include <opencv2/opencv.hpp>       
#include <opencv2/objdetect.hpp>
#include <opencv2/video.hpp>
#include <opencv2/videoio.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/highgui.hpp>
#include <string>
#include <Windows.h>                //header file for access to Serial port


using namespace cv;
using namespace std;

int main()
{   
    Mat frame, frameColor, frameRed, frameBlue, frameOrange, framePurple;     //matrices for video capture & color detection
    int posXred = -1, posYred = -1, posXblue = -1, posYblue = -1, posXorange = -1, posYorange = -1, posXpurple = -1, posYpurple = -1;   //initialize frame coordinates - (-1,-1) means color is out-of-frame)
    
    while (waitKey(10) != 'q') {   //run until "q" key is pressed
        int serial = 0;     //flag to indicate Serial is accessed

        char szPortName[9];
        wsprintf(szPortName,"\\\\.\\COM3",1);   //using Serial in COM3 port of computer
        HANDLE hSerial = CreateFile(szPortName, GENERIC_READ | GENERIC_WRITE, 0, 0, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);   //open COM3 for writing

        if (hSerial !=INVALID_HANDLE_VALUE) {   //Serial port accessed successfully
            //printf("Port opened! \n");
            serial = 1;

            DCB dcbSerialParams;        //holds Serial communication settings
            GetCommState(hSerial,&dcbSerialParams);

            dcbSerialParams.BaudRate = CBR_9600;     //set settings to the same settings as COM3 port
            dcbSerialParams.ByteSize = 8;
            dcbSerialParams.Parity = NOPARITY;
            dcbSerialParams.StopBits = ONESTOPBIT;

            SetCommState(hSerial, &dcbSerialParams);
        }
        else {       //Serial port access denied
            if (GetLastError() == ERROR_FILE_NOT_FOUND)
            {
                printf("Serial port doesn't exist! \n");
            }

            printf("Error while setting up serial port! \n");
        }

        DWORD btsIO;

        VideoCapture capture("http://192.168.5.155/capture");   //get image from camera via network server
        //VideoCapture capture(0);      //previously used laptop's webcam for testing
        if (!capture.isOpened()) {  //no image feed available
            printf("No video feed\n");
            return -1;
        }
        
        capture.read(frame);  //convert image into matrix frame

        if (frame.empty()) {    //frame is empty
            printf("No camera frame\n");
            return -1;
        }
        
        cvtColor(frame, frameColor, COLOR_BGR2HSV);   //frameColor uses hue, saturation, and value for color instead of RGB

        inRange(frameColor, Scalar(165, 90, 100), Scalar(179, 200, 255), frameRed);     //Only show red
        inRange(frameColor, Scalar(65, 70, 70), Scalar(120, 200, 255), frameBlue);      //Only show blue
        inRange(frameColor, Scalar(0, 90, 100), Scalar(50, 200, 255), frameOrange);     //Only show orange
        inRange(frameColor, Scalar(130, 60, 60), Scalar(165, 200, 255), framePurple);   //Only show purple

        erode(frameRed, frameRed, getStructuringElement(MORPH_ELLIPSE, Size(3, 3)) );   //erode then dilate each color's frame to only show larger objects
        dilate(frameRed, frameRed, getStructuringElement(MORPH_ELLIPSE, Size(5, 5)) );

        erode(frameBlue, frameBlue, getStructuringElement(MORPH_ELLIPSE, Size(5, 5)) );
        dilate(frameBlue, frameBlue, getStructuringElement(MORPH_ELLIPSE, Size(5, 5)) );

        erode(frameOrange, frameOrange, getStructuringElement(MORPH_ELLIPSE, Size(5, 5)) );
        dilate(frameOrange, frameOrange, getStructuringElement(MORPH_ELLIPSE, Size(5, 5)) ); 

        erode(framePurple, framePurple, getStructuringElement(MORPH_ELLIPSE, Size(5, 5)) );
        dilate(framePurple, framePurple, getStructuringElement(MORPH_ELLIPSE, Size(5, 5)) ); 


        dilate(frameRed, frameRed, getStructuringElement(MORPH_ELLIPSE, Size(3, 3)) ); //dilate then erode each color's frame to fill small gaps within detected objects
        erode(frameRed, frameRed, getStructuringElement(MORPH_ELLIPSE, Size(3, 3)) );

        dilate(frameBlue, frameBlue, getStructuringElement(MORPH_ELLIPSE, Size(5, 5)) ); 
        erode(frameBlue, frameBlue, getStructuringElement(MORPH_ELLIPSE, Size(5, 5)) );

        dilate(frameOrange, frameOrange, getStructuringElement(MORPH_ELLIPSE, Size(5, 5)) ); 
        erode(frameOrange, frameOrange, getStructuringElement(MORPH_ELLIPSE, Size(5, 5)) );

        dilate(framePurple, framePurple, getStructuringElement(MORPH_ELLIPSE, Size(5, 5)) ); 
        erode(framePurple, framePurple, getStructuringElement(MORPH_ELLIPSE, Size(5, 5)) );

        Moments oMomentsRed = moments(frameRed);        //track spatial moments of frame for each color
        Moments oMomentsBlue = moments(frameBlue);
        Moments oMomentsOrange = moments(frameOrange);
        Moments oMomentsPurple = moments(framePurple);

        double dM01Red = oMomentsRed.m01;               //get x-direction moments
        double dM10Red = oMomentsRed.m10;               //get y-direction moments
        double dAreaRed = oMomentsRed.m00;              //get moments' area

        double dM01Blue = oMomentsBlue.m01;
        double dM10Blue = oMomentsBlue.m10;
        double dAreaBlue = oMomentsBlue.m00;
        
        double dM01Orange = oMomentsOrange.m01;
        double dM10Orange = oMomentsOrange.m10;
        double dAreaOrange = oMomentsOrange.m00;

        double dM01Purple = oMomentsPurple.m01;
        double dM10Purple = oMomentsPurple.m10;
        double dAreaPurple = oMomentsPurple.m00;

        
        if (dAreaRed > 20000) {                 //smaller areas considered too small to be objects
            posXred = (int)(dM10Red / dAreaRed);    //x-value of object
            posYred = (int)(dM01Red / dAreaRed);    //y-value of object 
            
            //printf("Red: X:%d   Y:%d\n", posXred, posYred);
            Rect objBox(posXred, posYred, 15, 15);  
            rectangle(frame, objBox, Scalar(0, 0, 255));    //show box over object in output screen
        }
        else {      //no red object present
            posXred = -1;
            posYred = -1;  
        }

        if (dAreaBlue > 10000) {    //blue is usually harder to detect, so a smaller area is given
            posXblue = (int)(dM10Blue / dAreaBlue);
            posYblue = (int)(dM01Blue / dAreaBlue);        
            
            //printf("Blue: X:%d   Y:%d\n", posXblue, posYblue);
            Rect objBox(posXblue, posYblue, 15, 15);
            rectangle(frame, objBox, Scalar(255, 0, 0));
        }
        else {
            posXblue = -1;
            posYblue = -1;  
        }

        if (dAreaOrange > 20000) {
            posXorange = (int)(dM10Orange / dAreaOrange);
            posYorange = (int)(dM01Orange / dAreaOrange);        
            
            //printf("Yellow: X:%d   Y:%d\n", posXyellow, posYyellow);
            Rect objBox(posXorange, posYorange, 15, 15);
            rectangle(frame, objBox, Scalar(0, 128, 255));
        }
        else {
            posXorange = -1;
            posYorange = -1;  
        }

        if (dAreaPurple > 20000) {
            posXpurple = (int)(dM10Purple / dAreaPurple);
            posYpurple = (int)(dM01Purple / dAreaPurple);        
            
            //printf("Yellow: X:%d   Y:%d\n", posXyellow, posYyellow);
            Rect objBox(posXpurple, posYpurple, 15, 15);
            rectangle(frame, objBox, Scalar(255, 0, 128));
        }
        else {
            posXpurple = -1;
            posYpurple = -1;  
        }
        
        imshow("Video", frame);                //output full color frame and frames detecting each color - for monitoring purposes
        imshow("Red Detect", frameRed);
        imshow("Blue Detect", frameBlue);
        imshow("Orange Detect", frameOrange);
        imshow("Purple Detect", framePurple);

        if (posXred >= 0 || posYred >= 0) { //red object in camera frame
         
            //int redXcounter = 14;     //previously initialized values for X & Y values and used for-loop to get final X & Y values for each color
            //int redYcounter = 15;     //only showing for red for line space

            /*for (int i = 0; i < 15; ++i) {
                if ((posXred - 15) >= 0) {
                    posXred -= 15;
                    --redXcounter;
                }
                else{
                    i = 15;
                }
            }
            for (int i = 0; i < 15; ++i ) {
                if ((posYred - 15) >= 0) {
                    posYred -= 15;
                    --redYcounter;
                }
                else{
                    i = 15;
                }
            }*/

           int redXcounter;         //holders for x & y coordinates for claw machine grid
           int redYcounter;
        
           if (posXred < 10) {      //hard-coded values for range to convert frame coordinates to grid coordinates
            redXcounter = 15;
           }
           else 
           if (posXred < 15) {
            redXcounter = 14;
           }
           else 
           if (posXred < 30) {
            redXcounter = 13;
           }
           else 
           if (posXred < 50) {
            redXcounter = 12;
           }
           else 
           if (posXred < 65) {
            redXcounter = 11;
           }
           else 
           if (posXred < 80) {
            redXcounter = 10;
           }
           else 
           if (posXred < 98) {
            redXcounter = 9;
           }
           else if (posXred < 112) {
            redXcounter = 8;
           }
           else if (posXred < 126) {
            redXcounter = 7;
           }
           else if (posXred < 140) {
            redXcounter = 6;
           }
           else if (posXred < 154) {
            redXcounter = 5;
           }
           else if (posXred < 168) {
            redXcounter = 4;
           }
           else if (posXred < 175) {
            redXcounter = 3;
           }
           else if (posXred < 185) {
            redXcounter = 2;
           }
           else if (posXred < 195) {
            redXcounter = 1;
           }
           else {
            redXcounter = 0;
           }


           

           if (posYred < 10) {
            redYcounter = 15;
           }
           else 
           if (posYred < 15) {
            redYcounter = 14;
           }
           else 
           if (posYred < 27) {
            redYcounter = 13;
           }
           else 
           if (posYred < 35) {
            redYcounter = 12;
           }
           else 
           if (posYred < 45) {
            redYcounter = 11;
           }
           else 
           if (posYred < 55) {
            redYcounter = 10;
           }
           else 
           if (posYred < 64) {
            redYcounter = 9;
           }
           else if (posYred < 73) {
            redYcounter = 8;
           }
           else if (posYred < 82) {
            redYcounter = 7;
           }
           else if (posYred < 91) {
            redYcounter = 6;
           }
           else if (posYred < 100) {
            redYcounter = 5;
           }
           else if (posYred < 110) {
            redYcounter = 4;
           }
           else if (posYred < 120) {
            redYcounter = 3;
           }
           else if (posYred < 130) {
            redYcounter = 2;
           }
           else if (posYred < 140) {
            redYcounter = 1;
           }
           else {
            redYcounter = 0;
           }

            posXred = redXcounter;      //move holder values into color coordinate variables
            posYred = redYcounter;
        }

        if (posXblue >= 0 || posYblue >= 0) {
            int blueXcounter;
            int blueYcounter;
            
           if (posXblue < 10) {
            blueXcounter = 15;
           }
           else if (posXblue < 15) {
            blueXcounter = 14;
           }
           else if (posXblue < 30) {
            blueXcounter = 13;
           }
           else if (posXblue < 50) {
            blueXcounter = 12;
           }
           else if (posXblue < 65) {
            blueXcounter = 11;
           }
           else if (posXblue < 80) {
            blueXcounter = 10;
           }
           else if (posXblue < 98) {
            blueXcounter = 9;
           }
           else if (posXblue < 112) {
            blueXcounter = 8;
           }
           else if (posXblue < 126) {
            blueXcounter = 7;
           }
           else if (posXblue < 140) {
            blueXcounter = 6;
           }
           else if (posXblue < 154) {
            blueXcounter = 5;
           }
           else if (posXblue < 168) {
            blueXcounter = 4;
           }
           else if (posXblue < 175) {
            blueXcounter = 3;
           }
           else if (posXblue < 185) {
            blueXcounter = 2;
           }
           else if (posXblue < 195) {
            blueXcounter = 1;
           }
           else {
            blueXcounter = 0;
           }


           if (posYblue < 5) {
            blueYcounter = 15;
           }
           else 
           if (posYblue < 10) {
            blueYcounter = 14;
           }
           else 
           if (posYblue < 15) {
            blueYcounter = 13;
           }
           else 
           if (posYblue < 20) {
            blueYcounter = 12;
           }
           else 
           if (posYblue < 30) {
            blueYcounter = 11;
           }
           else 
           if (posYblue < 35) {
            blueYcounter = 10;
           }
           else 
           if (posYblue < 50) {
            blueYcounter = 9;
           }
           else if (posYblue < 65) {
            blueYcounter = 8;
           }
           else if (posYblue < 75) {
            blueYcounter = 7;
           }
           else if (posYblue < 88) {
            blueYcounter = 6;
           }
           else if (posYblue < 94) {
            blueYcounter = 5;
           }
           else if (posYblue < 100) {
            blueYcounter = 4;
           }
           else if (posYblue < 105) {
            blueYcounter = 3;
           }
           else if (posYblue < 110) {
            blueYcounter = 2;
           }
           else if (posYblue < 120) {
            blueYcounter = 1;
           }
           else {
            blueYcounter = 0;
           }

            posXblue = blueXcounter;
            posYblue = blueYcounter;
        }

        if (posXorange >= 0 || posYorange >= 0) {
            int orangeXcounter;
            int orangeYcounter;
            
           if (posXorange < 10) {
            orangeXcounter = 15;
           }
           else 
           if (posXorange < 15) {
            orangeXcounter = 14;
           }
           else 
           if (posXorange < 30) {
            orangeXcounter = 13;
           }
           else 
           if (posXorange < 50) {
            orangeXcounter = 12;
           }
           else 
           if (posXorange < 65) {
            orangeXcounter = 11;
           }
           else 
           if (posXorange < 80) {
            orangeXcounter = 10;
           }
           else 
           if (posXorange < 98) {
            orangeXcounter = 9;
           }
           else if (posXorange < 112) {
            orangeXcounter = 8;
           }
           else if (posXorange < 126) {
            orangeXcounter = 7;
           }
           else if (posXorange < 140) {
            orangeXcounter = 6;
           }
           else if (posXorange < 154) {
            orangeXcounter = 5;
           }
           else if (posXorange < 168) {
            orangeXcounter = 4;
           }
           else if (posXorange < 175) {
            orangeXcounter = 3;
           }
           else if (posXorange < 185) {
            orangeXcounter = 2;
           }
           else if (posXorange < 195) {
            orangeXcounter = 1;
           }
           else {
            orangeXcounter = 0;
           }


           if (posYorange < 10) {
            orangeYcounter = 15;
           }
           else 
           if (posYorange < 15) {
            orangeYcounter = 14;
           }
           else 
           if (posYorange < 27) {
            orangeYcounter = 13;
           }
           else 
           if (posYorange < 35) {
            orangeYcounter = 12;
           }
           else 
           if (posYorange < 45) {
            orangeYcounter = 11;
           }
           else 
           if (posYorange < 55) {
            orangeYcounter = 10;
           }
           else 
           if (posYorange < 64) {
            orangeYcounter = 9;
           }
           else if (posYorange < 73) {
            orangeYcounter = 8;
           }
           else if (posYorange < 82) {
            orangeYcounter = 7;
           }
           else if (posYorange < 91) {
            orangeYcounter = 6;
           }
           else if (posYorange < 100) {
            orangeYcounter = 5;
           }
           else if (posYorange < 110) {
            orangeYcounter = 4;
           }
           else if (posYorange < 120) {
            orangeYcounter = 3;
           }
           else if (posYorange < 130) {
            orangeYcounter = 2;
           }
           else if (posYorange < 140) {
            orangeYcounter = 1;
           }
           else {
            orangeYcounter = 0;
           }

            posXorange = orangeXcounter;
            posYorange = orangeYcounter;
        }

        if (posXpurple >= 0 || posYpurple >= 0) {
            int purpleXcounter;
            int purpleYcounter;
            
           if (posXpurple < 10) {
            purpleXcounter = 15;
           }
           else 
           if (posXpurple < 15) {
            purpleXcounter = 14;
           }
           else 
           if (posXpurple < 30) {
            purpleXcounter = 13;
           }
           else 
           if (posXpurple < 50) {
            purpleXcounter = 12;
           }
           else 
           if (posXpurple < 65) {
            purpleXcounter = 11;
           }
           else 
           if (posXpurple < 80) {
            purpleXcounter = 10;
           }
           else 
           if (posXpurple < 98) {
            purpleXcounter = 9;
           }
           else if (posXpurple < 112) {
            purpleXcounter = 8;
           }
           else if (posXpurple < 126) {
            purpleXcounter = 7;
           }
           else if (posXpurple < 140) {
            purpleXcounter = 6;
           }
           else if (posXpurple < 154) {
            purpleXcounter = 5;
           }
           else if (posXpurple < 168) {
            purpleXcounter = 4;
           }
           else if (posXpurple < 175) {
            purpleXcounter = 3;
           }
           else if (posXpurple < 185) {
            purpleXcounter = 2;
           }
           else if (posXpurple < 195) {
            purpleXcounter = 1;
           }
           else {
            purpleXcounter = 0;
           }


           if (posYpurple < 10) {
            purpleYcounter = 15;
           }
           else 
           if (posYpurple < 15) {
            purpleYcounter = 14;
           }
           else 
           if (posYpurple < 27) {
            purpleYcounter = 13;
           }
           else 
           if (posYpurple < 35) {
            purpleYcounter = 12;
           }
           else 
           if (posYpurple < 45) {
            purpleYcounter = 11;
           }
           else 
           if (posYpurple < 55) {
            purpleYcounter = 10;
           }
           else 
           if (posYpurple < 64) {
            purpleYcounter = 9;
           }
           else if (posYpurple < 73) {
            purpleYcounter = 8;
           }
           else if (posYpurple < 82) {
            purpleYcounter = 7;
           }
           else if (posYpurple < 91) {
            purpleYcounter = 6;
           }
           else if (posYpurple < 100) {
            purpleYcounter = 5;
           }
           else if (posYpurple < 110) {
            purpleYcounter = 4;
           }
           else if (posYpurple < 120) {
            purpleYcounter = 3;
           }
           else if (posYpurple < 130) {
            purpleYcounter = 2;
           }
           else if (posYpurple < 140) {
            purpleYcounter = 1;
           }
           else {
            purpleYcounter = 0;
           }

            posXpurple = purpleXcounter;
            posYpurple = purpleYcounter;
        }

        if (serial != 0) {      //Serial port accessible
             
            ostringstream RedX;             //string buffers to hold coordinate values
            RedX << posXred;
            ostringstream RedY; 
            RedY << posYred;
            ostringstream BlueX;
            BlueX << posXblue;
            ostringstream BlueY; 
            BlueY << posYblue;
            ostringstream OrangeX; 
            OrangeX << posXorange;
            ostringstream OrangeY; 
            OrangeY << posYorange;
            ostringstream PurpleX; 
            PurpleX << posXpurple;
            ostringstream PurpleY; 
            PurpleY << posYpurple;


            string RX = RedX.str();         //convert string buffers into strings for Serial message
            string RY = RedY.str();
            string BX = BlueX.str(); 
            string BY = BlueY.str();
            string OX = OrangeX.str(); 
            string OY = OrangeY.str();
            string PX = PurpleX.str(); 
            string PY = PurpleY.str();
            string space = " ";         //space indicates end of each x or y coordinate
            string newline = "\n";      //newline indicates end of message

            string result = RX+space+RY+space+BX+space+BY+space+OX+space+OY+space+PX+space+PY+newline;      //desired output for Serial port
            //result = "Red-x-value Red-y-value Blue-x-value Blue-y-value Orange-x-value Orange-y-value Purplex-value Purple-y-value";
            
            cout << result << endl;
            
            char outputChars[24] = "c";         
            for (int i = 0; i < result.length(); ++i) {     //convert string into array of chars to send to Serial port
                outputChars[i] = result[i];
            }

            for (int i = 0; i < result.length(); ++i) {
                cout << outputChars[i];
            } 
            cout << endl;
           
		    WriteFile(hSerial, outputChars, strlen(outputChars), &btsIO, NULL); //write each char to Serial port

        }

        CloseHandle(hSerial);   //close Serial communication
        for (int j = 0; j < 1500000000; ++j){}  //delay to give time for Serial to read each output
    }
    
    return 0;
}