#ifndef _LAUNCH_PLUGIN_HH_
#define _LAUNCH_PLUGIN_HH_

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

    public: virtual void Load(physics::ModelPtr _model, sdf::ElementPtr _sdf)
    {
      // Safety check
      if (_model->GetJointCount() < 2)
      {
        std::cerr << "Invalid joint count, Velodyne plugin not loaded\n";
        return;
      }

      // Store the model pointer for convenience.
      this->model = _model;

      // Get the first joint. We are making an assumption about the model
      // having one joint that is the rotational joint.
      this->joint1 = _model->GetJoints()[0];
      this->joint2 = _model->GetJoints()[1];
      std::cout << "Joints: " << _model->GetJointCount();

      // Setup a P-controller, with a gain of 0.1.
      this->pid = common::PID(0.1, 0, 0);

      // Apply the P-controller to the joint.
      this->model->GetJointController()->SetVelocityPID(
          this->joint1->GetScopedName(), this->pid);
      this->model->GetJointController()->SetVelocityPID(
          this->joint2->GetScopedName(), this->pid);

      std::cout << "Gazebo: " << GAZEBO_MAJOR_VERSION;
      // Create the node
      this->node = transport::NodePtr(new transport::Node());
      this->node->Init(this->model->GetWorld()->GetName());

      std::cout << "Name: " << this->model->GetName();
      // Create a topic name
      std::string topicName1 = "~/" + this->model->GetName() + "/vel_cmd1";

      // Subscribe to the topic, and register a callback
      this->sub1 = this->node->Subscribe(topicName1,
         &LaunchPlugin::OnMsg1, this);

      // Create a topic name
      std::string topicName2 = "~/" + this->model->GetName() + "/vel_cmd2";

      // Subscribe to the topic, and register a callback
      this->sub2 = this->node->Subscribe(topicName2,
         &LaunchPlugin::OnMsg2, this);
    }

    /// \brief Set the velocity of the Velodyne
    /// \param[in] _vel New target velocity
    public: void SetVelocity1(const double &_vel)
    {
      // Set the joint's target velocity.
      this->model->GetJointController()->SetVelocityTarget(
          this->joint1->GetScopedName(), _vel);
    }

    /// \brief Set the velocity of the Velodyne
    /// \param[in] _vel New target velocity
    public: void SetVelocity2(const double &_vel)
    {
      // Set the joint's target velocity.
      this->model->GetJointController()->SetVelocityTarget(
          this->joint2->GetScopedName(), _vel);
    }

    /// \brief Handle incoming message
    /// \param[in] _msg Repurpose a vector3 message. This function will
    /// only use the x component.
    private: void OnMsg1(ConstVector3dPtr &_msg)
    {
      this->SetVelocity1(_msg->x());
    }

    /// \brief Handle incoming message
    /// \param[in] _msg Repurpose a vector3 message. This function will
    /// only use the x component.
    private: void OnMsg2(ConstVector3dPtr &_msg)
    {
      this->SetVelocity2(_msg->x());
    }


    /// \brief A node used for transport
    private: transport::NodePtr node;

    /// \brief A subscriber to a named topic.
    private: transport::SubscriberPtr sub1;

    /// \brief A subscriber to a named topic.
    private: transport::SubscriberPtr sub2;

    /// \brief Pointer to the model.
    private: physics::ModelPtr model;

    /// \brief Pointer to the joint.
    private: physics::JointPtr joint1;

    /// \brief Pointer to the joint.
    private: physics::JointPtr joint2;

    /// \brief A PID controller for the joint.
    private: common::PID pid;
  };

  // Tell Gazebo about this plugin, so that Gazebo can call Load on this plugin.
  GZ_REGISTER_MODEL_PLUGIN(LaunchPlugin)
}
#endif