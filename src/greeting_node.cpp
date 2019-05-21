/******************************************************************************
*
* The MIT License (MIT)
*
* Copyright (c) 2018 Bluewhale Robot
*
* Permission is hereby granted, free of charge, to any person obtaining a copy
* of this software and associated documentation files (the "Software"), to deal
* in the Software without restriction, including without limitation the rights
* to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
* copies of the Software, and to permit persons to whom the Software is
* furnished to do so, subject to the following conditions:
*
* The above copyright notice and this permission notice shall be included in all
* copies or substantial portions of the Software.
*
* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
* IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
* FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
* AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
* LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
* OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
* SOFTWARE.
*
* Author: Randoms
*******************************************************************************/

#include "greeting_node.h"

sensor_msgs::ImageConstPtr last_frame = NULL;
float linear_x = 0;
float angle_z = 0;
cv::Mat previous_frame;
ros::Publisher image_pub;
ros::Publisher res_pub;
ros::Publisher audio_pub;
int threshold = 1000000;
int64_t audio_count = 0;
std::string greeting_words;
bool is_enabled = true;
bool enable_debug = false;



sensor_msgs::ImageConstPtr get_one_frame()
{
    return last_frame;
}

// 检测是否需要播放欢迎声音
bool check_greeting(cv::Mat image){
    audio_count -= 1000 / 30; // image 30hz
    if(audio_count <= 0)
        audio_count = 0;
    cv::Mat gray_image;
    cv::cvtColor(image, gray_image, CV_BGR2GRAY);
    cv::Mat smooth_image;
    GaussianBlur(gray_image, smooth_image, cv::Size(9, 9), 0);
    if(previous_frame.empty() || std::abs(linear_x) > 0.05 || std::abs(angle_z) > 0.1){
        previous_frame = smooth_image;
        return false;
    }
    cv::Mat diffFrame;
    cv::absdiff(smooth_image, previous_frame, diffFrame);
    previous_frame = smooth_image;
    cv::cvtColor(diffFrame, image, CV_GRAY2BGR);
    std_msgs::Int64 result;
    result.data = cv::sum(diffFrame)[0];
    if(enable_debug)
        res_pub.publish(result);
    if(result.data > threshold)
        return true;
    return false;
}

void update_frame(const sensor_msgs::ImageConstPtr &new_frame)
{
    if(!is_enabled)
        return;
    last_frame = new_frame;
    cv_bridge::CvImagePtr cv_ptr = cv_bridge::toCvCopy(new_frame, "bgr8");
    cv::Mat cv_image = cv_ptr->image;
    if(check_greeting(cv_image)){
        if(audio_count == 0){
            std_msgs::String greeting;
            greeting.data = greeting_words;
            audio_pub.publish(greeting);
            audio_count = 2 * 1000;
        }
    }
    if(enable_debug)
        image_pub.publish(cv_ptr->toImageMsg());
}

void update_odom(const nav_msgs::OdometryConstPtr &odom){
    linear_x = odom->twist.twist.linear.x;
    angle_z = odom->twist.twist.angular.z;
}

int main(int argc, char **argv)
{
    ros::init(argc, argv, "xiaoqiang_greeting_node");
    ros::AsyncSpinner spinner(4);
    spinner.start();
    ros::NodeHandle private_nh("~");
    image_pub = private_nh.advertise<sensor_msgs::Image>("processed_image", 10);
    res_pub = private_nh.advertise<std_msgs::Int64>("results", 10);
    audio_pub = private_nh.advertise<std_msgs::String>("text", 10);
    ros::Subscriber image_sub = private_nh.subscribe("image", 10, update_frame);
    ros::Subscriber twist_sub = private_nh.subscribe("odom", 10, update_odom);
    private_nh.param("threshold", threshold, 1000000);
    private_nh.param("enable_debug", enable_debug, false);
    ros::param::param<std::string>("~greeting_words", greeting_words, "欢迎光临");
    while (ros::ok())
    {
        private_nh.param("is_enabled", is_enabled, true);
        sleep(1);
    }
}