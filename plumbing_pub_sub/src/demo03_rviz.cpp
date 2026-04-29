#include "ros/ros.h"
#include "plumbing_pub_sub/ConeArray.h"  
#include <cmath>
#include <visualization_msgs/Marker.h>
#include <visualization_msgs/MarkerArray.h>

// 相机→车体 平移向量 [后0.3, 左0.1, 上0.95]
const double Tx = - 0.3;
const double Ty = 0.1;
const double Tz = 0.95;
// 绕车体Z轴左偏30°
const double YAW = 30.0 * M_PI / 180.0;
const double cosY = cos(YAW);
const double sinY = sin(YAW);
// 像素转米比例
const double PIXEL_TO_M = 1.0 / 100.0;
// 锥桶Marker尺寸
const double CONE_RASE_SIZE = 0.2;
const double CONE_HEIGHT = 0.3;

// 全局Marker发布器
ros::Publisher marker_pub;

void doMsg(const plumbing_pub_sub::ConeArray::ConstPtr& msg)
{
    ROS_INFO("===== 锥桶坐标转换 + RViz可视化 =====");
    ROS_INFO("锥桶总数：%ld", msg->cones.size());

    visualization_msgs::MarkerArray cone_marker_array;
    int marker_id = 0;  

    // 每个锥桶生成独立 MARKER
    for(size_t i = 0; i < msg->cones.size(); ++i)
    {
        visualization_msgs::Marker cone_marker;
        cone_marker.header.frame_id = "base_link";
        cone_marker.header.stamp = ros::Time::now();
        cone_marker.ns = "cone_list";    
        cone_marker.id = marker_id++; 
        
        // 关键：改用 CUBE，
        cone_marker.type = visualization_msgs::Marker::CYLINDER;
        cone_marker.action = visualization_msgs::Marker::ADD;
        cone_marker.scale.x = CONE_RASE_SIZE;
        cone_marker.scale.y = CONE_RASE_SIZE;
        cone_marker.scale.z = CONE_HEIGHT;
        cone_marker.pose.orientation.w = 1.0; 

        // 坐标转换（完全保留你的）
        double pixel_x = msg->cones[i].position.x;
        double pixel_y = msg->cones[i].position.y;
        double pixel_z = msg->cones[i].position.z;
        double cam_x = pixel_x * PIXEL_TO_M;
        double cam_y = pixel_y * PIXEL_TO_M;
        double cam_z = pixel_z * PIXEL_TO_M;

        double car_x = - cam_z * cosY - cam_x * sinY + Tx;
        double car_y = - cam_z * sinY + cam_x * cosY - Ty;
        double car_z = - cam_y + Tz;

        ROS_INFO("锥桶%2ld | 颜色：%s | 车体[X:%.2f,Y:%.2f,Z:%.2f]",
                 i+1, msg->cones[i].color.c_str(), car_x, car_y, car_z);

        // 每个锥桶单独设置位置
        cone_marker.pose.position.x = car_x;
        cone_marker.pose.position.y = car_y;
        cone_marker.pose.position.z = car_z;

        // 颜色判断
        if(msg->cones[i].color == "red_cone")
        {
            cone_marker.color.r = 1;
            cone_marker.color.g = 0;
            cone_marker.color.b = 0;
            cone_marker.color.a = 1;
        }
        else if(msg->cones[i].color == "blue_cone")
        {
            cone_marker.color.r = 0;
            cone_marker.color.g = 0;
            cone_marker.color.b = 1;
            cone_marker.color.a = 1;
        }
        else
        {
            cone_marker.color.r = 0.5;
            cone_marker.color.g = 0.5;
            cone_marker.color.b = 0.5;
            cone_marker.color.a = 1;
        }

        cone_marker_array.markers.push_back(cone_marker);
    }

    marker_pub.publish(cone_marker_array);
}

int main(int argc, char *argv[])
{
    setlocale(LC_ALL,"");
    ros::init(argc, argv, "cone_visual_node");
    ros::NodeHandle nh;

    ros::Subscriber sub = nh.subscribe<plumbing_pub_sub::ConeArray>("/test/camera_cones", 10, doMsg);
    
    marker_pub = nh.advertise<visualization_msgs::MarkerArray>("/rviz_cone_marker", 10);

    ROS_INFO("锥桶可视化节点启动！等待话题数据...");
    ros::spin();
    return 0;
}