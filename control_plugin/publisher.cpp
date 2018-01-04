#include <string>
#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <gazebo/gazebo_config.h>
#include <gazebo/transport/transport.hh>
#include <gazebo/msgs/msgs.hh>

#include <gazebo/gazebo_client.hh>

using namespace std;

vector<string> jointVel = {};

int publishVector(string topic,  vector<string> values)
{
  // Load gazebo as a client
  gazebo::client::setup();

  // Create our node for communication
  gazebo::transport::NodePtr node(new gazebo::transport::Node());
  node->Init();

  // Publish to the  velodyne topic
  gazebo::transport::PublisherPtr pub =
    node->Advertise<gazebo::msgs::GzString_V>(topic);//"~/my_velodyne/vel_cmd"

  // Wait for a subscriber to connect to this publisher
  pub->WaitForConnection();

  // Create a a vector3 message
  gazebo::msgs::GzString_V msg;

  for (int i = 0; i < values.size(); i++)
    msg.add_data(values[i]);

  // Send the message
  pub->Publish(msg);

  // Make sure to shut everything down.
  gazebo::client::shutdown();
}

void cb(ConstGzString_VPtr &_msg)
{
  for (int i = 0; i < _msg->data_size(); i++) 
    jointVel.push_back(_msg->data(i));
  for (vector<string>::const_iterator i = jointVel.begin(); i != jointVel.end(); ++i)
    std::cerr << *i << ' ';
  std::cerr << " were received values \n";

  // Make sure to shut everything down.
  gazebo::client::shutdown();
}

vector<string> getJointsVel(string robot)
{
  jointVel = {};
  // Load gazebo as a client
  gazebo::client::setup();

  // Create our node for communication
  gazebo::transport::NodePtr node(new gazebo::transport::Node());
  node->Init();
  
  // Subscriber
  gazebo::transport::SubscriberPtr subJointVel = node->Subscribe("~/" + robot + "/joints_vel", cb);

  // wait till message is received
  while(jointVel.empty()){
  	gazebo::common::Time::MSleep(10);
  }
  // wait a little bit longer for message to load
  gazebo::common::Time::MSleep(10);

  return jointVel;
}

PYBIND11_MODULE(publisher, m) {
    m.doc() = "Gazebo publisher"; // optional module docstring

    m.def("publishVector", &publishVector, "A function which publishes vector data to topic");
    m.def("getJointsVel", &getJointsVel, "A function which returns velocities of joints");
}