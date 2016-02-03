/*-----------------------------------------------------------------------------*/
/*                                                                             */
/*                        Copyright (c) James Pearman                          */
/*                                   2016                                      */
/*                            All Rights Reserved                              */
/*                                                                             */
/*-----------------------------------------------------------------------------*/
/*                                                                             */
/*    Module:     iqDefaultCode.c                                              */
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
/*                                                                             */
/*    A simulation of the Driver Control program for VEX IQ written in ROBOTC. */
/*                                                                             */
/*    PORT1  Left Drive Motor (optional)                                       */
/*    PORT2  Bumper switch, stop PORT4 motor going reverse. Or any sensor.     */
/*    PORT3  Bumper switch, stop PORT4 motor going forward. Or any sensor.     */
/*    PORT4  Auxilary motor (BtnRUp & BtnRDown). Or any sensor.                */
/*    PORT5  Auxilary motor (BtnFUp & BtnFDown). Or any sensor.                */
/*    PORT6  Right Drive Motor (optional)                                      */
/*                                                                             */
/*    PORT7  Left Drive Motor (optional)                                       */
/*    PORT8  Bumper switch, stop PORT10 motor going reverse. Or any sensor.    */
/*    PORT9  Bumper switch, stop PORT10 motor going forward. Or any sensor.    */
/*    PORT10 Auxilary motor (BtnLUp & BtnLDown). Or any sensor.                */
/*    PORT11 Auxilary motor (BtnEUp & BtnEDown. Or any sensor.)                */
/*    PORT12 Right Drive Motor (optional)                                      */
/*                                                                             */
/*    Optional sensors                                                         */
/*      Touch LED, enable/disable robot, any free port                         */
/*      Color Sensor, enable/disable robot, any free port                      */
/*      Gyro, reset robot to start orientation, any free port                  */
/*      Sonar (distance), stop robot hitting objects, any free port            */
/*                                                                             */
/*-----------------------------------------------------------------------------*/

#if !defined(VexIQ)
#error "This code is for the VexIQ platform"
#else

// Optional include of a logo drawing function
#include "iqRobotcLogo.c"

// Save config data
// Uncomment
// this line to allow non-volatile storage
#include "iqSettingsEeprom.c" // using EEPROM, saved when robot powered off

#include "iqSettingsSram.c"   // using RAM, not saved when robot powered off

// Custom types and definitions
#include "iqDefinitions.h"

// Globals used by the code
tJoystickMode   configDriveMode  = DRIVE_MODE_DEFAULT;
tMotorDirection configDriveTurn  = DRIVE_TURN_DEFAULT;
bool            robotRunning     = false;
bool            robotEnabled     = true;
bool            sonarFront       = true;
tSensors        robotTouchSensor = PORTNONE;
tSensors        robotColorSensor = PORTNONE;
tSensors        robotGyroSensor  = PORTNONE;
tSensors        robotSonarSensor = PORTNONE;
IQMotor         IQMotors[ kNumbOfRealMotors ];
short           robotTotalDevices;

/*-----------------------------------------------------------------------------*/
/*  Masks for our config data                                                  */
/*-----------------------------------------------------------------------------*/
#define   CODE_MASK  0xFFFF0000
#define   CODE_IDENT 0xC0DE0000
#define   MODE_MASK  0x00000003
#define   TURN_MASK  0x00000004
#define   M01_MASK   0x00000008
#define   M04_MASK   0x00000010
#define   M05_MASK   0x00000020
#define   M10_MASK   0x00000040
#define   M11_MASK   0x00000080
#define   SONAR_MASK 0x00000100
/*-----------------------------------------------------------------------------*/
/*    Store configuration settings                                             */
/*-----------------------------------------------------------------------------*/
void
iqSettingsStore()
{
    unsigned long data;

    data = iqSettingsReadData();

    switch(configDriveMode)
        {
        case  kJoystickTank:
            data = (data & ~MODE_MASK);
            break;
        case  kJoystickArcadeLeft:
            data = (data & ~MODE_MASK) | 0x00000001;
            break;
        case  kJoystickArcadeRight:
            data = (data & ~MODE_MASK) | 0x00000002;
            break;
        }

    if( configDriveTurn == kNormal )
        data &= ~TURN_MASK;
    else
        data |=  TURN_MASK;

    if( IQMotors[PORT1 ].direction == kNormal )
        data &= ~M01_MASK;
    else
        data |=  M01_MASK;

    if( IQMotors[PORT4 ].direction == kNormal )
        data &= ~M04_MASK;
    else
        data |=  M04_MASK;
    if( IQMotors[PORT5 ].direction == kNormal )
        data &= ~M05_MASK;
    else
        data |=  M05_MASK;
    if( IQMotors[PORT10].direction == kNormal )
        data &= ~M10_MASK;
    else
        data |=  M10_MASK;
    if( IQMotors[PORT11].direction == kNormal )
        data &= ~M11_MASK;
    else
        data |=  M11_MASK;
    if( sonarFront )
        data &= ~SONAR_MASK;
    else
        data |=  SONAR_MASK;

    iqSettingsWriteData( data );
}

/*-----------------------------------------------------------------------------*/
/*    Never used so init the EEPROM word                                       */
/*-----------------------------------------------------------------------------*/
void
iqSettingsInit( unsigned long data )
{
    iqSettingsWriteData( data );
}

