//namedWindow("Laser Harp Threshold", CV_WINDOW_NORMAL); // Create a new window
//namedWindow("Laser Harp", CV_WINDOW_NORMAL); // Create a new window
//moveWindow("Laser Harp", 200, 165);

boolean bFullscreen = false; // Fullscreen state

while (true)
{
	try
	{
		//System.out.println("Hello");
		capture.read(frame); // Query for frame from camera
		//System.out.println(" World!");

		showResult(frame);

		Imgproc.cvtColor(frame, frameThresholded, Imgproc.COLOR_BGR2HSV, 3); // Convert from BGR to HSV color
		Core.inRange(frameThresholded, new Scalar(0, 0, 245), new Scalar(180, 5, 255), frameThresholded); // Threshold the image
		Imgproc.dilate(frameThresholded, frameThresholded, Imgproc.getStructuringElement(Imgproc.MORPH_RECT, new Size(3, 3))); // Fill in all remaining spots
		//imshow("Laser Harp Threshold", frameThresholded); // Display the thresholded image
		//showResult(frameThresholded);

		System.out.println(frame.depth());

		Point origin = new Point(frame.cols() / 2, frame.rows()); // The origin point
		//circle(frame, origin, 16, new Scalar(255, 0, 0), -1, 0); // Draw a circle around the origin
		
		int nNumPoints = findPoints(); // Find all points
		for (int i = 0; i < nNumPoints; i++)
		{
			double fAngle = Math.atan2(Math.abs(points[i].x - origin.x), Math.abs(points[i].y - origin.y)) * 180 / PI; // Calculate the angle between the point and the horizontal
			if (points[i].x > origin.x) {
				fAngle += 90;
			}
			else {
				fAngle = 90 - fAngle;
			}
			double nRadius = Math.sqrt((points[i].y - origin.y) * (points[i].y - origin.y) + (points[i].x - origin.x) * (points[i].x - origin.x));

			Core.line(frame, origin, points[i], new Scalar(255, 255, 255), 2, Core.LINE_AA, 0); // Draw a line from the origin to the point
			Core.circle(frame, points[i], 16, new Scalar(255, 255, 255), -1); // Draw a white circle around the point
			Core.circle(frame, points[i], 16, new Scalar(0, 0, 0), 1, Core.LINE_AA, 0); // Draw a black outline around said circle

			String note = playNote(fAngle, nRadius);
			//putText(frame, note, new Point(points[i].x - 5 * note.length(), points[i].y + 5), Core.FONT_HERSHEY_SIMPLEX, 0.5, new Scalar(0, 0, 0), 1, Core.LINE_AA); // Put the name on the screen
		}
		
		//char chInput = waitKey(1); // Wait for 1 ms for user to hit any key
		//char chInput = (char)System.in.read();
		char chInput = 'a';

		// Save image
		if (chInput == 's') {
			//imwrite("/Users/jordi/Desktop/Current Projects/Laser Harp/opencv.png", frame);
		}
		// Toggle fullscreen state
		else if (chInput == 'f')
		{
			bFullscreen = !bFullscreen;

			/*
			if (bFullscreen)
				setWindowProperty("Laser Harp", CV_WND_PROP_FULLSCREEN, CV_WINDOW_FULLSCREEN);
			else
				setWindowProperty("Laser Harp", CV_WND_PROP_FULLSCREEN, CV_WINDOW_NORMAL);
			*/
		}
		// Exit the loop (27)
		else if (chInput == 'q') {
			break;
		}

		/*
		if (bFullscreen)
		{
			// Resize the frame to fill the whole screen
			Mat frameScaled = new Mat(1050, 1680, CV_8UC3);
			resize(frame, frameScaled, frameScaled.size());

			imshow("Laser Harp", frameScaled); // Display the captured image
		}
		else {
			imshow("Laser Harp", frame); // Display the captured image
		}
		*/
	}
	catch (Exception e) {}
}

capture.release(); // Release image
//destroyAllWindows(); // Release windows










