//
// Created by ctb on 1/20/19.
//

#include <iostream>
#include <sstream>
#include <string>
#include <math.h>
#include "std_msgs/String.h"
#include "ros/ros.h"
#include <visualization_msgs/Marker.h>
#include "nav_msgs/Odometry.h"
#include "nav_msgs/Path.h"
#include <cmath>
using namespace std;


ros::Publisher pub_distance;
ros::Publisher marker_pub;
visualization_msgs::Marker points, line_strip, line_list;



geometry_msgs::Point p;
geometry_msgs::Point AP1;
void pathTrackerCallback(const nav_msgs::Path &odom_msg)
{
    //ROS_INFO_STREAM("path"<<odom_msg.poses.size());
    int len=odom_msg.poses.size();
    float position_x=odom_msg.poses.back().pose.position.x;
    float position_y=odom_msg.poses.back().pose.position.y;
    float position_z=odom_msg.poses.back().pose.position.z;
    ROS_INFO_STREAM("position"<<position_x<<position_y<<position_z);

    float orientation_x=odom_msg.poses.back().pose.orientation.x;
    float orientation_y=odom_msg.poses.back().pose.orientation.y;
    float orientation_z=odom_msg.poses.back().pose.orientation.z;
    float orientation_w=odom_msg.poses.back().pose.orientation.w;

    //calculate the distance between the AP and the camera
    float position_ap_x=4.239254485518047;
    float position_ap_y=1.217918714770732;
    float position_ap_z=1.2099123218848773;
    float distance=pow(pow(position_x-position_ap_x,2) +pow(position_y-position_ap_y,2)+pow(position_z-position_ap_z ,2),0.5);

    p.x = position_x;
    p.y = position_y;
    p.z = position_z;

    points.points.push_back(p);
    line_strip.points.push_back(p);

    // The line list needs two points for each line
    line_list.points.push_back(p);
    AP1.x = position_ap_x;
    AP1.y = position_ap_y;
    AP1.z = position_ap_z;
    line_list.points.push_back(AP1);

    string result;
    ostringstream convert;
    convert <<distance;
    result=convert.str();
    ROS_INFO_STREAM("the distance between the AP and the camera is "<<result);

    //convert string to ROS data and publish
    std_msgs::String msg;
    msg.data=result;
    pub_distance.publish(msg);
    marker_pub.publish(points);
    marker_pub.publish(line_strip);
    marker_pub.publish(line_list);
    line_list.points.clear();
}

int main(int argc, char **argv)
{
    ros::init(argc,argv,"path_tracker");
    ros::NodeHandle n("~");
    ros::Rate r(30);
    ROS_INFO("tracking the path and creating the PCL");
    // Set our initial shape type to be a cube
    uint32_t shape = visualization_msgs::Marker::CUBE;
    pub_distance=n.advertise<std_msgs::String>("distance", 1000);

    ros::Subscriber sub_path=n.subscribe("/benchmark_publisher/path", 1000, pathTrackerCallback);
//    ros::Rate r(20);
    marker_pub = n.advertise<visualization_msgs::Marker>("visualization_marker", 1);


    float f = 0.0;

    points.header.frame_id = line_strip.header.frame_id = line_list.header.frame_id = "/my_frame";
    points.header.stamp = line_strip.header.stamp = line_list.header.stamp = ros::Time::now();
    points.ns = line_strip.ns = line_list.ns = "points_and_lines";
    points.action = line_strip.action = line_list.action = visualization_msgs::Marker::ADD;
    points.pose.orientation.w = line_strip.pose.orientation.w = line_list.pose.orientation.w = 1.0;

    points.id = 0;
    line_strip.id = 1;
    line_list.id = 2;



    points.type = visualization_msgs::Marker::POINTS;
    line_strip.type = visualization_msgs::Marker::LINE_STRIP;
    line_list.type = visualization_msgs::Marker::LINE_LIST;



    // POINTS markers use x and y scale for width/height respectively
    points.scale.x = 0.1;
    points.scale.y = 0.1;

    // LINE_STRIP/LINE_LIST markers use only the x component of scale, for the line width
    line_strip.scale.x = 0.1;
    line_list.scale.x = 0.1;



    // Points are green
    points.color.g = 1.0f;
    points.color.a = 1.0;

    // Line strip is blue
    line_strip.color.b = 1.0;
    line_strip.color.a = 1.0;

    // Line list is red
    line_list.color.r = 1.0;
    line_list.color.a = 1.0;
    points.points.push_back(AP1);
//    r.sleep();

    ros::spin();
}