/*-----------------------------------------------------------------------------*/
/*    Retrieve data word and setup configure options                           */
/*-----------------------------------------------------------------------------*/
void
iqSettingsRetrieve()
{
    unsigned long data;

    data = iqSettingsReadData();

    if( (data & CODE_MASK) == CODE_IDENT ) {
      //writeDebugStreamLine("Load %08X", data );
      switch( data & MODE_MASK )
          {
          case  0:
            configDriveMode = kJoystickTank;
            break;
          case  1:
            configDriveMode = kJoystickArcadeLeft;
            break;
          case  2:
            configDriveMode = kJoystickArcadeRight;
            break;
          }

      configDriveTurn            = (data & TURN_MASK) ? kReverse : kNormal;
      IQMotors[PORT1 ].direction = (data & M01_MASK) ? kReverse : kNormal;
      IQMotors[PORT6 ].direction = (data & M01_MASK) ? kReverse : kNormal;
      IQMotors[PORT4 ].direction = (data & M04_MASK) ? kReverse : kNormal;
      IQMotors[PORT5 ].direction = (data & M05_MASK) ? kReverse : kNormal;
      IQMotors[PORT10].direction = (data & M10_MASK) ? kReverse : kNormal;
      IQMotors[PORT11].direction = (data & M11_MASK) ? kReverse : kNormal;
      sonarFront = (data & SONAR_MASK) ? false : true;
    }
    else {
        if( iqSettingsIntegrityCheck() )
            iqSettingsInit( CODE_IDENT );
    }
}

/*-----------------------------------------------------------------------------*/
/*  Reset configuration and save                                               */
/*-----------------------------------------------------------------------------*/
void
iqResetToDefaults()
{
    configDriveMode  = DRIVE_MODE_DEFAULT;
    configDriveTurn  = DRIVE_TURN_DEFAULT;

    IQMotors[ (short)PORT1  ].direction = DRIVE_FWD_DEFAULT;
    IQMotors[ (short)PORT6  ].direction = DRIVE_FWD_DEFAULT;
    IQMotors[ (short)PORT4  ].direction = MOTOR_04_DEFAULT;
    IQMotors[ (short)PORT5  ].direction = MOTOR_05_DEFAULT;
    IQMotors[ (short)PORT10 ].direction = MOTOR_10_DEFAULT;
    IQMotors[ (short)PORT11 ].direction = MOTOR_11_DEFAULT;

    sonarFront = true;

    iqSettingsStore();

    playSound(soundTada);
}

/*-----------------------------------------------------------------------------*/
/*  Control robot wheels                                                       */
/*-----------------------------------------------------------------------------*/
void
iqRunRobotDrive()
{
    short leftDrive;
    short rightDrive;

    if( robotEnabled ) {
        switch( configDriveMode )
            {
            case kJoystickTank:
              leftDrive  = vexRT[ ChA ];
              rightDrive = vexRT[ ChD ];
              break;
            case kJoystickArcadeLeft:
              leftDrive  = vexRT[ ChA ] + vexRT[ ChB ];
              rightDrive = vexRT[ ChA ] - vexRT[ ChB ];
              break;
            case kJoystickArcadeRight:
              leftDrive  = vexRT[ ChD ] + vexRT[ ChC ];
              rightDrive = vexRT[ ChD ] - vexRT[ ChC ];
              break;
            }

        // threshold
        if( abs(leftDrive)  < 10 ) leftDrive  = 0;
        if( abs(rightDrive) < 10 ) rightDrive = 0;

        // flip drive direction based on configuration
        leftDrive  = (IQMotors[PORT1 ].direction == kNormal) ? leftDrive  : -leftDrive;
        rightDrive = (IQMotors[PORT6 ].direction == kNormal) ? rightDrive : -rightDrive;

        // reverse left and right & flips signs
        // Not sure I agree with this but it makes robot behave
        // as per the default driver program.
        // Robot front is now back.
        if( configDriveTurn == kReverse ) {
            int tmp = leftDrive;
            leftDrive  = -rightDrive;
            rightDrive = -tmp;
        }

        // When moving reset a timer used to start gyro control
        if( leftDrive || rightDrive )
            clearTimer( T1 );

        // Gyro sensor
        if( robotGyroSensor != PORTNONE ) {
            // If stopped for more than 500mS then reorientate using gyro
            if( time1[ T1 ] >= 500 ) {
                int heading = getGyroHeading(robotGyroSensor);

                // small amount of hysteresis
                if( abs( heading ) < 2 ) {
                    leftDrive = 0;
                    rightDrive = 0;
                }
                else
                if( heading > 3 ) {
                    leftDrive  =  30 + heading / 2;
                    rightDrive = -30 - heading / 2;
                }
                else
                if( heading < -3 ) {
                    leftDrive  = -30 + heading / 2;
                    rightDrive =  30 - heading / 2;
                }
            }
        }

        // Sonar sensor installed
        // stop motors going forwards
        if( robotSonarSensor != PORTNONE ) {
          // stop at 15cm
          if( getDistanceValue(robotSonarSensor) < 150 )
            {
            if( (sonarFront && leftDrive > 0) || (!sonarFront && leftDrive < 0) )
              leftDrive = 0;
            if( (sonarFront && rightDrive > 0) || (!sonarFront && rightDrive < 0) )
              rightDrive = 0;
            }
        }
    }
    else {
        leftDrive  = 0;
        rightDrive = 0;
    }

    // Control drive motors.
    if( IQMotors[ PORT1 ].installed )
      motor[ PORT1  ] = leftDrive;
    if( IQMotors[ PORT7 ].installed )
      motor[ PORT7  ] = leftDrive;
    if( IQMotors[ PORT6 ].installed )
      motor[ PORT6  ] = -rightDrive; // right is reversed
    if( IQMotors[ PORT12 ].installed )
      motor[ PORT12 ] = -rightDrive; // right is reversed
}

