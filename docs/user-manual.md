# Robotic Arm User Manual

This document describes the operation and safe use of the robotic arm system.

The robotic arm is designed for automated picking, transportation and precise placement of workpieces between defined positions.

The system supports:

- Manual joystick control  
- Automatic execution of predefined motion sequences  

---

## Image Attribution

Some figures in this document include a joystick 3D model.

The joystick model “Joystick KY-023” by Thingiverse user UniversalXx  
is licensed under Creative Commons Attribution-ShareAlike (CC-BY-SA).

Full attribution details are provided in the main project README.

---

## System Startup and Referencing

After power-up the robotic arm automatically performs a referencing procedure.

All axes move to their defined home positions to establish a consistent internal coordinate reference.

⚠️ Do not interfere with the robot during referencing.

The system is ready for operation only after successful completion.

<div align="center" style="margin: 24px 0 32px 0;">
  <img src="images/referencing.png" alt="Referencing Procedure" height="420">
  <p style="margin-top: 10px;"><em>Figure: Automatic referencing movement after system startup.</em></p>
</div>

---

## Axis Overview

### Axis 1 – Base Rotation (A1)

Rotates the entire robotic arm around the vertical axis.

<div align="center" style="margin: 24px 0 32px 0;">
  <img src="images/axis1_base_rotation.png" alt="Axis 1" height="360">
  <p style="margin-top: 10px;"><em>Figure: Axis 1 – base rotation.</em></p>
</div>

### Axis 2 – Shoulder Axis (A2)

Moves the arm vertically and defines working height.

<div align="center" style="margin: 24px 0 32px 0;">
  <img src="images/axis2_shoulder.png" alt="Axis 2" height="360">
  <p style="margin-top: 10px;"><em>Figure: Axis 2 – shoulder movement.</em></p>
</div>

### Axis 3 – Elbow Axis (A3)

Extends or retracts the arm and enables positioning in depth direction.

<div align="center" style="margin: 24px 0 32px 0;">
  <img src="images/axis3_elbow.png" alt="Axis 3" height="360">
  <p style="margin-top: 10px;"><em>Figure: Axis 3 – elbow movement.</em></p>
</div>

### Axis 4 – Tool Tilt Axis (A4)

Tilts the gripper up and down to adjust tool orientation.

<div align="center" style="margin: 24px 0 32px 0;">
  <img src="images/axis4_tilt.png" alt="Axis 4" height="360">
  <p style="margin-top: 10px;"><em>Figure: Axis 4 – gripper tilt.</em></p>
</div>

### Axis 5 – Wrist Rotation (A5)

Rotates the gripper around its longitudinal axis.

**Activation**

Press both joystick push buttons simultaneously while both joysticks are in the neutral position.

After activation:

- Press left joystick → rotate in one direction  
- Press right joystick → rotate in opposite direction  

<div align="center" style="margin: 24px 0 32px 0;">
  <img src="images/axis5_wrist_rotation.png" alt="Axis 5" height="360">
  <p style="margin-top: 10px;"><em>Figure: Axis 5 – wrist rotation control.</em></p>
</div>

### Axis 6 – Gripper (G6)

Opens and closes the gripper for workpiece handling.

**Activation**

Press both joystick push buttons simultaneously to switch from wrist rotation mode back to gripper control mode.

- Press left joystick → open gripper  
- Press right joystick → close gripper  

<div align="center" style="margin: 24px 0 32px 0;">
  <img src="images/gripper_control.png" alt="Gripper" height="360">
  <p style="margin-top: 10px;"><em>Figure: Gripper opening and closing.</em></p>
</div>

---

## Operating Modes

### Manual Mode

In **Manual Mode**, the robotic arm is controlled directly via the joystick control panel.

- Joystick movement controls axis direction and speed  
- Push buttons control tool functions  

Manual Mode is intended for:

- Setup and adjustment  
- Testing movements  
- Maintenance  
- Teaching positions  

<div align="center" style="margin: 24px 0 32px 0;">
  <img src="images/manual_mode.png" alt="Manual Mode" height="360">
  <p style="margin-top: 10px;"><em>Figure: Manual operation using the dual-joystick control panel.</em></p>
</div>

### Automatic Mode

In **Automatic Mode**, the robotic arm executes predefined motion sequences.

Typical process:

1. Move to pick position  
2. Grip workpiece  
3. Transport along programmed path  
4. Place workpiece at target position  

Manual joystick inputs are disabled during automatic operation.

<div align="center" style="margin: 24px 0 32px 0;">
  <img src="images/automatic_mode.png" alt="Automatic Mode" height="360">
  <p style="margin-top: 10px;"><em>Figure: Automatic workpiece transport cycle.</em></p>
</div>

---

## Emergency Stop

The system includes an emergency stop function.

When activated:

- All robot motion stops immediately  

Before restarting operation:

- Inspect the system  
- Resolve the cause of the stop condition  

<div align="center" style="margin: 24px 0 32px 0;">
  <img src="images/emergency_stop.png" alt="Emergency Stop" height="360">
  <p style="margin-top: 10px;"><em>Figure: Emergency stop button on control panel.</em></p>
</div>

---