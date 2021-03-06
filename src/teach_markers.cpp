#include <cstdio>
#include <tf/transform_listener.h>
#include <ros/ros.h>
#include <iostream>
#include <fstream>

#define _USE_MATH_DEFINES
class echoListener
{
	public:

	tf::TransformListener tf;
	
	//constructor with name
	echoListener()
	{

	};

	~echoListener()
	{

	};

	private:
};


int main(int argc, char ** argv)
{
	//Initialize ROS
	ros::init(argc, argv, "tf_echo", ros::init_options::AnonymousName);

	// Allow 1
	if (argc < 3 || argc > 3)
	{
		printf("Usage: teach_markers base_name file_name\n");
		return -1;
	}

	ros::NodeHandle nh;

	//Instantiate a local listener
	echoListener echoListener;

	std::string base_name = std::string(argv[1]);
	std::string file_name = std::string(argv[2]);
	
	std::string output;
	
	unsigned int iter = 0;
	
	double x, y, r;

	// Wait for up to one second for the first transforms to become avaiable. 
	echoListener.tf.waitForTransform("/map", "/base_footprint", ros::Time(), ros::Duration(1.0));

	//Nothing needs to be done except wait for a quit
	//The callbacks withing the listener class
	//will take care of everything
	while(nh.ok())
	{	
		std::stringstream s;
		s << iter;
		std::string converted(s.str());
		std::string target = base_name + converted;	
		
		try
		{
			tf::StampedTransform echo_transform;
			echoListener.tf.lookupTransform("/map", target, ros::Time(), echo_transform);
			std::cout.precision(3);
			std::cout.setf(std::ios::fixed,std::ios::floatfield);
			
			double yaw, pitch, roll;
			echo_transform.getBasis().getRPY(roll, pitch, yaw);
			tf::Quaternion q = echo_transform.getRotation();
			tf::Vector3 v = echo_transform.getOrigin();
			
			std::cout << "From: " << "/map to: " << target << std::endl;
			std::cout << "Raw data:" << std::endl;
			std::cout << "- Translation: [" << v.getX() << ", " << v.getY() << ", " << v.getZ() << "]" << std::endl;
			std::cout << "- Rotation: in Quaternion [" << q.getX() << ", " << q.getY() << ", " 
			<< q.getZ() << ", " << q.getW() << "]" << std::endl
			<< "            in RPY (radian) [" <<  roll << ", " << pitch << ", " << yaw << "]" << std::endl	<< "            in RPY (degree) [" <<  roll*180.0/M_PI << ", " << pitch*180.0/M_PI << ", " << yaw*180.0/M_PI << "]" << std::endl << std::endl;
			
			std::cout << "Normalized data:" << std::endl;
			tf::Quaternion q_norm;
			q_norm.setRPY(0.0, 0.0, yaw);
			
			std::cout << "- Translation: [" << v.getX() << ", " << v.getY() << ", " << 0.0 << "]" << std::endl;
			std::cout << "- Rotation: in Quaternion [" << q_norm.getX() << ", " << q_norm.getY() << ", " << q_norm.getZ() << ", " << q_norm.getW() << "]" << std::endl;
			
			x = v.getX();
			y = v.getY();
			r = yaw;
		}
		catch(tf::TransformException& ex)
		{
			std::cout << "Failure at "<< ros::Time::now() << std::endl;
			std::cout << "Exception thrown:" << ex.what()<< std::endl;
		}
		
		std::cout << "NESW for orientation to store, Q to quit anything else to skip" << std::endl;
		
		char c;
		std::cin.get(c);
		
		if(c == 'n'){
			std::cout << "NORTH" << std::endl;	
			tf::Quaternion q;
			q.setRPY(0.0, 0.0, r);
			
			std::stringstream s;
			s << " " << x << " " << y << " " << 0 << " " << q.getX() << " " << q.getY() << " " << q.getZ() << " " << q.getW() << std::endl;
			std::string converted(s.str());
			std::string target = base_name + converted;	
			output += converted;
			iter++;
		}
		else if(c == 'e'){
			std::cout << "EAST" << std::endl;
			tf::Quaternion q;
			q.setRPY(0.0, 0.0, r - (M_PI/2.0));
			
			std::stringstream s;
			s << " " << x << " " << y << " " << 0 << " " << q.getX() << " " << q.getY() << " " << q.getZ() << " " << q.getW() << std::endl;
			std::string converted(s.str());
			std::string target = base_name + converted;	
			output += converted;
			iter++;
		}
		else if(c == 's'){
			std::cout << "SOUTH" << std::endl;
			tf::Quaternion q;
			q.setRPY(0.0, 0.0, r + M_PI);
			
			std::stringstream s;
			s << " " << x << " " << y << " " << 0 << " " << q.getX() << " " << q.getY() << " " << q.getZ() << " " << q.getW() << std::endl;
			std::string converted(s.str());
			std::string target = base_name + converted;	
			output += converted;
			iter++;
		}
		else if(c == 'w'){
			std::cout << "WEST" << std::endl;
			tf::Quaternion q;
			q.setRPY(0.0, 0.0, r + (M_PI/2.0));
			
			std::stringstream s;
			s << " " << x << " " << y << " " << 0 << " " << q.getX() << " " << q.getY() << " " << q.getZ() << " " << q.getW() << std::endl;
			std::string converted(s.str());
			std::string target = base_name + converted;	
			output += converted;
			iter++;
		}
		else if(c == 'q'){
			std::cout << "QUIT" << std::endl;
			std::ofstream myfile (file_name.c_str());
			if (myfile.is_open())
			{
				myfile << output;
				myfile.close();
			}
			else std::cout << "Unable to open file";
			break;
		}
		else{
			std::cout << "SKIP" << std::endl;
		}
	}

	return 0;
};