/*-----------------------------------------------------------------------------*/
/*  Control the four Auxiliary motors, ports 4, 5, 10 & 11                     */
/*-----------------------------------------------------------------------------*/
void
iqRunRobotAux()
{
    int m04Drive;
    int m05Drive;
    int m10Drive;
    int m11Drive;

    if( robotEnabled ) {
        // Control the auxilary motors
        m04Drive = (vexRT[ BtnRUp ] * 100) - (vexRT[ BtnRDown ] * 100);
        m05Drive = (vexRT[ BtnFUp ] * 100) - (vexRT[ BtnFDown ] * 100);
        m10Drive = (vexRT[ BtnLUp ] * 100) - (vexRT[ BtnLDown ] * 100);
        m11Drive = (vexRT[ BtnEUp ] * 100) - (vexRT[ BtnEDown ] * 100);

        m04Drive = (IQMotors[PORT4 ].direction == kNormal) ? m04Drive : -m04Drive;
        m05Drive = (IQMotors[PORT5 ].direction == kNormal) ? m05Drive : -m05Drive;
        m10Drive = (IQMotors[PORT10].direction == kNormal) ? m10Drive : -m10Drive;
        m11Drive = (IQMotors[PORT11].direction == kNormal) ? m11Drive : -m11Drive;

        // Deal with touch sensors
        // stopping motor 4 and motor 10
        if( IQMotors[ PORT4 ].fwdTouch >= 0 ) {
            if( m04Drive > 0 && SensorValue[ IQMotors[ PORT4 ].fwdTouch ] )
                m04Drive = 0;
        }
        if( IQMotors[ PORT4 ].revTouch >= 0 ) {
            if( m04Drive < 0 && SensorValue[ IQMotors[ PORT4 ].revTouch ] )
                m04Drive = 0;
        }
        if( IQMotors[ PORT10 ].fwdTouch >= 0 ) {
            if( m10Drive > 0 && SensorValue[ IQMotors[ PORT10 ].fwdTouch ] )
                m10Drive = 0;
        }
        if( IQMotors[ PORT10 ].revTouch >= 0 ) {
            if( m10Drive < 0 && SensorValue[ IQMotors[ PORT10 ].revTouch ] )
                m10Drive = 0;
        }
    } // robotEnabled
    else {
        // Robot disabled
        m04Drive = m05Drive = m10Drive = m11Drive = 0;
    }

    // Control Aux motors
    if(IQMotors[ PORT4 ].installed)
      motor[ PORT4  ] = m04Drive;
    if(IQMotors[ PORT5 ].installed)
      motor[ PORT5  ] = m05Drive;
    if(IQMotors[ PORT10 ].installed)
      motor[ PORT10 ] = m10Drive;
    if(IQMotors[ PORT11 ].installed)
      motor[ PORT11 ] = m11Drive;
}

/*-----------------------------------------------------------------------------*/
/*  Check the touch LED and color sensors                                      */
/*-----------------------------------------------------------------------------*/
void
iqCheckSensors()
{
    static bool robotTouchSensorPressed;

    // Touch LED
    if( robotTouchSensor != PORTNONE ) {
        if( getTouchLEDValue( robotTouchSensor ) ) {
            if(!robotTouchSensorPressed) {
                robotEnabled = !robotEnabled;
                robotTouchSensorPressed = true;
            }
        }
        else {
            robotTouchSensorPressed = false;
        }
    }

    // Color sensor
    if( robotColorSensor != PORTNONE ) {
        // close object ?
        if( getColorProximity( robotColorSensor ) > 200 ) {
            TSimpleColors  color;
            color = getColorName( robotColorSensor );

            if( color == colorRed )
                robotEnabled = false;

            if( color == colorGreen )
                robotEnabled = true;
        }
    }

    // Display enabled status on the touch LED
    if( robotTouchSensor != PORTNONE ) {
        if( robotRunning ) {
            if(robotEnabled)
                setTouchLEDColor( robotTouchSensor, colorGreen );
            else
                setTouchLEDColor( robotTouchSensor, colorRed );
        }
        else
            setTouchLEDColor( robotTouchSensor, colorNone );
    }
}

/*-----------------------------------------------------------------------------*/
/*  Task used to drive the robot and handle all the discovered sensors         */
/*-----------------------------------------------------------------------------*/
task
iqRunRobotTask()
{
    while(1)
        {
        // Request to kill this task, we need to cleanup first
        // set to disabled to stop motors
        if( !robotRunning )
            robotEnabled = false;

        // Drive motors
        iqRunRobotDrive();
        // Aux motors
        iqRunRobotAux();
        // Check sensors
        iqCheckSensors();

        // done
        if( !robotRunning )
          break;

        // Don't hog the cpu
        wait1Msec(25);
        }
}

/*-----------------------------------------------------------------------------*/
/*  Calibrate gyro if installed                                                */
/*-----------------------------------------------------------------------------*/
void
iqCalibrateGyro()
{
    if( robotGyroSensor != PORTNONE ) {
        short count = 20;

        // VEX sets touch sensor yellow when calibrating
        if( robotTouchSensor != PORTNONE )
            setTouchLEDColor( robotTouchSensor, colorYellow );

        startGyroCalibration( robotGyroSensor, gyroCalibrateSamples512 );
        // delay so calibrate flag can be set internally to the gyro
        wait1Msec(100);

        eraseDisplay();

        // wait for calibration to finish or 2 seconds, whichever is longer
        while( getGyroCalibrationFlag(robotGyroSensor) || (count-- > 0) ) {
            char str[32];
            sprintf( str, "Calibrating...     %02d", count/10 );
            displayStringAt( 1, LCD_ROW1, str);
            wait1Msec(100);
        }

        // reset so this is 0 heading
        resetGyro(robotGyroSensor);
    }
}

