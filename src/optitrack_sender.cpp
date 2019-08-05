#include <ros/ros.h>

#include <iostream>
#include <Eigen/Eigen>
#include <px4_command/MocapInfo.h>
#include <OptiTrackFeedBackRigidBody.h>

px4_command::MocapInfo UAV_motion;
px4_command::MocapInfo Payload_motion;

rigidbody_state UAVstate;
rigidbody_state Payloadstate;


int main(int argc, char **argv)
{
    ros::init(argc, argv, "optitrack_sender");
    ros::NodeHandle nh("~");

    // publish 
    ros::Publisher UAV_motion_pub = nh.advertise<px4_command::MocapInfo>("/px4_command/cf1", 1000);
    ros::Publisher Payload_motion_pub = nh.advertise<px4_command::MocapInfo>("/px4_command/Payload", 1000);
    // 频率
    ros::Rate rate(50.0);

    OptiTrackFeedBackRigidBody UAV("/vrpn_client_node/cf1/pose",nh,3,3);
    OptiTrackFeedBackRigidBody Payload("/vrpn_client_node/Payload/pose",nh,3,3);
    int notfeedbackcounter = 3;
//>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>Main Loop<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
    while(ros::ok())
    {
        //回调一次 更新传感器状态
        ros::spinOnce();

        //利用OptiTrackFeedBackRigidBody类获取optitrack的数据
        UAV.FeedbackDetector(1);
        //Payload.FeedbackDetector(3);
        UAV.GetState(UAVstate);
        Payload.GetState(Payloadstate);
        UAV_motion.header.stamp = ros::Time::now();

        for(int i = 0;i<3;i++)
        {
            UAV_motion.position[i] = UAVstate.Position(i);
            UAV_motion.velocity[i] = UAVstate.V_I(i);
            UAV_motion.angular_velocity[i] =  UAVstate.Omega_BI(i);
            UAV_motion.Euler[i] = UAVstate.Euler(i);
            Payload_motion.position[i] = Payloadstate.Position(i);
            Payload_motion.velocity[i] = Payloadstate.V_I(i);
            Payload_motion.angular_velocity[i] =  Payloadstate.Omega_BI(i);
            Payload_motion.Euler[i] = Payloadstate.Euler(i); 
        }
        for(int i = 0;i<4;i++)
        {
            UAV_motion.quaternion[i] = UAVstate.quaternion(i);
            Payload_motion.quaternion[i] = Payloadstate.quaternion(i);
        }
        UAV_motion_pub.publish(UAV_motion);
        Payload_motion_pub.publish(Payload_motion);
        rate.sleep();
    }
    return 0;
}
