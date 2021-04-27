#include "opencv2/opencv.hpp"
#include <filesystem>
#include <iostream>

using namespace std;
using namespace cv;
void testDetectAndDisplay(Mat);
void test();
void detect(int);
vector<Rect> maskedDetection(Mat, int&, vector<Rect>);

CascadeClassifier face_cascade;

int main(int argc, const char** argv)
{
    CommandLineParser parser(argc, argv,
        "{help h ?  |       |                           }"
        "{test      |       |Enable test mode.          }"
        "{camera    |0      |Camera number.             }"
        "{audio     |1      |Enable audio (0/1).        }");
    parser.about("MaskedFaceDetection v0.3");
    if (parser.has("help")) {
        parser.printMessage();
        return 0;
    }
    String face_cascade_name = samples::findFile("data/maskedFaceCascade.xml");
    if (!face_cascade.load(face_cascade_name))
    {
        cout << "CASCADE LOAD ERROR" << endl;
        return -1;
    };
    
    
    if (parser.has("test")) {
        test();
    }
    else {
        detect(parser.get<int>("camera"));
    }
    return 0;
}

void test() {
	namedWindow("Detection", CV_WINDOW_NORMAL);
    std::string path = "data/images";
    for (const auto& entry : filesystem::directory_iterator(path)) {
        Mat img = imread(entry.path().u8string());
        while (waitKey(10) != 27) {
            if (img.empty())
            {
                cout << "IMAGE NOT FOUND: " << entry.path() << endl;
                break;
            }
            testDetectAndDisplay(img);
        }
    }
}

void detect(int camera_device) {
    VideoCapture capture;
    capture.open(camera_device);
    if (!capture.isOpened())
    {
        cout << "ERROR STARTING CAPTURE (CHECK CAMERA)" << endl;
        return;
    }
	namedWindow("Detection", CV_WINDOW_NORMAL);
    Mat frame;
	vector<Rect> prev = {};
	int frameCount = 0;
    while (capture.read(frame))
    {
		if (frameCount > 2) {

		}
        if (frame.empty())
        {
            cout << "CAMERA UNRESPONSIVE" << endl;
			break;
        }
		if (waitKey(10) == 27) {
			break;
		}
        prev = maskedDetection(frame, frameCount, prev);
    }
}

void testDetectAndDisplay(Mat frame)
{
    Mat frame_gray;
    cvtColor(frame, frame_gray, COLOR_BGR2GRAY);
    equalizeHist(frame_gray, frame_gray);
    std::vector<Rect> faces;
    face_cascade.detectMultiScale(frame_gray, faces);
    for (size_t i = 0; i < faces.size(); i++)
    {
        Point center(faces[i].x + faces[i].width / 2, faces[i].y + faces[i].height / 2);
        rectangle(frame, faces.at(i), Scalar(255, 0, 255), 4);
        Mat faceROI = frame_gray(faces[i]);
    }
    
    imshow("Detection", frame);
}

vector<Rect> maskedDetection(Mat frame, int& frameCount, vector<Rect> prevFaces)
{
	if (prevFaces.empty()) {
		frameCount = 0;
	}
	Mat frame_gray;
	cvtColor(frame, frame_gray, COLOR_BGR2GRAY);
	equalizeHist(frame_gray, frame_gray);
	vector<Rect> faces;
	face_cascade.detectMultiScale(frame_gray, faces);
	for (size_t i = 0; i < faces.size(); i++)
	{
		for (Rect& f : prevFaces){
			if (faces[i].area() > f.area()*0.9 && faces[i].area() < f.area()*1.1) {
				++frameCount;
				break;
			}
		}
		Point center(faces[i].x + faces[i].width / 2, faces[i].y + faces[i].height / 2);
		rectangle(frame, faces.at(i), frameCount > 2 ? Scalar(0, 255, 0) : Scalar(255, 0, 255), 4);
		Mat faceROI = frame_gray(faces[i]);
		if (frameCount > 2) {
			putText(frame, "Please enter", Point(faces[i].x, faces[i].y - 10), FONT_HERSHEY_SIMPLEX, 0.8, Scalar(0, 255, 0), 2);
		}
	}
	putText(frame, "Please look at the screen", Point(0, 30), FONT_HERSHEY_SIMPLEX, 1, Scalar(0, 0, 0), 2);
	imshow("Detection", frame);
	return faces;
}