/*-----------------------------------------------------------------------------*/
/*  Display one line on the IQ display with inverted text if selected          */
/*-----------------------------------------------------------------------------*/
void
iqDisplayLine( int row, bool select, char *str )
{
    if( row < 0 || row > 43 )
        return;

    if( select ) {
        fillRect( 0, row+1, 127, row-8);
        displayInverseStringAt( 1, row, str );
    }
    else {
        eraseRect( 0, row+1, 127, row-8);
        displayStringAt( 1, row, str);
    }
}

/*-----------------------------------------------------------------------------*/
/*  Redisplay the configuration menu                                           */
/*-----------------------------------------------------------------------------*/
void
iqMenuConfigurationDisplay( tConfigSelect selected, int offset )
{
    char  str[32];
    int rowOffset = offset * LCD_ROW;

    iqDisplayLine( LCD_ROW1 + rowOffset, selected == kConfig_exit, "Exit                 " );

    switch( configDriveMode )
    {
        case  kJoystickTank:        strcpy( str, "Control   2 Joystick "); break;
        case  kJoystickArcadeLeft:  strcpy( str, "Control   Left Stick "); break;
        case  kJoystickArcadeRight: strcpy( str, "Control   Right Stick"); break;
        default:                    strcpy( str, "Control   Error      "); break;
    }
    iqDisplayLine( LCD_ROW2 + rowOffset, selected == kConfig_0, str );

    switch( configDriveTurn )
    {
        case  kNormal:  strcpy( str, "DriveTurn Normal     "); break;
        case  kReverse: strcpy( str, "DriveTurn Reverse    "); break;
        default:        strcpy( str, "DriveTurn Error      "); break;
    }
    iqDisplayLine( LCD_ROW3 + rowOffset, selected == kConfig_1, str );

    switch( IQMotors[ PORT1 ].direction )
    {
        case  kNormal:  strcpy( str, "Drive Fwd Normal     "); break;
        case  kReverse: strcpy( str, "Drive Fwd Reverse    "); break;
        default:        strcpy( str, "Drive Fwd Error      "); break;
    }
    iqDisplayLine( LCD_ROW4 + rowOffset, selected == kConfig_2, str );

    switch( IQMotors[ PORT4 ].direction )
    {
        case  kNormal:  strcpy( str, "Motor 4   Normal     "); break;
        case  kReverse: strcpy( str, "Motor 4   Reverse    "); break;
        default:        strcpy( str, "Motor 4   Error      "); break;
    }
    iqDisplayLine( LCD_ROW5 + rowOffset, selected == kConfig_3, str );

    switch( IQMotors[ PORT5 ].direction )
    {
        case  kNormal:  strcpy( str, "Motor 5   Normal     ");break;
        case  kReverse: strcpy( str, "Motor 5   Reverse    ");break;
        default:        strcpy( str, "Motor 5   Error      "); break;
    }
    iqDisplayLine( LCD_ROW6 + rowOffset, selected == kConfig_4, str );

    switch( IQMotors[ PORT10 ].direction )
    {
        case  kNormal:  strcpy( str, "Motor 10  Normal     ");break;
        case  kReverse: strcpy( str, "Motor 10  Reverse    ");break;
        default:        strcpy( str, "Motor 10  Error      "); break;
    }
    iqDisplayLine( LCD_ROW7 + rowOffset, selected == kConfig_5, str );

    switch( IQMotors[ PORT11 ].direction )
    {
        case  kNormal:  strcpy( str, "Motor 11  Normal     ");break;
        case  kReverse: strcpy( str, "Motor 11  Reverse    ");break;
        default:        strcpy( str, "Motor 11  Error      "); break;
    }
    iqDisplayLine( LCD_ROW8 + rowOffset, selected == kConfig_6, str );

    switch( sonarFront )
    {
        case  true    : strcpy( str, "Sonar     Front      ");break;
        default:        strcpy( str, "Sonar     Rear       ");break;
    }
    iqDisplayLine( LCD_ROW9 + rowOffset, selected == kConfig_7, str );
}

