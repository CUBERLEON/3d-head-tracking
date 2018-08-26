#pragma once

#include <opencv2/core/core.hpp>
#include <opencv2/objdetect/objdetect.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <Flandmark.h>

#include <iostream>
#include <iomanip>
#include <string>
#include <cstdio>

class FaceRecognizer
{
public:
    FaceRecognizer();
    ~FaceRecognizer();

    void init(const std::string& flandmark_path, const std::string& haarcascade_path);
    void FaceRecognizer::update(cv::Mat& frame);

    float eyes_x = 0;
    float eyes_y = 0;
    float head_size_y = 1;
    float head_size_x = 1;
private:
    cv::CascadeClassifier face_cascade;
    clandmark::Flandmark* flandmark;

    cimg_library::CImg<unsigned char>* cvImgToCImg(cv::Mat& cvImg);
};