// throws MidiUnavailableException

try
{
	Synthesizer synth = MidiSystem.getSynthesizer();
	synth.open();
	MidiChannel[] channels = synth.getChannels();

	// --------------------------------------
	// Play a few notes.
	// The two arguments to the noteOn() method are:
	// "MIDI note number" (pitch of the note),
	// and "velocity" (i.e., volume, or intensity).
	// Each of these arguments is between 0 and 127.
	channels[channel].noteOn( 60, volume ); // C note
	Thread.sleep( duration );
	channels[channel].noteOff( 60 );
	channels[channel].noteOn( 62, volume ); // D note
	Thread.sleep( duration );
	channels[channel].noteOff( 62 );
	channels[channel].noteOn( 64, volume ); // E note
	Thread.sleep( duration );
	channels[channel].noteOff( 64 );

	Thread.sleep( 500 );

	// --------------------------------------
	// Play a C major chord.
	channels[channel].noteOn( 60, volume ); // C
	channels[channel].noteOn( 64, volume ); // E
	channels[channel].noteOn( 67, volume ); // G
	Thread.sleep( 3000 );
	channels[channel].allNotesOff();
	Thread.sleep( 500 );



	synth.close();
}
catch (Exception e) {
	e.printStackTrace();
}










import java.io.*;

// Math stuff
import java.lang.Math;

// Window stuff
import java.awt.image.*;
import java.awt.Component.*;
import javax.swing.*;
import javax.imageio.*;

// OpenCV stuff
import org.opencv.core.*;
import org.opencv.highgui.*;
import org.opencv.imgproc.*;

// MIDI stuff
import javax.sound.midi.*;



public class LaserHarp
{
	// Constants
	private static double PI = 3.14159265;
	private static int RADIUS = 75;
	private static int nSteps = 4;
	private static int channel = 0; // 0 is a piano, 9 is percussion, other channels are for other instruments
	private static int volume = 127; // between 0 et 127

	private static Mat frameThresholded; // Thresholded frame
	private static Point points[]; // Array of points



	public static void main(String[] args) throws IOException, InterruptedException, MidiUnavailableException
	{
		System.loadLibrary(Core.NATIVE_LIBRARY_NAME); // Load OpenCV library

		try
		{
			//Synthesizer synth = MidiSystem.getSynthesizer();
			//synth.open();
			//MidiChannel[] channels = synth.getChannels(); // Instruments for synth

			//Thread.sleep(1000);

			JFrame jWindow = new JFrame("Laser Harp"); // Create a new window
			jWindow.setDefaultCloseOperation(JFrame.EXIT_ON_CLOSE); // Set it to exit upon clicking the close button
			//jWindow.setSize(1280, 720); // Set the size to 720p

			//Mat frame = new Mat(); // Create new frame
			VideoCapture capture = new VideoCapture(0); // Create a capture device. Here 0 indicates that we want to use the default iSight camera

			if (capture.isOpened())
			{
				Thread.sleep(1000);
				System.out.println("Camera opened: " + capture.toString());

				jWindow.setVisible(true);
				Mat frame = new Mat();

				while (true)
				{
					//Mat frame = new Mat();
					capture.read(frame); // Query for frame from camera

					if (!frame.empty())
					{
						System.out.println("Grabbed frame");
						//Thread.sleep(200);

						MatToBufImg converter = new MatToBufImg(frame, ".jpg");
						BufferedImage bufImg = converter.getBufferedImage();

						JFrame jWindow2 = new JFrame("Laser Harp 2"); // Create a new window
						jWindow2.setDefaultCloseOperation(JFrame.EXIT_ON_CLOSE); // Set it to exit upon clicking the close button

						jWindow2.add(new JLabel(new ImageIcon(bufImg)));
						jWindow2.pack();
						jWindow2.setVisible(true);

						jWindow2.revalidate();
						jWindow2.repaint();

						Thread.sleep(10);
					}
					else {
						System.out.println("Missed frame");
					}
				}
			}
			else {
				System.out.println("Could not open camera");
			}

			capture.release();
			//synth.close();
		} catch (Exception e) {
			e.printStackTrace();
		}
	}

