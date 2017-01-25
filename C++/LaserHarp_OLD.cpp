#include "LaserHarp.h"
using namespace std;
using namespace cv;

#define RADIUS		50



int main()
{
	Mat frame; // Create new frame
	VideoCapture capture = VideoCapture(0); // Create a capture device. Here 0 indicates that we want to use the default iSight camera
	namedWindow("Laser Harp", CV_WINDOW_NORMAL); // Create a new window

	int nThreshold = 75;
	createTrackbar("Threshold", "Laser Harp", &nThreshold, 255);

	bool bFullscreen = 0; // Fullscreen state
	//int nNumPoints = 0; // Number of notes being played

	while(true)
	{
		capture.read(frame); // Query for frame from camera

		// Split the frame into its component channels
		Mat redChannel(frame.rows, frame.cols, CV_8UC1);
		int anFromTo[] = {2,0};
		mixChannels(&frame, 1, &redChannel, 1, anFromTo, 1);
		Mat greenChannel(frame.rows, frame.cols, CV_8UC1);
		anFromTo[0] = 1;
		mixChannels(&frame, 1, &greenChannel, 1, anFromTo, 1);
		Mat blueChannel(frame.rows, frame.cols, CV_8UC1);
		anFromTo[0] = 0;
		mixChannels(&frame, 1, &blueChannel, 1, anFromTo, 1);

		// Calculate (red - green - blue) to find the brightest red points
		Mat maxRed(frame.rows, frame.cols, CV_8UC1);
		subtract(redChannel, greenChannel, maxRed);
		subtract(maxRed, blueChannel, maxRed);

		// Calculate (green - red - blue) to find the brightest green points
		Mat maxGreen(frame.rows, frame.cols, CV_8UC1);
		subtract(greenChannel, redChannel, maxGreen);
		subtract(maxGreen, blueChannel, maxGreen);

		// Calculate (blue - red - green) to find the brightest blue points
		Mat maxBlue(frame.rows, frame.cols, CV_8UC1);
		subtract(blueChannel, redChannel, maxBlue);
		subtract(maxBlue, greenChannel, maxBlue);

		ostringstream oStream; // Text stream
		string sLaser(""); // Text string

		Point origin;
		minMaxLoc(maxBlue, NULL, NULL, NULL, &origin);
		uint8_t nOrigin = maxBlue.at<uint8_t>(origin);
		//cout << (int)nOrigin << endl;
		if (nOrigin > 200)
		{
			circle(frame, origin, 16, Scalar(255, 0, 0), -1); // Draw a circle around first point
			line(frame, Point(origin.x, 0), Point(origin.x, frame.rows), Scalar(255, 0, 0), 2);
			line(frame, Point(0, origin.y), Point(frame.cols, origin.y), Scalar(255, 0, 0), 2);
		}
		else {
			origin = Point(frame.cols / 2, frame.rows);
		}

		while (findPoint() > nThreshold)
		{
		}

		// Find the first point of the laser
		Point laser1;
		minMaxLoc(maxGreen, NULL, NULL, NULL, &laser1);
		uint8_t nLaser1 = maxGreen.at<uint8_t>(laser1);

		circle(maxGreen, laser1, RADIUS, Scalar(0, 0, 0), -1); // Block out a radius of pixels around the first point

		// Find the second point of the laser
		Point laser2;
		minMaxLoc(maxGreen, NULL, NULL, NULL, &laser2);
		uint8_t nLaser2 = maxGreen.at<uint8_t>(laser2);

		// Ensure that point 1 is the leftmost point
		if (laser1.x > laser2.x)
		{
			Point temp = laser1;
			laser1 = laser2;
			laser2 = temp;
		}

		if (nLaser1 > nThreshold) // Check if point 1 is above the threshold
		{
			double fAngle1 = atan2(abs(laser1.y - origin.y), abs(laser1.x - origin.x)) * 180 / PI; // Calculate the angle between point 1 and the horizontal

			circle(frame, laser1, 16, Scalar(0, 255, 0), -1); // Draw a circle around first point
			line(frame, origin, laser1, Scalar(0, 255, 0), 2);
			putText(frame, "PT1", Point(laser1.x - 13, laser1.y + 5), CV_FONT_HERSHEY_SIMPLEX, 0.5, Scalar(0, 0, 0), 2); // Put the name on the screen

			// Write the location of point 1 to the screen
			oStream << laser1.x;
			sLaser = ("Point 1: (" + oStream.str() + ",");
			oStream.str("");
			oStream << laser1.y;
			sLaser += (oStream.str() + ") [Int: ");
			oStream.str("");
			oStream << (int)nLaser1;
			sLaser += (oStream.str() + "] [Deg: ");
			oStream.str("");
			oStream << (int)fAngle1;
			sLaser += (oStream.str() + "]");
		}

		if (nLaser2 > nThreshold) // Check if point 2 is above the threshold
		{
			double fAngle2 = atan2(abs(laser2.y - origin.y), abs(laser2.x - origin.x)) * 180 / PI; // Calculate the angle between point 2 and the horizontal

			circle(frame, laser2, 16, Scalar(0, 255, 0), -1); // Draw a circle around second point
			line(frame, origin, laser2, Scalar(0, 255, 0), 2);
			putText(frame, "PT2", Point(laser2.x - 13, laser2.y + 5), CV_FONT_HERSHEY_SIMPLEX, 0.5, Scalar(0, 0, 0), 2); // Put the name on the screen

			// Write the location of point 2 to the screen
			oStream.str("");
			oStream << laser2.x;
			sLaser += (", Point 2: (" + oStream.str() + ",");
			oStream.str("");
			oStream << laser2.y;
			sLaser += (oStream.str() + ") [Int: ");
			oStream.str("");
			oStream << (int)nLaser2;
			sLaser += (oStream.str() + "] [Deg: ");
			oStream.str("");
			oStream << (int)fAngle2;
			sLaser += (oStream.str() + "]");
		}

		putText(frame, sLaser.c_str(), Point(0, frame.rows - 8), CV_FONT_HERSHEY_SIMPLEX, 0.5, Scalar(255, 255, 255)); // Put the text on the screen

		char chInput = waitKey(1); // Wait for 1 ms for user to hit any key

		if (chInput == 's') // Save image
			imwrite("/Users/jordi/Desktop/Current Projects/Laser Harp/opencv.png", frame);
		else if (chInput == 'f') // Toggle fullscreen state
		{
			bFullscreen = !bFullscreen;

			if (bFullscreen)
				setWindowProperty("Laser Harp", CV_WND_PROP_FULLSCREEN, CV_WINDOW_FULLSCREEN);
			else
				setWindowProperty("Laser Harp", CV_WND_PROP_FULLSCREEN, CV_WINDOW_NORMAL);
		}
		else if (chInput == 27) // Exit the loop
			break;

		if (bFullscreen)
		{
			// Resize the frame to fill the whole screen
			Mat frameScaled(1050, 1680, CV_8UC3);
			resize(frame, frameScaled, frameScaled.size());

			imshow("Laser Harp", frameScaled); // Display the captured image
		}
		else
			imshow("Laser Harp", frame); // Display the captured image
	}

	capture.release(); // Release image
	destroyWindow("Laser Harp"); // Release window

	return 0;
}

uint8_t findPoints()
{
	return 0;
}