/*-----------------------------------------------------------------------------*/
/*  Action the select button when in the configure menu                        */
/*  returns true if the exit item was selected                                 */
/*-----------------------------------------------------------------------------*/
bool
iqMenuConfigurationSelect( tConfigSelect selected )
{
  switch(selected)
    {
    case  kConfig_exit:
        return( true );
        break;

    case  kConfig_0:
        switch(configDriveMode)
        {
            case  kJoystickTank:        configDriveMode = kJoystickArcadeRight; break;
            case  kJoystickArcadeRight: configDriveMode = kJoystickArcadeLeft;  break;
            case  kJoystickArcadeLeft:  configDriveMode = kJoystickTank;        break;
            default:                    configDriveMode = kJoystickTank;        break;
        }
        break;

    case  kConfig_1:
        configDriveTurn = (configDriveTurn == kNormal) ? kReverse : kNormal;
        break;

    case  kConfig_2:
        // Use motors on port1 and port6 as reference even if they are not installed
        IQMotors[ PORT1  ].direction = (IQMotors[ PORT1  ].direction == kNormal) ? kReverse : kNormal;
        IQMotors[ PORT6  ].direction = (IQMotors[ PORT6  ].direction == kNormal) ? kReverse : kNormal;
        break;
    case  kConfig_3:
        IQMotors[ PORT4  ].direction = (IQMotors[ PORT4  ].direction  == kNormal) ? kReverse : kNormal;
        break;
    case  kConfig_4:
        IQMotors[ PORT5  ].direction = (IQMotors[ PORT5  ].direction  == kNormal) ? kReverse : kNormal;
        break;
    case  kConfig_5:
        IQMotors[ PORT10 ].direction = (IQMotors[ PORT10 ].direction == kNormal) ? kReverse : kNormal;
        break;
    case  kConfig_6:
        IQMotors[ PORT11 ].direction = (IQMotors[ PORT11 ].direction == kNormal) ? kReverse : kNormal;
        break;
    case  kConfig_7:
        sonarFront = !sonarFront;
        break;

    default:
        break;
    }

    return( false );
}

/*-----------------------------------------------------------------------------*/
/*  Action the "configure" menu item                                           */
/*-----------------------------------------------------------------------------*/
void
iqMenuConfiguration()
{
    static  tConfigSelect configSelect = kConfig_exit;
    static int offset = 0;
    bool done   = false;
    bool update = true;
    bool dirty  = false;

    // Wait for LCD button release
    while( nLCDButtons != kButtonNone )
    wait1Msec(10);

    eraseDisplay();

    while( !done ) {
        // Display menu
        if( update ) {
            iqMenuConfigurationDisplay( configSelect, offset );
            update = false;
        }

        // Check LCD buttons
        if( nLCDButtons != kButtonNone ) {
            if( nLCDButtons == kButtonDown  ) {
                if( configSelect++ == kConfig_7 )
                    configSelect = kConfig_7;
                if( configSelect > kConfig_3 && offset == 0 )
                    offset = 4;
            }

            if( nLCDButtons == kButtonUp  ) {
                if( configSelect-- == kConfig_exit )
                    configSelect = kConfig_exit;
                if( configSelect < kConfig_3 && offset == 4 )
                    offset = 0;
            }

            if( nLCDButtons == kButtonSelect  ) {
                if( iqMenuConfigurationSelect( configSelect ) ) {
                    if( dirty )
                        iqSettingsStore();

                    done = true;
                }
                else
                    dirty = true;
            }

            // Exit doesn't really work
            if( nLCDButtons == kButtonExit  ) {
                done = true;
            }

            // Wait for LCD button release
            while( nLCDButtons != kButtonNone )
                wait1Msec(10);

            // update display
            update = true;
        }

    wait1Msec(25);
    }

    // erase before we leave this sub menu
    eraseDisplay();
}

/*-----------------------------------------------------------------------------*/
/*  Function to draw up or down arrow as the LCD character set seems limited   */
/*-----------------------------------------------------------------------------*/

void
drawArrow( int xpos, int ypos, bool dir )
{
    int i;

    eraseRect(xpos, ypos, xpos+8, ypos+4);

    if( dir ) { // Up arrow
        for(i=0;i<=4;i++)
            drawLine( xpos+i, ypos+i, xpos + (8-i), ypos+i );
    }
    else {      // Down arrow
        for(i=0;i<=4;i++)
            drawLine( xpos+(4-i), ypos+i, xpos + (4+i), ypos+i );
    }
}

/*-----------------------------------------------------------------------------*/
/*  Display a signed value on the LCD, standard character set has an offset    */
/*  between + and - characters, fix that here.                                 */
/*-----------------------------------------------------------------------------*/
void
displaySignedValueAt( short x, short y, int value, bool zeroSign = true )
{
    if(( value >  0 ) || (value == 0 && zeroSign))
        displayStringAt( x, y,   "+" );
    else
    if( value <  0 )
        displayStringAt( x, y+1, "-" );

    displayStringAt( x+6, y, "%3d", abs(value) );
}

