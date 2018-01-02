#ifndef _LAUNCH_PLUGIN_HH_
#define _LAUNCH_PLUGIN_HH_

#include <string> 

#include <gazebo/gazebo.hh>
#include <gazebo/physics/physics.hh>
#include <gazebo/transport/transport.hh>
#include <gazebo/msgs/msgs.hh>

namespace gazebo
{
  /// \brief A plugin to control a Velodyne sensor.
  class LaunchPlugin : public ModelPlugin
  {
    /// \brief Constructor
    public: LaunchPlugin() {}

    /// \brief A node used for transport 
    private: transport::NodePtr node; 
 
    /// \brief A subscriber to a named topic. 
    private: transport::SubscriberPtr sub; 
 
    /// \brief Pointer to the model. 
    private: physics::ModelPtr model; 
 
    /// \brief Pointer to the joint. 
    private: std::vector<physics::JointPtr> joints; 
 
    /// \brief A PID controller for the joint. 
    private: common::PID pid; 

    public: virtual void Load(physics::ModelPtr _model, sdf::ElementPtr _sdf)
    {
      // Store the model pointer for convenience.
      this->model = _model;

      // Get the first joint. We are making an assumption about the model
      // having one joint that is the rotational joint.
      this->joints = _model->GetJoints(); 
      std::cerr << "This model has " << joints.size()<< " joints \n"; 

      // Setup a P-controller, with a gain of 0.1.
      this->pid = common::PID(0.1, 0, 0);

      // Apply the P-controller to the joint.
      for (int i = 0; i < joints.size(); i++) 
          this->joints[i]->GetScopedName(), this->pid;

      // Create the node
      this->node = transport::NodePtr(new transport::Node());
      this->node->Init(this->model->GetWorld()->GetName());

      std::cout << "Name: " << this->model->GetName();
      // Create a topic name
      std::string topicName = "~/" + this->model->GetName() + "/control"; 

      // Subscribe to the topic, and register a callback
       this->sub = this->node->Subscribe(topicName, 
         &LaunchPlugin::OnMsg, this); 
    }

    /// \brief Set the velocity of the Velodyne
    /// \param[in] _vel New target velocity
    public: void SetVelocity(const double &_vel, int &_index)
    {
      // Set the joint's target velocity.
      this->model->GetJointController()->SetVelocityTarget(
          this->joints[_index]->GetScopedName(), _vel);
    }

    /// \brief Handle incoming message
    /// \param[in] _msg Repurpose a vector3 message. This function will
    /// only use the x component.
    private: void OnMsg(ConstGzString_VPtr &msg)
    {
      std::cerr << "Not enough commands for every joint \n" << msg->data_size(); 
      if(msg->data_size() < this->joints.size()){
      	std::cerr << "Not enough commands for every joint \n"; 
      }else{
      	for (int i = 0; i < this->joints.size(); i++) 
      	  this->SetVelocity(std::stod (msg->data(i)), i);
      }
    }

  };

  // Tell Gazebo about this plugin, so that Gazebo can call Load on this plugin.
  GZ_REGISTER_MODEL_PLUGIN(LaunchPlugin)
}
#endif