	public static void showResult(Mat img)
	{
		//Imgproc.resize(img, img, new Size(1280, 720));
		MatOfByte matOfByte = new MatOfByte();
		Highgui.imencode(".jpg", img, matOfByte);
		byte[] byteArray = matOfByte.toArray();
		BufferedImage bufImage = null;
		try
		{
			InputStream in = new ByteArrayInputStream(byteArray);
			bufImage = ImageIO.read(in);
			JFrame frame = new JFrame();
			frame.getContentPane().add(new JLabel(new ImageIcon(bufImage)));
			frame.pack();
			frame.setVisible(true);
		}
		catch (Exception e) {
			e.printStackTrace();
		}
	}

	static int findPoints()
	{
		int nValue = 255;
		int nNumPoints = 0;

		while (nValue > 0)
		{
			int[] nTempVal = new int[1];
			points[nNumPoints] = Core.minMaxLoc(frameThresholded).maxLoc; // Find a point
			frameThresholded.get((int)points[nNumPoints].x, (int)points[nNumPoints].y, nTempVal); // Get the value of that point
			Core.circle(frameThresholded, points[nNumPoints], RADIUS, new Scalar(0), -1); // Block out a nRadius of pixels around the point

			nValue = nTempVal[0];

			if (nValue > 0)
			{
				if (nNumPoints == 255) {
					break;
				}
				
				nNumPoints++;
			}
			else {
				break;
			}
		}

		return nNumPoints;
	}

	static String playNote(double fAngle, double nRadius)
	{
		fAngle /= 1.8 * nSteps; // Convert from degrees to nSteps
		fAngle -= 20 / nSteps; // Shift so that F# is in the center

		System.out.println(nRadius);

		if ((int)fAngle == 1)
		{
			//channels[channel].noteOn(60, volume); // C note
			return "C";
		}
		else if ((int)fAngle == 2)
		{
			return "C#";
		}
		else if ((int)fAngle == 3)
		{
			return "D";
		}
		else if ((int)fAngle == 4)
		{
			return "D#";
		}
		else if ((int)fAngle == 5)
		{
			return "E";
		}
		else if ((int)fAngle == 6)
		{
			return "F";
		}
		else if ((int)fAngle == 7)
		{
			return "F#";
		}
		else if ((int)fAngle == 8)
		{
			return "G";
		}
		else if ((int)fAngle == 9)
		{
			return "G#";
		}
		else if ((int)fAngle == 10)
		{
			return "A";
		}
		else if ((int)fAngle == 11)
		{
			return "A#";
		}
		else if ((int)fAngle == 12)
		{
			return "B";
		}
		else if ((int)fAngle == 13)
		{
			return "C";
		}
		else {
			return "NaN";
		}
	}
}

class MatToBufImg
{
	private Mat matrix;
	private MatOfByte mob;
	private String fileExten;

	// The file extension string should be ".jpg", ".png", etc
	public MatToBufImg() {}

	// The file extension string should be ".jpg", ".png", etc
	public MatToBufImg(Mat amatrix, String fileExt)
	{
		matrix = amatrix;
		fileExten = fileExt;
		mob = new MatOfByte();
	}

	public void setMatrix(Mat amatrix, String fileExt )
	{
		matrix = amatrix;
		fileExten = fileExt;
		mob = new MatOfByte();
	}

	public BufferedImage getBufferedImage()
	{
		//convert the matrix into a matrix of bytes appropriate for
		//this file extension
		Highgui.imencode(fileExten, matrix, mob);

		//convert the "matrix of bytes" into a byte array
		byte[] byteArray = mob.toArray();
		BufferedImage bufImage = null;
		try
		{
			InputStream in = new ByteArrayInputStream(byteArray);
			bufImage = ImageIO.read(in);
		} catch (Exception e) {
			e.printStackTrace();
		}
		return bufImage;
	}
}