/*-----------------------------------------------------------------------------*/
/*  Display controller status                                                  */
/*  Long form formatting to try and match the VEX version under driver control */
/*  ROBOTC standard text position does not match the VEX display               */
/*-----------------------------------------------------------------------------*/
void
iqRunDisplayJoystickStatus()
{
    // background text
    displayStringAt(  1, LCD_ROW1, "Button     Axis" );
    displayStringAt(  1, LCD_ROW2, "E" );
    displayStringAt(  1, LCD_ROW3, "F" );
    displayStringAt(  1, LCD_ROW4, "R" );
    displayStringAt(  1, LCD_ROW5, "L" );
    displayStringAt( 67, LCD_ROW2, "A" );
    displayStringAt( 67, LCD_ROW3, "B" );
    displayStringAt( 67, LCD_ROW4, "C" );
    displayStringAt( 67, LCD_ROW5, "D" );

    // Display button status
    if( vexRT[ BtnEDown ] )
        drawArrow( 13, LCD_ROW2-6, false );
    else
        displayStringAt( 13, LCD_ROW2, "  " );
    if( vexRT[ BtnEUp ] )
        drawArrow( 23, LCD_ROW2-6, true );
    else
        displayStringAt( 23, LCD_ROW2, "  " );
    if( vexRT[ BtnFDown ] )
        drawArrow( 13, LCD_ROW3-6, false );
    else
        displayStringAt( 13, LCD_ROW3, "  " );
    if( vexRT[ BtnFUp ] )
        drawArrow( 23, LCD_ROW3-6, true );
    else
        displayStringAt( 23, LCD_ROW3, "  " );
    if( vexRT[ BtnRDown ] )
        drawArrow( 13, LCD_ROW4-6, false );
    else
        displayStringAt( 13, LCD_ROW4, "  " );
    if( vexRT[ BtnRUp ] )
        drawArrow( 23, LCD_ROW4-6, true );
    else
        displayStringAt( 23, LCD_ROW4, "  " );
    if( vexRT[ BtnLDown ] )
        drawArrow( 13, LCD_ROW5-6, false );
    else
        displayStringAt( 13, LCD_ROW5, "  " );
    if( vexRT[ BtnLUp ] )
        drawArrow( 23, LCD_ROW5-6, true );
    else
        displayStringAt( 23, LCD_ROW5, "  " );

    // Display controller value
    displaySignedValueAt( 79, LCD_ROW2, vexRT[ ChA ] );
    displaySignedValueAt( 79, LCD_ROW3, vexRT[ ChB ] );
    displaySignedValueAt( 79, LCD_ROW4, vexRT[ ChC ] );
    displaySignedValueAt( 79, LCD_ROW5, vexRT[ ChD ] );

    // Percent symbol needs fixing somehow (reversed??)
    // and dropped one line
    displayStringAt( 104, LCD_ROW2+1,"%%");
    displayStringAt( 104, LCD_ROW3+1,"%%");
    displayStringAt( 104, LCD_ROW4+1,"%%");
    displayStringAt( 104, LCD_ROW5+1,"%%");
}

/*-----------------------------------------------------------------------------*/
/*  Display device status information when running                             */
/*-----------------------------------------------------------------------------*/
void
iqRunDisplayDeviceStatus( int page )
{
    TVexIQDeviceTypes   type;
    TDeviceStatus       status;
    short               ver;
    short               index;
    short               port;

    short               deviceCount = 0;
    short               deviceStart = (page * 4) + 1;
    short               displayRow  = LCD_ROW1;

    displayStringAt( 1, LCD_ROW1, "Port      Status" );

    if( robotTotalDevices == 0 )
        return;

    // encoder counts are raw
    setMotorEncoderUnits( encoderCounts );

    // Get all device info
    for(index=(short)PORT1;index<=(short)PORT12;index++) {
        port = index + 1;
        getVexIqDeviceInfo( index, type, status, ver );

        // found a device
        if( type != vexIQ_SensorNone )
          deviceCount++;
        else
          continue;

        // device for this page ?
        if( deviceCount < deviceStart )
          continue;

        // next row
        displayRow = displayRow - LCD_ROW;

        // Motor
        if( type == vexIQ_SensorMOTOR ) {
          // We have to calculate velocity as ROBOTC
          // returns the commanded motor power when using getMotorSpeed
          short deltaMs = nSysTime - IQMotors[ index ].lastTime;
          short deltaEn = getMotorEncoder( index ) - IQMotors[ index ].lastEncoder;
          IQMotors[ index ].lastTime = nSysTime;
          IQMotors[ index ].lastEncoder = getMotorEncoder( index );

          displayStringAt( 1, displayRow, "%2d Motor       rpm", port );
          IQMotors[ index ].velocity = (1000.0 / deltaMs) * deltaEn * 60.0 / IQ_TICK_PER_REV;

          displaySignedValueAt( 61, displayRow, (int)IQMotors[ index ].velocity );
        }

        // bumper switch
        if( type == vexIQ_SensorBUMPER ) {
          if( getBumperValue( index ) )
            displayStringAt( 1, displayRow, "%2d Bumper Pressed ", port );
          else
            displayStringAt( 1, displayRow, "%2d Bumper Released", port );
        }

        // Touch LED
        if( type == vexIQ_SensorLED ) {
          if( getTouchLEDValue( index ) )
            displayStringAt( 1, displayRow, "%2d Led    Pressed ", port );
          else
            displayStringAt( 1, displayRow, "%2d Led    Released", port );
        }

        // Color sensor
        if( type == vexIQ_SensorRGB ) {
          displayStringAt( 1, displayRow, "%2d Color  ", port );
          if( getColorProximity(index) > 200 ) {
              switch( getColorName( index ) )
                  {
                  default:              displayStringAt( 61, displayRow, "No Object "); break;
                  case  colorRedViolet: displayStringAt( 61, displayRow, "RED/VIOLET"); break;
                  case  colorRed:       displayStringAt( 61, displayRow, "RED       "); break;
                  case  colorDarkOrange:displayStringAt( 61, displayRow, "DRK ORANGE"); break;
                  case  colorOrange:    displayStringAt( 61, displayRow, "ORANGE    "); break;
                  case  colorDarkYellow:displayStringAt( 61, displayRow, "DRK YELLOW"); break;
                  case  colorYellow:    displayStringAt( 61, displayRow, "YELLOW    "); break;
                  case  colorLimeGreen: displayStringAt( 61, displayRow, "LIME GREEN"); break;
                  case  colorGreen:     displayStringAt( 61, displayRow, "GREEN     "); break;
                  case  colorBlueGreen: displayStringAt( 61, displayRow, "BLUE/GREEN"); break;
                  case  colorBlue:      displayStringAt( 61, displayRow, "BLUE      "); break;
                  case  colorDarkBlue:  displayStringAt( 61, displayRow, "DRK BLUE  "); break;
                  case  colorViolet:    displayStringAt( 61, displayRow, "VIOLET    "); break;
                  }
            }
            else
                displayStringAt( 61, displayRow, "No Object "); break;
         }

        // Gyro
        if( type == vexIQ_SensorGYRO ) {
            int angle = getGyroHeading( index );
            if( angle < 0 ) angle += 360;
            displayStringAt( 1, displayRow, "%2d Gyro    %3d deg", port, angle );
        }

        // Sonar
        if( type == vexIQ_SensorSONAR ) {
            float dist = getDistanceValue(robotSonarSensor);
            displayStringAt( 1, displayRow, "%2d Dist   %4d in", port, (int)(dist/25.4) );
        }
    }
}

