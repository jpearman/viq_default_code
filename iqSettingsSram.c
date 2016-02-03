/*-----------------------------------------------------------------------------*/
/*                                                                             */
/*                        Copyright (c) James Pearman                          */
/*                                   2016                                      */
/*                            All Rights Reserved                              */
/*                                                                             */
/*-----------------------------------------------------------------------------*/
/*                                                                             */
/*    Module:     iqSettingsSram.c                                             */
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
/*    Save settings - SRAM version                                             */
/*    We use a single word in high memory.  ROBOTC VM uses memory in the range */
/*    0x20005000 - 0x20007FFF, a few words at the top are unused.              */
/*    Anything stored here is lost when the IQ is powered down                 */
/*                                                                             */
/*-----------------------------------------------------------------------------*/

#ifndef _IQ_SETTINGS_
#define _IQ_SETTINGS_

#if !defined(VexIQ)
#error "This code is for the VexIQ platform"
#else

#if !defined(_Target_Emulator_)
// Base address of SRAM area
unsigned long sramPtr = 0x20007FF0;
#else
unsigned long sramSim = 0;
#endif

/*-----------------------------------------------------------------------------*/
/*    Read one unsigned long from SRAM                                         */
/*-----------------------------------------------------------------------------*/
unsigned long
iqSettingsReadData()
{
#if !defined(_Target_Emulator_)
    unsigned long *p;
    unsigned long data;

    p = (unsigned long *)(sramPtr);
    data = *p;

    return( data );
#else
    return( sramSim );
#endif
}
/*-----------------------------------------------------------------------------*/
/*    Write one unsigned long to SRAM                                          */
/*-----------------------------------------------------------------------------*/
void
iqSettingsWriteData( unsigned long data )
{
#if !defined(_Target_Emulator_)
    unsigned long *p;

     p = (unsigned long *)(sramPtr);
    *p = data;
    //writeDebugStreamLine("save %08X", data);
#else
    sramSim = data;
#endif
}

/*-----------------------------------------------------------------------------*/
/*  No integrity check for SRAM version                                        */
/*-----------------------------------------------------------------------------*/
bool
iqSettingsIntegrityCheck()
{
    return( true );
}

/*-----------------------------------------------------------------------------*/
#endif // #if defined(VexIQ
#endif // _IQ_SETTINGS_

