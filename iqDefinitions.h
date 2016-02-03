/*-----------------------------------------------------------------------------*/
/*                                                                             */
/*                        Copyright (c) James Pearman                          */
/*                                   2016                                      */
/*                            All Rights Reserved                              */
/*                                                                             */
/*-----------------------------------------------------------------------------*/
/*                                                                             */
/*    Module:     iqDefinitions.h                                              */
/*    Author:     James Pearman                                                */
/*    Created:    31 Jan 2016                                                  */
/*                                                                             */
/*                V1.00 31 Jan 2016  Initial release tested with V4.52         */
/*                                                                             */
/*-----------------------------------------------------------------------------*/
/*                                                                             */
/*    The author is supplying this software for use with the VEX IQ            */
/*    control system. This file can be freely distributed and teams are        */
/*    authorized to freely use this program , however, it is requested that    */
/*    improvements or additions be shared with the Vex community via the vex   */
/*    forum.  Please acknowledge the work of the authors when appropriate.     */
/*    Thanks.                                                                  */
/*                                                                             */
/*    Licensed under the Apache License, Version 2.0 (the "License");          */
/*    you may not use this file except in compliance with the License.         */
/*    You may obtain a copy of the License at                                  */
/*                                                                             */
/*      http://www.apache.org/licenses/LICENSE-2.0                             */
/*                                                                             */
/*    Unless required by applicable law or agreed to in writing, software      */
/*    distributed under the License is distributed on an "AS IS" BASIS,        */
/*    WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. */
/*    See the License for the specific language governing permissions and      */
/*    limitations under the License.                                           */
/*                                                                             */
/*    The author can be contacted on the vex forums as jpearman                */
/*    or electronic mail using jbpearman_at_mac_dot_com                        */
/*    Mentor for team 8888 RoboLancers, Pasadena CA.                           */
/*                                                                             */
/*-----------------------------------------------------------------------------*/
/*                                                                             */
/*    Description:                                                             */
/*    header file for the VexIQ default code examples                          */
/*-----------------------------------------------------------------------------*/

#ifndef _IQ_DEFINITIONS_
#define _IQ_DEFINITIONS_

// configuration menu items
typedef enum tConfigSelect {
    kConfig_exit = 0,
    kConfig_0    = 1,
    kConfig_1    = 2,
    kConfig_2    = 3,
    kConfig_3    = 4,
    kConfig_4    = 5,
    kConfig_5    = 6,
    kConfig_6    = 7,
    kConfig_7    = 8
} tConfigSelect;

// joystick control modes
typedef enum tJoystickMode {
    kJoystickTank,
    kJoystickArcadeLeft,
    kJoystickArcadeRight
} tJoystickMode;

// motor control direction
typedef enum tMotorDirection {
    kNormal,
    kReverse
} tMotorDirection;

// Motor data is collected together in a structure
typedef struct _IQMotor {
    bool              installed;    ///< is motor installed
    tMotorDirection   direction;    ///< reverse motor
    portName          fwdTouch;     ///< touch sensor port to stop motor going forwards
    portName          revTouch;     ///< touch sensor port to stop motor going reverse
    float             velocity;     ///< calculated velocity
    long              lastEncoder;
    long              lastTime;
} IQMotor;

// Default setting for config page if they are not stored
// in memory
#define   DRIVE_MODE_DEFAULT    kJoystickTank
#define   DRIVE_TURN_DEFAULT    kNormal
#define   DRIVE_FWD_DEFAULT     kNormal
#define   MOTOR_04_DEFAULT      kNormal
#define   MOTOR_05_DEFAULT      kNormal
#define   MOTOR_10_DEFAULT      kNormal
#define   MOTOR_11_DEFAULT      kNormal

//#define   IQ_TICK_PER_REV       360   ///< 360 counts/rev when set to deg
#define   IQ_TICK_PER_REV       960   ///< raw encoder counts/rev

// Globals to hold state and sensor info
#define PORTNONE  ((tSensors)(-1))

// LCD rows
#define LCD_ROW1    43
#define LCD_ROW2    34
#define LCD_ROW3    25
#define LCD_ROW4    16
#define LCD_ROW5     7
#define LCD_ROW6    -2
#define LCD_ROW7    -11
#define LCD_ROW8    -20
#define LCD_ROW9    -29

#define LCD_ROW      9

#endif