/*-----------------------------------------------------------------------------*/
/*  Display logo on the home screen when running                               */
/*-----------------------------------------------------------------------------*/
void
iqRunDisplayLogo()
{
#ifdef _LOGO
    drawBitmap( logo );
#else
    displayCenteredBigTextLine(2, "ROBOTC");
#endif
}

/*-----------------------------------------------------------------------------*/
/*  Display the little check box with some text next to it                     */
/*-----------------------------------------------------------------------------*/
void
iqDrawSelectButtonAndText( int xpos, int ypos, char *text )
{
    drawLine( xpos+ 1, ypos,   xpos+ 9, ypos   );
    drawLine( xpos+ 1, ypos+6, xpos+ 9, ypos+6 );
    drawLine( xpos   , ypos+1, xpos   , ypos+5 );
    drawLine( xpos+10, ypos+1, xpos+10, ypos+5 );
    drawLine( xpos+ 2, ypos+3, xpos+ 4, ypos+1 );
    drawLine( xpos+ 4, ypos+1, xpos+ 8, ypos+5 );
    displayStringAt( xpos+12, ypos+6, text );
}
/*-----------------------------------------------------------------------------*/
/*  Display information for home screen when running                           */
/*-----------------------------------------------------------------------------*/
void
iqRunDisplayHome( bool showExit = false )
{
    static  long  nPgmStartTime = nPgmTime;

    // display static text
    displayStringAt(  1, LCD_ROW1, "Running" );

    // Calculate robot running time
    long secs  = (nPgmTime - nPgmStartTime)/1000;
    int  mins  = secs / 60;
    int  hours = mins / 60;

    // display program running time
    displayStringAt(  80, LCD_ROW1, "%02d:%02d:%02d", hours, mins % 60, secs % 60 );

    // run/stop/exit select box
    if( showExit )
      iqDrawSelectButtonAndText( 0, 1, "Exit");
    else
    if( robotEnabled )
      iqDrawSelectButtonAndText( 0, 1, "Stop");
    else
      iqDrawSelectButtonAndText( 0, 1, "Run ");

    // Display sonar
    if( robotSonarSensor != PORTNONE ) {
        int dist = getDistanceValue(robotSonarSensor);

        if( dist > 1000 )
            displayStringAt(  61, LCD_ROW5, "----", dist);
        else
            displayStringAt(  61, LCD_ROW5, "%04d", dist);
    }

    // Display gyro
    if( robotGyroSensor != PORTNONE ) {
        int heading = getGyroHeading(robotGyroSensor);
        if( heading < 0 ) heading += 360;
        displayStringAt(  100, LCD_ROW5, "%04d", heading);
    }
}

/*-----------------------------------------------------------------------------*/
/*  Action the "run" menu item                                                 */
/*-----------------------------------------------------------------------------*/
void
iqMenuRun()
{
    bool  done = false;
    int select = 0;
    int select_old = 0;
    int selectMax = 2 + (robotTotalDevices-1)/4;
    int pressedTime;

    // Clear the LCD
    eraseDisplay();

    // Draw the ROBOTC logo
    iqRunDisplayLogo();

    // Wait for LCD button release
    while( nLCDButtons != kButtonNone )
        wait1Msec(10);

    // Update LCD and check buttons
    while(!done)
        {
        // Check LCD buttons
        if( nLCDButtons != kButtonNone ) {
             pressedTime = 0;

             // Down button pushed ?
             if( nLCDButtons == kButtonDown  )
                if( select++ == selectMax ) select = 0;

            // Up button pushed ?
            if( nLCDButtons == kButtonUp  )
                if( select-- == 0 ) select = selectMax;

            // Select button pushed ?
            if( nLCDButtons == kButtonSelect  )
                robotEnabled = !robotEnabled;

            // Exit button pushed ?
            if( nLCDButtons == kButtonExit  )
                done = true;

            // Wait for LCD button release
            // If select is held then simulate exit
            while( nLCDButtons != kButtonNone ) {
                if( select == 0 && nLCDButtons == kButtonSelect && pressedTime++ == 120 ) {
                    iqRunDisplayHome( true );
                    done = true;
                }
                wait1Msec(10);
            }

            // change of screen ?
            if( select != select_old ) {
                // Clear the LCD display ready for a new status screen
                eraseDisplay();

               // Only draw the logo bitmap once when screen 0 is selected
                if( select == 0 )
                    iqRunDisplayLogo();
                }

            select_old = select;
            }

            // Update status based on which status screen has been selected
            switch( select )
                {
                case    0:
                    iqRunDisplayHome();
                    break;
                case    1:
                    iqRunDisplayJoystickStatus();
                    break;
                case    2:
                case    3:
                case    4:
                    iqRunDisplayDeviceStatus( select-2 );
                    break;
                default:
                    break;
                }

        // no need to run often
        wait1Msec(50);
        }

    // cleanup
    eraseDisplay();
}

