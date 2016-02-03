/*-----------------------------------------------------------------------------*/
/*                                                                             */
/*                        Copyright (c) James Pearman                          */
/*                                   2016                                      */
/*                            All Rights Reserved                              */
/*                                                                             */
/*-----------------------------------------------------------------------------*/
/*                                                                             */
/*    Module:     iqDefaultCodeSimple.c                                        */
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
/*    Simple version, no sensors, motors only.                                 */
/*                                                                             */
/*    PORT1  Left Drive Motor (optional)                                       */
/*    PORT4  Auxilary motor (BtnRUp & BtnRDown)                                */
/*    PORT5  Auxilary motor (BtnFUp & BtnFDown)                                */
/*    PORT6  Right Drive Motor (optional)                                      */
/*                                                                             */
/*    PORT7  Left Drive Motor (optional)                                       */
/*    PORT10 Auxilary motor (BtnLUp & BtnLDown)                                */
/*    PORT11 Auxilary motor (BtnEUp & BtnEDown)                                */
/*    PORT12 Right Drive Motor (optional)                                      */
/*                                                                             */
/*-----------------------------------------------------------------------------*/

#if !defined(VexIQ)
#error "This code is for the VexIQ platform"
#else

// Optional include of a logo drawing function
#include "iqRobotcLogo.c"

// Custom types and definitions
#include "iqDefinitions.h"

// Globals used by the code
tJoystickMode   configDriveMode  = DRIVE_MODE_DEFAULT;
tMotorDirection configDriveTurn  = DRIVE_TURN_DEFAULT;
bool            robotRunning     = false;
bool            robotEnabled     = true;
IQMotor         IQMotors[ kNumbOfRealMotors ];

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

        // done
        if( !robotRunning )
          break;

        // Don't hog the cpu
        wait1Msec(25);
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
                if( configSelect++ == kConfig_6 )
                    configSelect = kConfig_6;
                if( configSelect > kConfig_3 && offset == 0 )
                    offset = 3;
            }

            if( nLCDButtons == kButtonUp  ) {
                if( configSelect-- == kConfig_exit )
                    configSelect = kConfig_exit;
                if( configSelect < kConfig_2 && offset == 3 )
                    offset = 0;
            }

            if( nLCDButtons == kButtonSelect  ) {
                if( iqMenuConfigurationSelect( configSelect ) )
                    done = true;
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
}

/*-----------------------------------------------------------------------------*/
/*  Action the "run" menu item                                                 */
/*-----------------------------------------------------------------------------*/
void
iqMenuRun()
{
    bool  done = false;
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

            // Select button pushed ?
            if( nLCDButtons == kButtonSelect  )
                robotEnabled = !robotEnabled;

            // Exit button pushed ?
            if( nLCDButtons == kButtonExit  )
                done = true;

            // Wait for LCD button release
            // If select is held then simulate exit
            while( nLCDButtons != kButtonNone ) {
                if( nLCDButtons == kButtonSelect && pressedTime++ == 120 ) {
                    iqRunDisplayHome( true );
                    done = true;
                }
                wait1Msec(10);
            }
        }

        // Update status
        iqRunDisplayHome();

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
    }

    // We have no way to save settings in the simplified code
    // so set to user defaults, full version overrides this
    IQMotors[ (short)PORT1  ].direction = DRIVE_FWD_DEFAULT;
    IQMotors[ (short)PORT6  ].direction = DRIVE_FWD_DEFAULT;
    IQMotors[ (short)PORT4  ].direction = MOTOR_04_DEFAULT;
    IQMotors[ (short)PORT5  ].direction = MOTOR_05_DEFAULT;
    IQMotors[ (short)PORT10 ].direction = MOTOR_10_DEFAULT;
    IQMotors[ (short)PORT11 ].direction = MOTOR_11_DEFAULT;

    // Get all device info
    for(index=(short)PORT1;index<=(short)PORT12;index++) {
        getVexIqDeviceInfo( index, type, status, ver );
        if( type == vexIQ_SensorMOTOR ) {
            // We expect left motors on port 1 and 7
            // We expect right motors on port 6 and 12
            // Aux motors on ports 4,5,10 & 11
            IQMotors[ index ].installed = true;
        }
    }
}

/*-----------------------------------------------------------------------------*/
/*  Main entry, start menu task and then spin until killed                     */
/*-----------------------------------------------------------------------------*/
task main()
{
    // Look for motors and sensors
    iqDiscoverDevices();

    // start the main menu task
    startTask( iqMenuTask );

    // nothing to do here but wait for death !
    while(1) wait1Msec(50);
}

/*-----------------------------------------------------------------------------*/
#endif // #if defined(VexIQ)
