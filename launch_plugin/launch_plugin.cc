#ifndef _LAUNCH_PLUGIN_HH_
#define _LAUNCH_PLUGIN_HH_

#include <gazebo/gazebo.hh>
#include <gazebo/physics/physics.hh>

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

      // Setup a P-controller, with a gain of 0.1.
      this->pid = common::PID(0.1, 0, 0);

      // Apply the P-controller to the joint.
      this->model->GetJointController()->SetVelocityPID(
          this->joint1->GetScopedName(), this->pid);
      this->model->GetJointController()->SetVelocityPID(
          this->joint2->GetScopedName(), this->pid);

      // Default to zero velocity
      double velocity1 = 0;
      double velocity2 = 0;

      // Check that the velocity element exists, then read the value
      if (_sdf->HasElement("velocity1"))
        velocity1 = _sdf->Get<double>("velocity1");
      if (_sdf->HasElement("velocity2"))
        velocity2 = _sdf->Get<double>("velocity2");

      // Set the joint's target velocity. This target velocity is just
      // for demonstration purposes.
      this->model->GetJointController()->SetVelocityTarget(
          this->joint1->GetScopedName(), velocity1);
      this->model->GetJointController()->SetVelocityTarget(
          this->joint2->GetScopedName(), velocity2);
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

    /// \brief A node used for transport
    private: transport::NodePtr node;

    /// \brief A subscriber to a named topic.
    private: transport::SubscriberPtr sub;

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