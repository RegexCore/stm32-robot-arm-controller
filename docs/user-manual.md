# Robotic Arm User Manual

This document describes the operation and safe use of the robotic arm system during production use.

The robotic arm is designed for automated picking, transportation and precise placement of workpieces between defined positions.

The system can be operated in two modes:

- Manual mode via joystick control panel
- Automatic mode using predefined motion sequences

---

## System Startup and Referencing

After the system is powered on, the robotic arm automatically performs a calibration movement to its reference position.

During this referencing procedure:

- All axes move to their defined home positions
- A consistent internal position reference is established

⚠️ No manual intervention is allowed during referencing.

The robotic arm is ready for operation only after successful completion of the calibration process.

![Robot Arm Referencing](images/referencing.png)

---

## Axis Overview

### Axis 1 – Base Rotation (A1)

**Function**

- Rotates the entire robotic arm around the vertical axis
- Enables lateral positioning within the working area

![Axis 1 Base Rotation](images/axis1_base_rotation.png)

---

### Axis 2 – Shoulder Axis (A2)

**Function**

- Moves the arm up and down
- Defines working height and reach

![Axis 2 Shoulder](images/axis2_shoulder.png)

---

### Axis 3 – Elbow Axis (A3)

**Function**

- Extends or retracts the arm
- Enables precise positioning in depth direction

![Axis 3 Elbow](images/axis3_elbow.png)

---

### Axis 4 – Tool Tilt Axis (A4)

**Function**

- Tilts the gripper up and down
- Adjusts tool orientation

![Axis 4 Tilt](images/axis4_tilt.png)

---

### Axis 5 – Wrist Rotation (A5)

**Function**

- Rotates the gripper around its longitudinal axis
- Allows correct alignment of the workpiece

**Activation**

The wrist rotation mode is activated by pressing both joystick push buttons simultaneously while the joysticks are in the center position.

After activation:

- Pressing the left joystick rotates the gripper in one direction
- Pressing the right joystick rotates the gripper in the opposite direction

![Axis 5 Wrist Rotation](images/axis5_wrist_rotation.png)

---

### Axis 6 – Gripper Drive (G6)

**Function**

- Opens and closes the gripper
- Enables picking and releasing of workpieces

**Activation**

The gripper control mode is activated by pressing both joystick push buttons simultaneously while the joysticks are centered.

After activation:

- Pressing the left joystick opens the gripper
- Pressing the right joystick closes the gripper

![Gripper Control](images/gripper_control.png)

---

## Operating Modes

### Manual Mode

In manual mode the robotic arm is directly controlled via the joystick control panel.

Characteristics:

- Each joystick axis directly controls a corresponding robot axis
- Joystick deflection defines movement direction and speed
- Joystick push buttons control gripper functions

Manual mode is mainly used for:

- Setup and adjustment
- Testing movements
- Maintenance work
- Teaching positions

Switching to automatic mode is possible via the operating mode selector switch, provided the robotic arm is in a safe and defined state.

![Manual Operation](images/manual_mode.png)

---

### Automatic Mode

In automatic mode the robotic arm executes predefined motion sequences autonomously.

Typical process:

1. Move to predefined pick position  
2. Grip the workpiece  
3. Transport along programmed motion path  
4. Place the workpiece at target position  

All movements are executed reproducibly within configured limits for:

- motion range
- speed
- acceleration

⚠️ Manual joystick inputs are disabled during automatic operation.

Switching back to manual mode is possible via the operating mode selector.

![Automatic Operation](images/automatic_mode.png)

---

## Emergency Stop

The system provides an emergency stop function for hazardous situations.

When the emergency stop is activated:

- All robot motion is immediately stopped

Before resuming operation:

- The system must be inspected
- The cause of the emergency stop must be resolved

![Emergency Stop](images/emergency_stop.png)

---

## Safety Notes

- Do not reach into the robot workspace during operation
- Ensure referencing procedure is completed before use
- Use manual mode only for setup and maintenance
- Verify safe conditions before enabling automatic mode
- Always test new motion sequences at reduced speed