/*-----------------------------------------------------------------------------*/
/*  Run a task that handles the main menu                                      */
/*-----------------------------------------------------------------------------*/
task
iqMenuTask()
{
    static  int select = 0;
            bool update = true;

    // This should allow using the exit button but it doesn't seem to work
    bUserControlsLCDButtons = true;

    // Run menu forever
    while( true ) {
        if( update ) {
            // Update main menu, only three items
            iqDisplayLine( LCD_ROW1, select == 0, "Run" );
            iqDisplayLine( LCD_ROW2, select == 1, "Configure" );
            iqDisplayLine( LCD_ROW3, select == 2, "Reset to Default" );
        update = false;
        }

        // Check LCD buttons
        if( nLCDButtons != kButtonNone ) {
            if( nLCDButtons == kButtonDown  )
                if( select++ == 2 ) select = 2;

            if( nLCDButtons == kButtonUp  )
                if( select-- == 0 ) select = 0;

            if( nLCDButtons == kButtonSelect  ) {
                // reset
                if( select == 2 ) iqResetToDefaults();

                // Enter configure menu
                if( select == 1 ) iqMenuConfiguration();

                // Enter run mode
                if( select == 0 ) {
                    // Gyro sensor installed ?
                    if( robotGyroSensor != PORTNONE )
                        iqCalibrateGyro();

                    // Set everything as enabled
                    robotRunning = true;
                    robotEnabled = true;

                    // Start the control task
                    startTask( iqRunRobotTask, kDefaultTaskPriority + 1 );

                    // and display information on the LCD while running...
                    iqMenuRun();

                    // When we return to this point user has indicated program should stop

                    // force task to stop motors and quit
                    robotRunning = false;
                    wait1Msec(100);
                    // task should be done
                }
            }

        // Wait for LCD button release
        while( nLCDButtons != kButtonNone )
          wait1Msec(10);

        // update display
        update = true;
        }

    // Don't hog cpu
    wait1Msec(25);
    }
}

/*-----------------------------------------------------------------------------*/
/*  Discover all the motors and sensors connected to the VexIQ                 */
/*-----------------------------------------------------------------------------*/
void
iqDiscoverDevices()
{
    TVexIQDeviceTypes   type;
    TDeviceStatus       status;
    short               ver;
    short               index;

    // Clear data
    for(index=(short)PORT1;index<=(short)PORT12;index++) {
        IQMotors[ index ].installed = false;
        IQMotors[ index ].direction = kNormal;
        IQMotors[ index ].fwdTouch  = (portName)-1;
        IQMotors[ index ].revTouch  = (portName)-1;
    }

    // We have no way to save settings in the simplified code
    // so set to user defaults, full version overrides this
    IQMotors[ (short)PORT1  ].direction = DRIVE_FWD_DEFAULT;
    IQMotors[ (short)PORT6  ].direction = DRIVE_FWD_DEFAULT;
    IQMotors[ (short)PORT4  ].direction = MOTOR_04_DEFAULT;
    IQMotors[ (short)PORT5  ].direction = MOTOR_05_DEFAULT;
    IQMotors[ (short)PORT10 ].direction = MOTOR_10_DEFAULT;
    IQMotors[ (short)PORT11 ].direction = MOTOR_11_DEFAULT;

    //
    robotTotalDevices = 0;

    // Get all device info
    for(index=(short)PORT1;index<=(short)PORT12;index++) {
        getVexIqDeviceInfo( index, type, status, ver );

        // Device found ?
        if( type != vexIQ_SensorNone)
          robotTotalDevices++;

        // Motor ?
        if( type == vexIQ_SensorMOTOR ) {
            // We expect left motors on port 1 and 7
            // We expect right motors on port 6 and 12
            // Aux motors on ports 4,5,10 & 11
            IQMotors[ index ].installed = true;
        }

        // Configure bumper switch to stop motors 4 and 10
        if( type == vexIQ_SensorBUMPER ) {
            // These seem backwards but match's VEX implementation
            if( index == (short)PORT2 )
                IQMotors[ PORT4  ].revTouch = PORT2;
            if( index == (short)PORT3 )
                IQMotors[ PORT4  ].fwdTouch = PORT3;

            if( index == (short)PORT8 )
                IQMotors[ PORT10 ].revTouch = PORT8;
            if( index == (short)PORT9 )
                IQMotors[ PORT10 ].fwdTouch = PORT9;
          }

        // Touch LED on any port used to start stop robot
        if( type == vexIQ_SensorLED )
            robotTouchSensor = (tSensors)index;

        // Color sensor on any port used to start stop robot
        if( type == vexIQ_SensorRGB ) {
            robotColorSensor = (tSensors)index;
            setColorMode( robotColorSensor, colorTypeRGB_12Colors_Ambient );
        }

        // Gyro on any port used to reorientate robot
        if( type == vexIQ_SensorGYRO )
            robotGyroSensor = (tSensors)index;

        // Sonar on any port used to stop robot
        if( type == vexIQ_SensorSONAR )
            robotSonarSensor = (tSensors)index;
    }
}

/*-----------------------------------------------------------------------------*/
/*  Main entry, start menu task and then spin until killed                     */
/*-----------------------------------------------------------------------------*/
task main()
{
    // Look for motors and sensors
    iqDiscoverDevices();

    // recover configuration
    iqSettingsRetrieve();

    // start the main menu task
    startTask( iqMenuTask );

    // nothing to do here but wait for death !
    while(1) wait1Msec(50);
}

/*-----------------------------------------------------------------------------*/
#endif // #if defined(VexIQ)
