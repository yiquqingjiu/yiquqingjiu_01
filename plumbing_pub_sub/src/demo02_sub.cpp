#include "ros/ros.h"
#include "plumbing_pub_sub/ConeArray.h"  

void doMsg(const plumbing_pub_sub::ConeArray::ConstPtr& msg)
{
    ROS_INFO("===== 收到锥桶数据 =====");
    ROS_INFO("frame_id: %s", msg->header.frame_id.c_str());
    ROS_INFO("锥桶数量: %ld", msg->cones.size()); 

    for (size_t i = 0; i < msg->cones.size(); ++i)
    {
        ROS_INFO("锥桶 %ld: [x: %.2f, y: %.2f, z: %.2f],color:[%s]", 
                 i+1, 
                 msg->cones[i].position.x, 
                 msg->cones[i].position.y,
                 msg->cones[i].position.z,
                 msg->cones[i].color.c_str()
                );
    }
}


int main(int argc, char *argv[])
{
    setlocale(LC_ALL,"");
    
    ros::init(argc, argv, "cone_subscriber_node");
    
    ros::NodeHandle nh;
    
    ros::Subscriber sub = nh.subscribe<plumbing_pub_sub::ConeArray>(
        "/test/camera_cones", 
        10,                    
        doMsg                  
    );

    ROS_INFO("成功订阅 /test/camera_cones 话题，等待数据...");
    
    ros::spin();
    return 0;
}