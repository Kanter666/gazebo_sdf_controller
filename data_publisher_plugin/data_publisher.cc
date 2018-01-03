#include <boost/bind.hpp>
#include <boost/lexical_cast.hpp>
#include <gazebo/gazebo.hh>
#include <gazebo/physics/physics.hh>
#include <gazebo/common/common.hh>
#include <stdio.h>
#include <gazebo/msgs/msgs.hh>
#include <gazebo/transport/transport.hh>
#include <string>

namespace gazebo
{
  class DataPublisher : public ModelPlugin
  {
    public: void Load(physics::ModelPtr _parent, sdf::ElementPtr /*_sdf*/)
    {
      // Store the pointer to the model
      this->model = _parent;
      this->joints = this->model->GetJoints(); 

      // Listen to the update event. This event is broadcast every
      // simulation iteration.
      this->updateConnection = event::Events::ConnectWorldUpdateBegin(
          boost::bind(&DataPublisher::OnUpdate, this, _1));

      std::string topicStart = "~/" + this->model->GetName() + "/"; 

      // Create our node for communication
      transport::NodePtr node(new transport::Node());
        node->Init();

      // setting up publishers
      this->pubJointsPos =
        node->Advertise<msgs::GzString_V>(topicStart+"joints_pos");
      this->pubJointsVel =
        node->Advertise<msgs::GzString_V>(topicStart+"joints_vel");
      this->pubModelPos =
        node->Advertise<msgs::GzString_V>(topicStart+"model_pos");
    }

    // Called by the world update start event
    public: void OnUpdate(const common::UpdateInfo & /*_info*/)
    {
      // Create a messages
      msgs::GzString_V joints_pos;
      msgs::GzString_V joints_vel;

      // Get values for every joint
      for (int i = 0; i < joints.size(); i++){
      	joints_pos.add_data(boost::lexical_cast<std::string>(this->joints[i]->GetAngle(1).Degree()));
      	joints_vel.add_data(boost::lexical_cast<std::string>(this->joints[i]->GetVelocity(0)));
      } 

      // Send the message
      pubJointsPos->Publish(joints_pos);
      pubJointsVel->Publish(joints_vel);

    }
    // Pointer to the update event connection
    private: event::ConnectionPtr updateConnection;

    // Pointer to the model
    private: physics::ModelPtr model;

    private: std::vector<physics::JointPtr> joints; 

    private: transport::PublisherPtr pubJointsPos;
    private: transport::PublisherPtr pubJointsVel;
    private: transport::PublisherPtr pubModelPos;
  };

  // Register this plugin with the simulator
  GZ_REGISTER_MODEL_PLUGIN(DataPublisher)
}