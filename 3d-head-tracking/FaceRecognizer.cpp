#include "FaceRecognizer.h"

#include <helpers.h>

FaceRecognizer::FaceRecognizer()
{
    flandmark = nullptr;
}

FaceRecognizer::~FaceRecognizer()
{
    delete flandmark;
}

void FaceRecognizer::init(const std::string& flandmark_path, const std::string& haarcascade_path)
{
    flandmark = clandmark::Flandmark::getInstanceOf(flandmark_path.c_str());

    if (!flandmark) throw std::runtime_error("Couldn't load the flandmark.");

    if (!face_cascade.load(haarcascade_path)) throw std::runtime_error("Couldn't load the haar cascade.");
}

void FaceRecognizer::update(cv::Mat& frame)
{
    std::vector<cv::Rect> faces;
    cv::Mat frame_gray;
    
    int bbox[8];
    clandmark::fl_double_t* landmarks;

    cv::cvtColor(frame, frame_gray, CV_BGR2GRAY);
    cv::equalizeHist(frame_gray, frame_gray);

    face_cascade.detectMultiScale(frame_gray, faces, 1.1, 2, 0 | CV_HAAR_SCALE_IMAGE, cv::Size(50, 50));
    
    cimg_library::CImg<unsigned char>* frm_gray = cvImgToCImg(frame_gray);

    float largest = 0;
    for (uint32_t i = 0; i < faces.size(); i++)
    {
        // Get detected face bounding box
        bbox[0] = faces[i].x;
        bbox[1] = faces[i].y;
        bbox[2] = faces[i].x + faces[i].width;
        bbox[3] = faces[i].y;
        bbox[4] = faces[i].x + faces[i].width;
        bbox[5] = faces[i].y + faces[i].height;
        bbox[6] = faces[i].x;
        bbox[7] = faces[i].y + faces[i].height;

        // Detect facial landmarks
        flandmark->detect(frm_gray, bbox);

        // Get detected landmarks
        landmarks = flandmark->getLandmarks();

        // Draw bounding box and detected landmarks
        cv::rectangle(frame, cv::Point(bbox[0], bbox[1]), cv::Point(bbox[4], bbox[5]), cv::Scalar(255, 0, 0));
        //cv::circle(frame, cv::Point(int(landmarks[0]), int(landmarks[1])), 2, cv::Scalar(255, 0, 0), -1);

        float top = FLT_MAX;
        for (int i = 2; i < 2 * flandmark->getLandmarksCount(); i += 2)
        {
            top = std::min(top, (float)landmarks[i + 1]);
            //cv::circle(frame, cv::Point(int(landmarks[i]), int(landmarks[i + 1])), 2, cv::Scalar(0, 0, 255), -1);
        }

        // Textual output
        //clandmark::printTimingStats(flandmark->timings);
        //clandmark::printLandmarks(landmarks, flandmark->getLandmarksCount());
        //clandmark::printLandmarks(flandmark->getLandmarksNF(), flandmark->getLandmarksCount());

        float cur = faces[i].width * faces[i].height;
        if (cur > largest)
        {
            largest = cur;

            eyes_x = float(faces[i].x + faces[i].width / 2) / frame.cols - 0.5f;
            eyes_y = float(top) / frame.rows - 0.5f;
            head_size_y = float(faces[i].height) / frame.rows;
            head_size_x = float(faces[i].width) / frame.cols;

            cv::circle(frame, cv::Point(int(faces[i].x + faces[i].width / 2), int(top)), 2, cv::Scalar(0, 255, 255), -1);
        }
    }

    delete frm_gray;
}

cimg_library::CImg<unsigned char>* FaceRecognizer::cvImgToCImg(cv::Mat& cvImg)
{
    cimg_library::CImg<unsigned char>* result = new cimg_library::CImg<unsigned char>(cvImg.cols, cvImg.rows);

    for (int x = 0; x < cvImg.cols; ++x)
        for (int y = 0; y < cvImg.rows; ++y)
            (*result)(x, y) = cvImg.at<uchar>(y, x);

    return result;
}