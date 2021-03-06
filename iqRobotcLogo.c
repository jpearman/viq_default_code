/*-----------------------------------------------------------------------------*/
/*                                                                             */
/*                        Copyright (c) James Pearman                          */
/*                                   2016                                      */
/*                            All Rights Reserved                              */
/*                                                                             */
/*-----------------------------------------------------------------------------*/
/*                                                                             */
/*    Module:     iqRobotcLogo.c                                               */
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
/*    header file that adds functionality to draw robotc logo on VexIQ         */
/*-----------------------------------------------------------------------------*/

#ifndef _LOGO
#define _LOGO

// stops logo array from apprearing in the global variables window
#pragma systemfile

/*-----------------------------------------------------------------------------*/

const unsigned char logo[384] = {
0xFF,0xE0,0x00,0x1F,0xE0,0x01,0xFF,0xF0,0x00,0x1F,0xE0,0x07,0xFF,0xF8,0x03,0xFA,
0xFF,0xF8,0x00,0x7F,0xF8,0x01,0xFF,0xF8,0x00,0x7F,0xF8,0x07,0xFF,0xF8,0x05,0x55,
0xFF,0xFE,0x01,0xFF,0xFC,0x01,0xFF,0xFC,0x00,0xFF,0xFE,0x07,0xFF,0xF8,0x2F,0xEE,
0xFF,0xFE,0x03,0xFF,0xFF,0x01,0xFF,0xFE,0x01,0xFF,0xFF,0x07,0xFF,0xF8,0x55,0x55,
0xFF,0xFF,0x07,0xFF,0xFF,0x01,0xFF,0xFE,0x03,0xFF,0xFF,0x87,0xFF,0xF8,0xFF,0xFF,
0xFF,0xFF,0x07,0xF8,0xFF,0x81,0xF8,0x7F,0x07,0xFC,0x7F,0xC7,0xFF,0xF8,0x55,0x55,
0xF8,0x7F,0x0F,0xE0,0x3F,0xC1,0xF8,0x3F,0x0F,0xF0,0x1F,0xC0,0x3F,0x00,0xFE,0x02,
0xF8,0x3F,0x8F,0xC0,0x1F,0xC1,0xF8,0x3F,0x0F,0xE0,0x0F,0xE0,0x3F,0x01,0x54,0x01,
0xF8,0x3F,0x9F,0xC0,0x0F,0xE1,0xF8,0x3E,0x0F,0xC0,0x0F,0xE0,0x3F,0x03,0xF8,0x00,
0xF8,0x3F,0x9F,0x80,0x0F,0xE1,0xF8,0x7C,0x1F,0xC0,0x07,0xF0,0x3F,0x01,0x50,0x00,
0xF8,0x7F,0x1F,0x80,0x07,0xE1,0xFF,0xF8,0x1F,0xC0,0x07,0xF0,0x3F,0x03,0xF8,0x00,
0xFF,0xFF,0x1F,0x80,0x07,0xE1,0xFF,0xF8,0x1F,0x80,0x07,0xF0,0x3F,0x01,0x50,0x00,
0xFF,0xFE,0x1F,0x80,0x07,0xE1,0xFF,0xFE,0x1F,0x80,0x07,0xF0,0x3F,0x03,0xF8,0x00,
0xFF,0xF8,0x1F,0x80,0x07,0xE1,0xFF,0xFF,0x1F,0xC0,0x07,0xF0,0x3F,0x01,0x50,0x00,
0xFF,0xF0,0x1F,0x80,0x0F,0xE1,0xF8,0x3F,0x1F,0xC0,0x07,0xF0,0x3F,0x02,0xF8,0x00,
0xFF,0xF0,0x1F,0xC0,0x0F,0xE1,0xF8,0x1F,0x8F,0xC0,0x0F,0xE0,0x3F,0x01,0x54,0x00,
0xFB,0xF8,0x0F,0xE0,0x1F,0xC1,0xF8,0x1F,0x8F,0xE0,0x0F,0xE0,0x3F,0x01,0xFE,0x03,
0xF9,0xFC,0x0F,0xF0,0x3F,0xC1,0xF8,0x1F,0x8F,0xF0,0x1F,0xC0,0x3F,0x01,0x55,0x05,
0xF8,0xFC,0x07,0xFF,0xFF,0x81,0xF8,0x3F,0x87,0xFC,0x7F,0xC0,0x3F,0x00,0xFF,0xEE,
0xF8,0xFE,0x07,0xFF,0xFF,0x01,0xFF,0xFF,0x03,0xFF,0xFF,0x80,0x3F,0x00,0x55,0x55,
0xF8,0x7F,0x03,0xFF,0xFE,0x01,0xFF,0xFF,0x01,0xFF,0xFF,0x00,0x3F,0x00,0x3F,0xFF,
0xF8,0x7F,0x00,0xFF,0xFC,0x01,0xFF,0xFE,0x00,0xFF,0xFE,0x00,0x3F,0x00,0x15,0x55,
0xF8,0x3F,0x80,0x7F,0xF8,0x01,0xFF,0xF8,0x00,0x3F,0xF8,0x00,0x3F,0x00,0x0E,0xFE,
0xF8,0x1F,0x80,0x0F,0xC0,0x01,0xFF,0xF0,0x00,0x0F,0xC0,0x00,0x3F,0x00,0x01,0x54
};

/*-----------------------------------------------------------------------------*/
/*  Draw the bitmap defined in the array above on the LCD                      */
/*  Just a hardcoded hack for now, will improve later                          */
/*-----------------------------------------------------------------------------*/

void
drawBitmap( const unsigned char *bitmap )
{
    int row, stripe, pix;
    int p;

    eraseRect( 0, 10, 127, 24 );

    // 24 rows
    for(row=0;row<24;row++)
        {
        // 16 bytes each with 8 pixels
        for(stripe=0;stripe<16;stripe++)
            {
            p = bitmap[ (row*16) + stripe ];
            for(pix=0;pix<8;pix++)
                {
                if( ( p & 0x80 ) )
                    setPixel( (stripe*8) + pix, (34-row) );
                p <<= 1;
                }
            }
        }
}

#endif
