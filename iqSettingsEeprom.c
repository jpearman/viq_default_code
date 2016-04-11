/*-----------------------------------------------------------------------------*/
/*                                                                             */
/*                        Copyright (c) James Pearman                          */
/*                                   2016                                      */
/*                            All Rights Reserved                              */
/*                                                                             */
/*-----------------------------------------------------------------------------*/
/*                                                                             */
/*    Module:     iqSettingsEeprom.c                                           */
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
/*    Save settings - EEPROM version                                           */
/*    VEX uses several pages of EEPROM, page 21 seems safe to use.             */
/*                                                                             */
/*    Do not modify this code !!!                                              */
/*                                                                             */
/*-----------------------------------------------------------------------------*/

#ifndef _IQ_SETTINGS_
#define _IQ_SETTINGS_

#if !defined(VexIQ)
#error "This code is for the VexIQ platform"
#else

#if !defined(_Target_Emulator_)
// Base address of EEPROM
unsigned long eepromPtr = 0x400AF000;
#else
unsigned long eepromSim = 0;
#endif

#define   EEPROM_PAGE_REG   0x04
#define   EEPROM_OFFSET_REG 0x08
#define   EEPROM_RDWR_REG   0x10
#define   EEPROM_RDWRI_REG  0x14

// The page we use, be very careful with this, don't change it !!
#define   EEPROM_PAGE       0x15

/*-----------------------------------------------------------------------------*/
/*    Initialize read or write                                                 */
/*-----------------------------------------------------------------------------*/
void
iqSettingsSetupPage()
{
#if !defined(_Target_Emulator_)
    unsigned long *p;

    p = (unsigned long *)(eepromPtr+EEPROM_PAGE_REG);
    *p = EEPROM_PAGE; // default page is 21
    p = (unsigned long *)(eepromPtr+EEPROM_OFFSET_REG);
    *p = 0;           // offset 0
#endif
}

/*-----------------------------------------------------------------------------*/
/*    Read one unsigned long from EEPROM                                       */
/*-----------------------------------------------------------------------------*/
unsigned long
iqSettingsReadData()
{
#if !defined(_Target_Emulator_)
    unsigned long *p;
    unsigned long data;

    iqSettingsSetupPage();
    p = (unsigned long *)(eepromPtr+EEPROM_RDWR_REG);
    data = *p;

    return( data );
#else
    iqSettingsSetupPage(); // just to stop warnings
    return( eepromSim );
#endif
}
/*-----------------------------------------------------------------------------*/
/*    Write one unsigned long to EEPROM                                        */
/*-----------------------------------------------------------------------------*/
void
iqSettingsWriteData( unsigned long data )
{
#if !defined(_Target_Emulator_)
    unsigned long *p;

    iqSettingsSetupPage();
    p = (unsigned long *)(eepromPtr+EEPROM_RDWR_REG);
    *p = data;
    //writeDebugStreamLine("save %08X", data);
#else
    eepromSim = data;
#endif
}

/*-----------------------------------------------------------------------------*/
/*  Simple integrity check on EEPROM page to check most locations are 0xFF     */
/*-----------------------------------------------------------------------------*/
bool
iqSettingsIntegrityCheck()
{
#if !defined(_Target_Emulator_)
    long *p;
    int  i;
    bool fail = false;

    // Do some simple checks on the EEPROM
    iqSettingsSetupPage();

    p = (unsigned long *)(eepromPtr+EEPROM_RDWRI_REG);

    // ignore first location, we may have written bad data
    unsigned long tmp = *p; tmp = tmp;

    // all other locations should be 0xFF
    // ROBOTC does not support unsigned long so we use -1
    for(i=0;i<8;i++) {
      if( *p != -1 )
        fail = true;
    }
    return( !fail );
#else
    return( true );
#endif
}

/*-----------------------------------------------------------------------------*/
#endif // #if defined(VexIQ
#endif // _IQ_SETTINGS_
