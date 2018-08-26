#include <algorithm>

#include "FaceRecognizer.h"
#include "Renderer.h"

const std::string flandmark_path = "data/flandmark_model.xml";
const std::string haarcascade_path = "data/haarcascade_frontalface_alt.xml";

int cam_id = 0;

constexpr int window_width = 1600;
constexpr int window_height = 1600;

int main(int argc, const char** argv)
{
    try
    {
        FaceRecognizer face_rec;
        Renderer renderer(window_width, window_height, "Scene");

        face_rec.init(flandmark_path, haarcascade_path);

        cv::VideoCapture capture;
        capture.open(cam_id);
        if (!capture.isOpened()) throw std::runtime_error("Couldn't open video input.");

        cv::Mat frame;
        capture >> frame;
        if (frame.empty()) throw std::runtime_error("Unable to read a frame.");

        const std::string window_id = "win1";
        cv::namedWindow(window_id, CV_WINDOW_KEEPRATIO);
        cv::setWindowTitle(window_id, "Camera input");

        double tic;

        float angle_x = 0, angle_y = 0;
        float old1_angle_x = 0, old1_angle_y = 0;
        float old2_angle_x = 0, old2_angle_y = 0;
        float old1_dist = 0, old2_dist = 0;

        while (!glfwWindowShouldClose(renderer.getWindow()))
        {
            tic = (double)cv::getTickCount();

            capture >> frame;

            if (!frame.empty())
            {
                face_rec.update(frame);

                tic = ((double)cv::getTickCount() - tic) / cv::getTickFrequency() * 1000;

                std::stringstream fps;
                fps << "fps: " << std::setprecision(4) << std::setw(4) << 1000.0 / tic << " ";
                
                std::cout << fps.str() << std::endl;
                //cv::putText(frame, fps.str(), cv::Point(10, 25), CV_FONT_HERSHEY_COMPLEX, 1, cv::Scalar(255, 255, 0));
                //				putText(frame, fps.str(), Point(10, 25), FONT_HERSHEY_COMPLEX, 1, Scalar(255, 255, 0));

                cv::imshow(window_id, frame);
            }
            else {
                std::cerr << "No frame --- skip." << std::endl;
            }

            //std::cout << face_rec.eyes_x << ", " << face_rec.eyes_y << ": " << face_rec.head_size << std::endl;

            float camera_fov_y = 45.f;
            float head_height = 0.2f, head_width = 0.2f;

            float dist = (head_height / (2.f * tan(glm::radians(camera_fov_y / 2.f)))) / face_rec.head_size_y; // 0.24f
            
            angle_x = 2.5f * atan((head_width * face_rec.eyes_x) / (dist * face_rec.head_size_x));
            angle_y = 2.5f * atan((head_height * face_rec.eyes_y)/ (dist * face_rec.head_size_y));

            angle_x = (1 * old2_angle_x + 2 * old1_angle_x + 1 * angle_x) / 4;
            angle_y = (1 * old2_angle_y + 2 * old1_angle_y + 1 * angle_y) / 4;
            dist = (1 * old2_dist + 2 * old1_dist + 1 * dist) / 4;

            std::cout << angle_x << " " << angle_y << " " << dist << std::endl;

            renderer.update(angle_x, angle_y, 2.2f * dist);

            old2_angle_x = old1_angle_x;
            old2_angle_y = old1_angle_y;
            old2_dist = old1_dist;

            old1_angle_x = angle_x;
            old1_angle_y = angle_y;
            old1_dist = dist;

            glfwPollEvents();

            if (glfwGetKey(renderer.getWindow(), GLFW_KEY_ESCAPE) == GLFW_PRESS) break;
        }
    }
    catch (const std::exception& e)
    {
        std::cerr << e.what() << " Exiting..." << std::endl;
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
