#include "opencv2/opencv.hpp"
#include <filesystem>
#include <iostream>

using namespace std;
using namespace cv;
void detectAndDisplay(Mat frame);
void test();
void detect();

CascadeClassifier face_cascade;

int main(int argc, const char** argv)
{
    CommandLineParser parser(argc, argv,
        "{help h ?  |       |                           }"
        "{test      |0      |Enable test mode (0/1)     }"
        "{camera    |0      |Camera number.             }"
        "{audio     |1      |Enable audio (0/1)         }");
    parser.about("MaskedFaceDetection v0.1");
    String face_cascade_name = samples::findFile("maskedFaceCascade.xml");
    if (!face_cascade.load(face_cascade_name))
    {
        cout << "CASCADE LOAD ERROR" << endl;
        return -1;
    };
    
    if (parser.get<int>("test")) {
        test();
    }
    else {
        detect();
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

void detect() {
    /*
    int camera_device = parser.get<int>("camera");
    VideoCapture capture;
    //-- 2. Read the video stream
    capture.open(camera_device);
    if (!capture.isOpened())
    {
        cout << "--(!)Error opening video capture\n";
        return -1;
    }

    Mat frame;
    while (capture.read(frame))
    {
    */

    std::string path = "A:\\Documents\\FinalYearProject\\FMLD-main\\test-images\\images";
    for (const auto& entry : filesystem::directory_iterator(path)) {
        while (waitKey(10) != 27) {

            Mat img = imread(entry.path().u8string());
            if (img.empty())
            {
                cout << "--(!) No image -- Break!\n";
            }
            //-- 3. Apply the classifier to the frame
            detectAndDisplay(img);

        }
    }
    //}
}

void detectAndDisplay(Mat frame)
{
    Mat frame_gray;
    cvtColor(frame, frame_gray, COLOR_BGR2GRAY);
    equalizeHist(frame_gray, frame_gray);
    //-- Detect faces
    std::vector<Rect> faces;
    face_cascade.detectMultiScale(frame_gray, faces);
    for (size_t i = 0; i < faces.size(); i++)
    {
        Point center(faces[i].x + faces[i].width / 2, faces[i].y + faces[i].height / 2);
        rectangle(frame, faces.at(i), Scalar(255, 0, 255), 4);
        Mat faceROI = frame_gray(faces[i]);
    }
    //-- Show what you got
    namedWindow("Detection", CV_WINDOW_NORMAL);
    imshow("Detection", frame);
}