#include "opencv2/opencv.hpp"
#include <filesystem>
#include <iostream>

using namespace std;
using namespace cv;
void detectAndDisplay(Mat);
void test();
void detect(int);

CascadeClassifier face_cascade;

int main(int argc, const char** argv)
{
    CommandLineParser parser(argc, argv,
        "{help h ?  |       |                           }"
        "{test      |       |Enable test mode.          }"
        "{camera    |0      |Camera number.             }"
        "{audio     |1      |Enable audio (0/1).        }");
    parser.about("MaskedFaceDetection v0.1");
    if (parser.has("help")) {
        parser.printMessage();
        return 0;
    }
    String face_cascade_name = samples::findFile("EnhancedCascade.xml");
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
    std::string path = "A:\\Documents\\FinalYearProject\\FMLD-main\\test-images\\images";
    for (const auto& entry : filesystem::directory_iterator(path)) {
        Mat img = imread(entry.path().u8string());
        while (waitKey(10) != 27) {
            if (img.empty())
            {
                cout << "IMAGE NOT FOUND: " << entry.path() << endl;
                break;
            }
            detectAndDisplay(img);
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

    Mat frame;
    while (capture.read(frame))
    {
        if (frame.empty())
        {
            cout << "CAMERA UNRESPONSIVE" << endl;
        }
        detectAndDisplay(frame);

    }
}

void detectAndDisplay(Mat frame)
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
    namedWindow("Detection", CV_WINDOW_NORMAL);
    imshow("Detection", frame);
}