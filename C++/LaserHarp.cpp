#include "LaserHarp.h"
using namespace std;
using namespace cv;

#define RADIUS 75 // Smallest distance between points
#define STEPS 4 // Number of steps between each beam

Mat frameThresholded;
Point *points = new Point[1];
int anNotes[13];
int nInstrument = 52;
bool bRecord = 1;



int main()
{
	Mat frame; // Create new frame
	VideoCapture capture = VideoCapture(0); // Create a capture device. Here 0 indicates that we want to use the default iSight camera
	namedWindow("Laser Harp Threshold", CV_WINDOW_NORMAL); // Create a new window
	namedWindow("Laser Harp", CV_WINDOW_NORMAL); // Create a new window
	moveWindow("Laser Harp", 200, 165);

	namedWindow("Control Panel");
	createTrackbar("MIDI Instrument", "Laser Harp", &nInstrument, 127);

	bool bFullscreen = 0; // Fullscreen state

	while (true)
	{
		capture.read(frame); // Query for frame from camera
		
		flip(frame, frame, 1); // Flip frame horizontally
		cvtColor(frame, frameThresholded, CV_BGR2HSV); // Convert from BGR to HSV color
		//inRange(frameThresholded, Scalar(0, 0, 245), Scalar(180, 5, 255), frameThresholded); // Threshold the image
		inRange(frameThresholded, Scalar(100, 0, 245), Scalar(140, 15, 255), frameThresholded); // Threshold the image
		dilate(frameThresholded, frameThresholded, getStructuringElement(MORPH_RECT, Size(3, 3))); // Fill in all remaining spots
		imshow("Laser Harp Threshold", frameThresholded); // Display the thresholded image

		Point origin = Point(frame.cols / 2, frame.rows); // The origin point
		//circle(frame, origin, 16, Scalar(255, 0, 0), -1); // Draw a circle around the origin

		for (int i = 0; i < 13; i++) {
			anNotes[i] = 0;
		}
		
		uint8_t nNumPoints = findPoints(); // Find all points
		for (uint8_t i = 0; i < nNumPoints; i++)
		{
			double fAngle = atan2(abs(points[i].x - origin.x), abs(points[i].y - origin.y)) * 180 / PI; // Calculate the angle between the point and the horizontal
			if (points[i].x > origin.x) {
				fAngle += 90;
			}
			else {
				fAngle = 90 - fAngle;
			}
			double fRadius = sqrt((points[i].y - origin.y) * (points[i].y - origin.y) + (points[i].x - origin.x) * (points[i].x - origin.x));

			line(frame, origin, points[i], Scalar(255, 255, 255), 2, CV_AA); // Draw a line from the origin to the point
			circle(frame, points[i], 16, Scalar(255, 255, 255), -1); // Draw a white circle around the point
			circle(frame, points[i], 16, Scalar(0, 0, 0), 1, CV_AA); // Draw a black outline around said circle

			string note = playNote(fAngle, fRadius);
			putText(frame, note, Point(points[i].x - 5 * note.length(), points[i].y + 5), CV_FONT_HERSHEY_SIMPLEX, 0.5, Scalar(0, 0, 0), 1, CV_AA); // Put the name on the screen
		}

		ofstream outf("/Users/jordi/Desktop/Current Projects/Laser Harp/Java/resources/midi.dat");
		outf << "0" << endl << nInstrument << endl;
		for (int i = 0; i < 13; i++) {
			outf << anNotes[i] << " ";
		}
		outf.seekp(0, ios::beg);
		outf << "1";
		outf.close();
		
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

		if (bRecord) {
			putText(frame, "REC", Point(5, 50), CV_FONT_HERSHEY_SIMPLEX, 2, Scalar(255, 255, 255), 1, CV_AA);
		}

		if (bFullscreen)
		{
			// Resize the frame to fill the whole screen
			Mat frameScaled(1050, 1680, CV_8UC3);
			resize(frame, frameScaled, frameScaled.size());

			imshow("Laser Harp", frameScaled); // Display the captured image
		}
		else
		{
			imshow("Laser Harp", frame); // Display the captured image
		}
	}

	capture.release(); // Release image
	destroyAllWindows(); // Release windows

	return 0;
}

uint8_t findPoints()
{
	uint8_t nValue = 255;
	uint8_t nNumPoints = 0;

	delete[] points;
	points = new Point[1];

	while (nValue)
	{
		minMaxLoc(frameThresholded, NULL, NULL, NULL, &points[nNumPoints]); // Find a point
		nValue = frameThresholded.at<uint8_t>(points[nNumPoints]); // Get the value of that point
		circle(frameThresholded, points[nNumPoints], RADIUS, 0, -1); // Block out a radius of pixels around the point

		if (nValue)
		{
			if (nNumPoints == 255) {
				break;
			}
			
			nNumPoints++;

			Point *pTemp = new Point[nNumPoints + 1];
			for (int i = 0; i < nNumPoints; i++) {
				pTemp[i] = points[i];
			}
			delete[] points;

			points = pTemp;
			pTemp = 0;
		}
		else {
			break;
		}
	}

	return nNumPoints;
}

string playNote(double fAngle, double fRadius)
{
	fAngle /= 1.8 * STEPS; // Convert from degrees to steps
	fAngle -= 20 / STEPS; // Shift so that F# is in the center

	fRadius /= 5.625; // Remap from 0-720 to 0-127
	//fRadius = 127; // TEST

	for (int i = 0; i < 13; i++)
	{
		if ((int)fAngle == i + 1) {
			anNotes[i] = (int)fRadius;
		}
	}

	switch ((int)fAngle)
	{
		case 1:
			anNotes[0] = (int)fRadius;
			return "C";
		case 2:
			anNotes[1] = (int)fRadius;
			return "C#";
		case 3:
			anNotes[2] = (int)fRadius;
			return "D";
		case 4:
			anNotes[3] = (int)fRadius;
			return "D#";
		case 5:
			anNotes[4] = (int)fRadius;
			return "E";
		case 6:
			anNotes[5] = (int)fRadius;
			return "F";
		case 7:
			anNotes[6] = (int)fRadius;
			return "F#";
		case 8:
			anNotes[7] = (int)fRadius;
			return "G";
		case 9:
			anNotes[8] = (int)fRadius;
			return "G#";
		case 10:
			anNotes[9] = (int)fRadius;
			return "A";
		case 11:
			anNotes[10] = (int)fRadius;
			return "A#";
		case 12:
			anNotes[11] = (int)fRadius;
			return "B";
		case 13:
			anNotes[12] = (int)fRadius;
			return "C";
	}

	return "NaN";
}