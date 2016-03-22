#include <vector>
#include <string>
#include <sstream>
#include <fstream>

#include <ros/ros.h>
#include <tf/tf.h>
#include <move_base_msgs/MoveBaseAction.h>
#include <geometry_msgs/PoseStamped.h>
#include <geometry_msgs/Point.h>
#include <geometry_msgs/Quaternion.h>
#include <actionlib/client/simple_action_client.h>

typedef actionlib::SimpleActionClient<move_base_msgs::MoveBaseAction> MoveBaseClient;

int main(int argc, char** argv)
{
	ros::init(argc, argv, "goal_queue");
	
	if (argc < 2 || argc > 2)
	{
		printf("Usage: send_markers file_name\n");
		return -1;
	}
	
	std::string file_name = std::string(argv[1]);

	ROS_INFO("Goal Queue Started.");
	
	int goal_itr = 0;
	bool ac_online = false;

	ros::NodeHandle n;
	
	MoveBaseClient ac("/move_base", true);

	if(ac.waitForServer(ros::Duration(10.0)))
	{
		ROS_INFO("Contacted move_base action server");
		ac_online = true;
	}

	else
		ROS_INFO("Unable to contact move_base action server.");

	ros::Rate r(1);
	
	std::vector<std::string> targets;
	
	std::string line;
	std::ifstream myfile (file_name.c_str());
	if (myfile.is_open())
	{
		while ( getline (myfile,line) )
		{
			//cout << line << '\n';
			targets.push_back(line);
		}
		myfile.close();
	}
	else {
		std::cout << "Unable to open file" << std::endl;; 
		return -1;
	}
	
	/*double x, y, z, qx, qy, qz, qw;
	
	for (unsigned int i=0; i < targets.size(); i++) {
		std::string tmp = targets.at(i);
		
		std::stringstream ss(tmp);
		
		std::string str_x, str_y, str_z, str_qx, str_qy, str_qz, str_qw;
		ss >> str_x >> str_y >> str_z >> str_qx >> str_qy >> str_qz >> str_qw;
		
		x = atof(str_x.c_str());
		y = atof(str_y.c_str());
		z = atof(str_z.c_str());
		qx = atof(str_qx.c_str());
		qy = atof(str_qy.c_str());
		qz = atof(str_qz.c_str());
		qw = atof(str_qw.c_str());
		
		std::cout << x << " " << y << " " << z << " " << qx << " " << qy << " " << qz << " " << qw << std::endl;
	}*/


	while(ros::ok() && ac_online && goal_itr < targets.size())
	{
		double x, y, z, qx, qy, qz, qw;
		
		std::string tmp = targets.at(goal_itr);
		
		std::stringstream ss(tmp);
		
		std::string str_x, str_y, str_z, str_qx, str_qy, str_qz, str_qw;
		ss >> str_x >> str_y >> str_z >> str_qx >> str_qy >> str_qz >> str_qw;
		
		x = atof(str_x.c_str());
		y = atof(str_y.c_str());
		z = atof(str_z.c_str());
		qx = atof(str_qx.c_str());
		qy = atof(str_qy.c_str());
		qz = atof(str_qz.c_str());
		qw = atof(str_qw.c_str());
		
		move_base_msgs::MoveBaseGoal goal;
		
		//geometry_msgs::Pose pose(geometry_msgs::Point(x, y, z), geometry_msgs::Quaternion(qx, qy, qz, qw));
		//geometry_msgs::Pose pose;
		
		/*geometry_msgs::Vector3 point;// = tf::Vector3(x, y, z);
		geometry_msgs::Quaternion q;// = tf::Quaternion(qx, qy, qz, qw);
		
		pose.position = point;
		pose.orientation = q;*/
		
		geometry_msgs::PoseStamped pose;
		pose.header.stamp = ros::Time::now();
		pose.header.frame_id = "/map";
		pose.pose.position.x = x;
		pose.pose.position.y = y;
		pose.pose.position.z = z;
		pose.pose.orientation.x = qx;
		pose.pose.orientation.y = qy;
		pose.pose.orientation.z = qz;
		pose.pose.orientation.w = qw;

		goal.target_pose = pose;
		goal.target_pose.header.stamp = ros::Time::now();

		ROS_INFO("Sending Goal nr: %d / %lu", goal_itr + 1, targets.size());
		std::cout << "Goal data: " << std::endl;
		std::cout << x << " " << y << " " << z << " " << qx << " " << qy << " " << qz << " " << qw << std::endl;
		ac.sendGoal(goal);
		ac.waitForResult();

		if(ac.getState() == actionlib::SimpleClientGoalState::SUCCEEDED)
			ROS_INFO("Goal reached succesfully.");
		else
			ROS_INFO("Reaching the goal has failed.");
		
		ros::Duration(5).sleep();
		
		if(goal_itr++ == targets.size()){
			goal_itr = 0;
			break;
		}

		ROS_INFO("Moving to goal nr: %d / %lu", goal_itr + 1, targets.size());

		ros::spinOnce();
		r.sleep();
	}

	ROS_INFO("Stop sending targets...");

	return 0;
}
