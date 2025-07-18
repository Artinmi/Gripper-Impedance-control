# Robotic Gripper with Impedance and Current Feedback1 Control
## Abstract
This project involves designing and implementing a **robotic gripper** that utilizes **impedance control** to adaptively grip objects of varying stiffness. The gripper is designed in **SolidWorks**, optimized, and then **3D printed**. The system integrates **two controllers**—an **impedance controller** and a **current feedback controller**—to ensure safe and effective gripping of different objects. The gripper consists of **two graspers** and can hold objects ranging from **1 cm to 5 cm** in size. A **hybrid control approach** is used to select the optimal gripping force by comparing the desired force calculated by both controllers. The real-time force values are displayed using an **Arduino and a display module**.
21
---

## 1. Table of Contents
1. [Abstract](#0-abstract)
2. [Introduction](#2-introduction)
3. [Impedance Controller](#3-impedance-controller)
4. [Current Feedback Controller](#4-current-feedback-controller)
5. [Hybrid Controller](#5-hybrid-controller)
6. [Arduino Integration](#6-arduino-integration)
7. [Display System](#7-display-system)
8. [Credits](#8-credits)

---

## 2. Introduction
Robotic grippers require precise control to handle objects of different stiffness without causing damage. **Impedance control** simulates a **mass-spring-damper system**, allowing adaptability in force exertion. In this project, we developed a robotic gripper that uses impedance control along with **current feedback control** to optimize gripping force. The system was first modeled and tested in **SolidWorks**, followed by **3D printing** the final design.

---

## 3. Impedance Controller
### How It Works
Impedance control treats the interaction between the robotic gripper and the object as a **spring-mass-damper system**. The force applied adapts based on the stiffness of the object, preventing excessive force that could damage fragile items like an **egg** or **plastic cup**.

### Why We Used It
- Ensures adaptive gripping for objects with different stiffness levels.
- Prevents excessive force application (avoiding breakage or deformation).
- Simulates a physical response similar to human gripping dynamics.

---

## 4. Current Feedback Controller
### Functionality
- The **INA219 sensor** is used to measure the current drawn by the servo motors.
- This feedback provides an estimate of the force applied by the gripper.
- A desired force is computed based on the real-time current readings.

### Purpose
- Ensures real-time force monitoring.
- Provides an additional method to calculate the required gripping force.
- Prevents excessive strain on the servo motors.

---

## 5. Hybrid Controller
### How It Works
- The **impedance controller** computes a **desired force** based on the object’s stiffness.
- The **current feedback controller** calculates another **desired force** from the measured servo current.
- The **lower force** from these two calculations is selected to grip the object safely.

### Benefits
- **Enhances gripping safety** by avoiding excessive force application.
- **Ensures adaptability** for different object types.
- **Optimizes servo motor performance** by reducing unnecessary load.

---

## 6. Arduino Integration
- An **Arduino board** is used as the central controller.
- It processes force calculations from both the **impedance** and **current feedback controllers**.
- Controls the **servo motors** based on the selected gripping force.
- Communicates with the **display module** to show real-time force values.

---

## 7. Display System
- A **display module** (LCD or OLED) is used to show real-time force values.
- Displays:
  - **Impedance controller’s calculated force**.
  - **Current feedback controller’s calculated force**.
- Helps in monitoring and debugging the gripping process.

---

## 8. Credits
- **Project Design & Development:** [Your Name]
- **Mechanical Design:** [Team Member's Name]
- **Electronics & Programming:** [Team Member's Name]
- **3D Printing & Prototyping:** [Team Member's Name]

For any inquiries or contributions, feel free to **open an issue** or **submit a pull request** on this GitHub repository.

---

### 🚀 Future Improvements
- Implementing **force sensors** for more precise gripping feedback.
- Exploring **machine learning** for adaptive grip tuning.
- Enhancing gripper **speed and efficiency** through optimized control algorithms.

Feel free to **star ⭐ the repository** if you find this project useful!

