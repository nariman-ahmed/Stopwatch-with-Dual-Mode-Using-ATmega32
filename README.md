# **Stopwatch with Dual Mode (Increment and Countdown) Using ATmega32 and Seven-Segment Display**

## 📋 **Project Overview**
This project is a digital stopwatch built with an **ATmega32 microcontroller** that offers two operational modes:  
1. **Increment Mode (Default):** Counts upwards from zero, displaying hours, minutes, and seconds on six seven-segment displays.  
2. **Countdown Mode:** Counts down from a user-defined time, functioning as a countdown timer.  

The system includes multiple push buttons for user interaction and an alarm/buzzer for notifications.  

## 🎯 **Objective**
To design and implement a versatile stopwatch that can switch between increment and countdown modes, offering essential features like reset, pause, and resume.

## 🛠️ **Features**
- **Reset Function:** Resets the time to zero in both modes.  
- **Pause Function:** Pauses the stopwatch, holding the current time.  
- **Resume Function:** Resumes counting from the paused time.  
- **Mode Toggle:** Allows switching between increment and countdown modes.  
- **Alarm/Buzzer:** Activates at the end of the countdown.  
- **LED Indicators:** Show the current mode and status.  

## ⚙️ **Hardware Components**
- **Microcontroller:** ATmega32 with a 16 MHz system clock.  
- **Display:** Six multiplexed seven-segment displays (common anode) controlled by a **7447 BCD to seven-segment decoder**.  
- **Push Buttons:** 10 buttons for start, stop, reset, mode toggle, and time adjustment.  
- **Alarm/Buzzer:** Provides audio feedback when the countdown reaches zero.  
- **LED Indicators:** Indicate the active mode (increment or countdown).  


## 🖼️ **Check out the demo**

Trying the Pause, Resume and Reset buttons on countup mode

https://github.com/user-attachments/assets/d50cff78-c1cc-4424-aa6a-5aa099369861

Switching to countdown mode

https://github.com/user-attachments/assets/e7a8a09e-9d0b-4491-bb41-0a227a94d076

Watch as the buzzer gets triggered as the countdown comes to an end!

https://github.com/user-attachments/assets/259b4ea0-1edc-4c66-a5b6-411f8417e449

