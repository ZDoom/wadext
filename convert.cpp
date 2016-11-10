// 
//---------------------------------------------------------------------------
//
// Copyright(C) 2016 Christoph Oelckers
// All rights reserved.
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see http://www.gnu.org/licenses/
//
//--------------------------------------------------------------------------
//
// Portions of this file from ZDoom under the following license:
/*
** m_png.cpp
** Routines for manipulating PNG files.
**
**---------------------------------------------------------------------------
** Copyright 2002-2006 Randy Heit
** All rights reserved.
**
** Redistribution and use in source and binary forms, with or without
** modification, are permitted provided that the following conditions
** are met:
**
** 1. Redistributions of source code must retain the above copyright
**    notice, this list of conditions and the following disclaimer.
** 2. Redistributions in binary form must reproduce the above copyright
**    notice, this list of conditions and the following disclaimer in the
**    documentation and/or other materials provided with the distribution.
** 3. The name of the author may not be used to endorse or promote products
**    derived from this software without specific prior written permission.
**
** THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR
** IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
** OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
** IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT,
** INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
** NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
** DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
** THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
** (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
** THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
**---------------------------------------------------------------------------
**
*/


#include <windows.h>
#include <direct.h>
#include <io.h>
#include <stdio.h>
#include "zlib/zlib.h"
#include "wadext.h"
#include "ResourceFile.h"
#include "fileformat.h"
#include "tarray.h"

#pragma warning(disable:4996)

const uint8_t doompal[]={
  0,  0,  0, 31, 23, 11, 23, 15,  7, 75, 75, 75,255,255,255, 27,
 27, 27, 19, 19, 19, 11, 11, 11,  7,  7,  7, 47, 55, 31, 35, 43,
 15, 23, 31,  7, 15, 23,  0, 79, 59, 43, 71, 51, 35, 63, 43, 27,
255,183,183,247,171,171,243,163,163,235,151,151,231,143,143,223,
135,135,219,123,123,211,115,115,203,107,107,199, 99, 99,191, 91,
 91,187, 87, 87,179, 79, 79,175, 71, 71,167, 63, 63,163, 59, 59,
155, 51, 51,151, 47, 47,143, 43, 43,139, 35, 35,131, 31, 31,127,
 27, 27,119, 23, 23,115, 19, 19,107, 15, 15,103, 11, 11, 95,  7,
  7, 91,  7,  7, 83,  7,  7, 79,  0,  0, 71,  0,  0, 67,  0,  0,
255,235,223,255,227,211,255,219,199,255,211,187,255,207,179,255,
199,167,255,191,155,255,187,147,255,179,131,247,171,123,239,163,
115,231,155,107,223,147, 99,215,139, 91,207,131, 83,203,127, 79,
191,123, 75,179,115, 71,171,111, 67,163,107, 63,155, 99, 59,143,
 95, 55,135, 87, 51,127, 83, 47,119, 79, 43,107, 71, 39, 95, 67,
 35, 83, 63, 31, 75, 55, 27, 63, 47, 23, 51, 43, 19, 43, 35, 15,
239,239,239,231,231,231,223,223,223,219,219,219,211,211,211,203,
203,203,199,199,199,191,191,191,183,183,183,179,179,179,171,171,
171,167,167,167,159,159,159,151,151,151,147,147,147,139,139,139,
131,131,131,127,127,127,119,119,119,111,111,111,107,107,107, 99,
 99, 99, 91, 91, 91, 87, 87, 87, 79, 79, 79, 71, 71, 71, 67, 67,
 67, 59, 59, 59, 55, 55, 55, 47, 47, 47, 39, 39, 39, 35, 35, 35,
119,255,111,111,239,103,103,223, 95, 95,207, 87, 91,191, 79, 83,
175, 71, 75,159, 63, 67,147, 55, 63,131, 47, 55,115, 43, 47, 99,
 35, 39, 83, 27, 31, 67, 23, 23, 51, 15, 19, 35, 11, 11, 23,  7,
191,167,143,183,159,135,175,151,127,167,143,119,159,135,111,155,
127,107,147,123, 99,139,115, 91,131,107, 87,123, 99, 79,119, 95,
 75,111, 87, 67,103, 83, 63, 95, 75, 55, 87, 67, 51, 83, 63, 47,
159,131, 99,143,119, 83,131,107, 75,119, 95, 63,103, 83, 51, 91,
 71, 43, 79, 59, 35, 67, 51, 27,123,127, 99,111,115, 87,103,107,
 79, 91, 99, 71, 83, 87, 59, 71, 79, 51, 63, 71, 43, 55, 63, 39,
255,255,115,235,219, 87,215,187, 67,195,155, 47,175,123, 31,155,
 91, 19,135, 67,  7,115, 43,  0,255,255,255,255,219,219,255,187,
187,255,155,155,255,123,123,255, 95, 95,255, 63, 63,255, 31, 31,
255,  0,  0,239,  0,  0,227,  0,  0,215,  0,  0,203,  0,  0,191,
  0,  0,179,  0,  0,167,  0,  0,155,  0,  0,139,  0,  0,127,  0,
  0,115,  0,  0,103,  0,  0, 91,  0,  0, 79,  0,  0, 67,  0,  0,
231,231,255,199,199,255,171,171,255,143,143,255,115,115,255, 83,
 83,255, 55, 55,255, 27, 27,255,  0,  0,255,  0,  0,227,  0,  0,
203,  0,  0,179,  0,  0,155,  0,  0,131,  0,  0,107,  0,  0, 83,
255,255,255,255,235,219,255,215,187,255,199,155,255,179,123,255,
163, 91,255,143, 59,255,127, 27,243,115, 23,235,111, 15,223,103,
 15,215, 95, 11,203, 87,  7,195, 79,  0,183, 71,  0,175, 67,  0,
255,255,255,255,255,215,255,255,179,255,255,143,255,255,107,255,
255, 71,255,255, 35,255,255,  0,167, 63,  0,159, 55,  0,147, 47,
  0,135, 35,  0, 79, 59, 39, 67, 47, 27, 55, 35, 19, 47, 27, 11,
  0,  0, 83,  0,  0, 71,  0,  0, 59,  0,  0, 47,  0,  0, 35,  0,
  0, 23,  0,  0, 11,  0,  0,  0,255,159, 67,255,231, 75,255,123,
255,255,  0,255,207,  0,207,159,  0,155,111,  0,107,167,107,107,
 };

const uint8_t hereticpal[]={
  2,  2,  2,  2,  2,  2, 16, 16, 16, 24, 24, 24, 31, 31, 31, 36,
 36, 36, 44, 44, 44, 48, 48, 48, 55, 55, 55, 63, 63, 63, 70, 70,
 70, 78, 78, 78, 86, 86, 86, 93, 93, 93,101,101,101,108,108,108,
116,116,116,124,124,124,131,131,131,139,139,139,146,146,146,154,
154,154,162,162,162,169,169,169,177,177,177,184,184,184,192,192,
192,200,200,200,207,207,207,210,210,210,215,215,215,222,222,222,
228,228,228,236,236,236,245,245,245,255,255,255, 50, 50, 50, 59,
 60, 59, 69, 72, 68, 78, 80, 77, 88, 93, 86, 97,100, 95,109,112,
104,116,123,112,125,131,121,134,141,130,144,151,139,153,161,148,
163,171,157,172,181,166,181,189,176,189,196,185, 20, 16, 36, 24,
 24, 44, 36, 36, 60, 52, 52, 80, 68, 68, 96, 88, 88,116,108,108,
136,124,124,152,148,148,172,164,164,184,180,184,200,192,196,208,
208,208,216,224,224,224, 27, 15,  8, 38, 20, 11, 49, 27, 14, 61,
 31, 14, 65, 35, 18, 74, 37, 19, 83, 43, 19, 87, 47, 23, 95, 51,
 27,103, 59, 31,115, 67, 35,123, 75, 39,131, 83, 47,143, 91, 51,
151, 99, 59,160,108, 64,175,116, 74,180,126, 81,192,135, 91,204,
143, 93,213,151,103,216,159,115,220,167,126,223,175,138,227,183,
149,230,190,161,233,198,172,237,206,184,240,214,195, 62, 40, 11,
 75, 50, 16, 84, 59, 23, 95, 67, 30,103, 75, 38,110, 83, 47,123,
 95, 55,137,107, 62,150,118, 75,163,129, 84,171,137, 92,180,146,
101,188,154,109,196,162,117,204,170,125,208,176,133, 37, 20,  4,
 47, 24,  4, 57, 28,  6, 68, 33,  4, 76, 36,  3, 84, 40,  0, 97,
 47,  2,114, 54,  0,125, 63,  6,141, 75,  9,155, 83, 17,162, 95,
 21,169,103, 26,180,113, 32,188,124, 20,204,136, 24,220,148, 28,
236,160, 23,244,172, 47,252,187, 57,252,194, 70,251,201, 83,251,
208, 97,251,214,110,251,221,123,250,228,136,157, 51,  4,170, 65,
  2,185, 86,  4,213,118,  4,236,164,  3,248,190,  3,255,216, 43,
255,255,  0, 67,  0,  0, 79,  0,  0, 91,  0,  0,103,  0,  0,115,
  0,  0,127,  0,  0,139,  0,  0,155,  0,  0,167,  0,  0,179,  0,
  0,191,  0,  0,203,  0,  0,215,  0,  0,227,  0,  0,239,  0,  0,
255,  0,  0,255, 52, 52,255, 74, 74,255, 95, 95,255,123,123,255,
155,155,255,179,179,255,201,201,255,215,215, 60, 12, 88, 80,  8,
108,104,  8,128,128,  0,144,152,  0,176,184,  0,224,216, 44,252,
224,120,240, 37,  6,129, 60, 33,147, 82, 61,165,105, 88,183,128,
116,201,151,143,219,173,171,237,196,198,255,  2,  4, 41,  2,  5,
 49,  6,  8, 57,  2,  5, 65,  2,  5, 79,  0,  4, 88,  0,  4, 96,
  0,  4,104,  2,  5,121,  2,  5,137,  6,  9,159, 12, 16,184, 32,
 40,200, 56, 60,220, 80, 80,253, 80,108,252, 80,136,252, 80,164,
252, 80,196,252, 72,220,252, 80,236,252, 84,252,252,152,252,252,
188,252,244, 11, 23,  7, 19, 35, 11, 23, 51, 15, 31, 67, 23, 39,
 83, 27, 47, 99, 35, 55,115, 43, 63,131, 47, 67,147, 55, 75,159,
 63, 83,175, 71, 91,191, 79, 95,207, 87,103,223, 95,111,239,103,
119,255,111, 23, 31, 23, 27, 35, 27, 31, 43, 31, 35, 51, 35, 43,
 55, 43, 47, 63, 47, 51, 71, 51, 59, 75, 55, 63, 83, 59, 67, 91,
 67, 75, 95, 71, 79,103, 75, 87,111, 79, 91,115, 83, 95,123, 87,
103,131, 95,255,223,  0,255,191,  0,255,159,  0,255,127,  0,255,
 95,  0,255, 63,  0,244, 14,  3, 55,  0,  0, 47,  0,  0, 39,  0,
  0, 23,  0,  0, 15, 15, 15, 11, 11, 11,  7,  7,  7,255,255,255,
};



const uint8_t hexenpal[]={
  2,  2,  2,  4,  4,  4, 15, 15, 15, 19, 19, 19, 27, 27, 27, 28,
 28, 28, 33, 33, 33, 39, 39, 39, 45, 45, 45, 51, 51, 51, 57, 57,
 57, 63, 63, 63, 69, 69, 69, 75, 75, 75, 81, 81, 81, 86, 86, 86,
 92, 92, 92, 98, 98, 98,104,104,104,112,112,112,121,121,121,130,
130,130,139,139,139,147,147,147,157,157,157,166,166,166,176,176,
176,185,185,185,194,194,194,203,203,203,212,212,212,221,221,221,
230,230,230, 29, 32, 29, 38, 40, 37, 50, 50, 50, 59, 60, 59, 69,
 72, 68, 78, 80, 77, 88, 93, 86, 97,100, 95,109,112,104,116,123,
112,125,131,121,134,141,130,144,151,139,153,161,148,163,171,157,
172,181,166,181,189,176,189,196,185, 22, 29, 22, 27, 36, 27, 31,
 43, 31, 35, 51, 35, 43, 55, 43, 47, 63, 47, 51, 71, 51, 59, 75,
 55, 63, 83, 59, 67, 91, 67, 75, 95, 71, 79,103, 75, 87,111, 79,
 91,115, 83, 95,123, 87,103,131, 95, 20, 16, 36, 30, 26, 46, 40,
 36, 57, 50, 46, 67, 59, 57, 78, 69, 67, 88, 79, 77, 99, 89, 87,
109, 99, 97,120,109,107,130,118,118,141,128,128,151,138,138,162,
148,148,172, 62, 40, 11, 75, 50, 16, 84, 59, 23, 95, 67, 30,103,
 75, 38,110, 83, 47,123, 95, 55,137,107, 62,150,118, 75,163,129,
 84,171,137, 92,180,146,101,188,154,109,196,162,117,204,170,125,
208,176,133, 27, 15,  8, 38, 20, 11, 49, 27, 14, 61, 31, 14, 65,
 35, 18, 74, 37, 19, 83, 43, 19, 87, 47, 23, 95, 51, 27,103, 59,
 31,115, 67, 35,123, 75, 39,131, 83, 47,143, 91, 51,151, 99, 59,
160,108, 64,175,116, 74,180,126, 81,192,135, 91,204,143, 93,213,
151,103,216,159,115,220,167,126,223,175,138,227,183,149, 37, 20,
  4, 47, 24,  4, 57, 28,  6, 68, 33,  4, 76, 36,  3, 84, 40,  0,
 97, 47,  2,114, 54,  0,125, 63,  6,141, 75,  9,155, 83, 17,162,
 95, 21,169,103, 26,180,113, 32,188,124, 20,204,136, 24,220,148,
 28,236,160, 23,244,172, 47,252,187, 57,252,194, 70,251,201, 83,
251,208, 97,251,221,123,  2,  4, 41,  2,  5, 49,  6,  8, 57,  2,
  5, 65,  2,  5, 79,  0,  4, 88,  0,  4, 96,  0,  4,104,  4,  6,
121,  2,  5,137, 20, 23,152, 38, 41,167, 56, 59,181, 74, 77,196,
 91, 94,211,109,112,226,127,130,240,145,148,255, 31,  4,  4, 39,
  0,  0, 47,  0,  0, 55,  0,  0, 67,  0,  0, 79,  0,  0, 91,  0,
  0,103,  0,  0,115,  0,  0,127,  0,  0,139,  0,  0,155,  0,  0,
167,  0,  0,185,  0,  0,202,  0,  0,220,  0,  0,237,  0,  0,255,
  0,  0,255, 46, 46,255, 91, 91,255,137,137,255,171,171, 20, 16,
  4, 13, 24,  9, 17, 33, 12, 21, 41, 14, 24, 50, 17, 28, 57, 20,
 32, 65, 24, 35, 73, 28, 39, 80, 31, 44, 86, 37, 46, 95, 38, 51,
104, 43, 60,122, 51, 68,139, 58, 77,157, 66, 85,174, 73, 94,192,
 81,157, 51,  4,170, 65,  2,185, 86,  4,213,119,  6,234,147,  5,
255,178,  6,255,195, 26,255,216, 45,  4,133,  4,  8,175,  8,  2,
215,  2,  3,234,  3, 42,252, 42,121,255,121,  3,  3,184, 15, 41,
220, 28, 80,226, 41,119,233, 54,158,239, 67,197,246, 80,236,252,
244, 14,  3,255, 63,  0,255, 95,  0,255,127,  0,255,159,  0,255,
195, 26,255,223,  0, 43, 13, 64, 61, 14, 89, 90, 15,122,120, 16,
156,149, 16,189,178, 17,222,197, 74,232,215,129,243,234,169,253,
 61, 16, 16, 90, 36, 33,118, 56, 49,147, 77, 66,176, 97, 83,204,
117, 99, 71, 53,  2, 81, 63,  6, 96, 72,  0,108, 80,  0,120, 88,
  0,128, 96,  0,149,112,  1,181,136,  3,212,160,  4,255,255,255,
};

const uint8_t strifepal[]={
  0,  0,  0,231,227,227,223,219,219,215,211,211,207,203,203,199,
195,195,191,191,191,183,183,183,179,175,175,171,167,167,163,159,
159,155,151,151,147,147,147,139,139,139,131,131,131,123,123,123,
119,115,115,111,111,111,103,103,103, 95, 95, 95, 87, 87, 87, 79,
 79, 79, 71, 71, 71, 67, 63, 63, 59, 59, 59, 51, 51, 51, 43, 43,
 43, 35, 35, 35, 27, 27, 27, 19, 19, 19, 11, 11, 11,  7,  7,  7,
187,191,183,179,183,171,167,179,159,163,171,147,155,167,139,147,
159,127,139,155,119,131,147,107,127,143,103,119,135, 91,115,131,
 83,107,123, 75,103,119, 67, 99,111, 63, 91,107, 55, 87, 99, 47,
 83, 95, 43, 75, 87, 35, 71, 83, 31, 67, 75, 27, 63, 71, 23, 59,
 63, 19, 51, 59, 15, 47, 51, 11, 43, 47,  7, 39, 43,  7, 31, 35,
  7, 27, 31,  0, 23, 23,  0, 15, 19,  0, 11, 11,  0,  7,  7,  0,
219, 43, 43,203, 35, 35,191, 31, 31,175, 27, 27,163, 23, 23,147,
 19, 19,135, 15, 15,119, 11, 11,107,  7,  7, 91,  7,  7, 79,  0,
  0, 63,  0,  0, 51,  0,  0, 39,  0,  0, 23,  0,  0, 11,  0,  0,
235,231,  0,231,211,  0,215,179,  0,199,151,  0,183,127,  0,167,
103,  0,151, 83,  0,135, 63,  0,119, 47,  0,103, 35,  0, 87, 23,
  0, 71, 11,  0, 55,  7,  0, 39,  0,  0, 23,  0,  0, 11,  0,  0,
183,231,127,163,215,111,143,199, 95,127,183, 79,107,171, 67, 91,
155, 55, 75,139, 43, 63,123, 35, 47,111, 27, 35, 95, 19, 23, 79,
 11, 15, 67,  7,  7, 51,  7,  0, 35,  0,  0, 19,  0,  0,  7,  0,
199,207,255,183,187,239,163,171,219,151,155,203,135,139,187,123,
127,171,107,111,155, 95, 99,139, 83, 83,123, 67, 71,107, 55, 59,
 91, 47, 47, 75, 35, 35, 59, 23, 23, 43, 15, 15, 27,  0,  0, 11,
199,191,147,179,171,131,167,155,119,155,139,111,143,127, 99,131,
111, 91,119, 99, 79,107, 87, 71, 91, 71, 59, 79, 59, 51, 67, 47,
 43, 55, 39, 35, 43, 27, 27, 31, 19, 19, 19, 11, 11,  7,  7,  0,
143,195,211,123,179,195,107,167,183, 91,155,167, 75,139,155, 59,
127,139, 47,115,127, 35,103,115, 27, 91, 99, 19, 79, 87, 11, 67,
 71,  7, 55, 59,  0, 43, 43,  0, 31, 31,  0, 19, 19,  0,  7,  7,
211,191,175,203,179,163,195,171,151,191,159,143,183,151,131,175,
143,123,171,135,115,163,123,103,155,115, 95,151,107, 87,143, 99,
 79,139, 91, 71,131, 83, 67,123, 75, 59,119, 67, 51,111, 59, 47,
103, 55, 39, 99, 47, 35, 91, 43, 31, 83, 35, 27, 79, 31, 23, 71,
 27, 19, 63, 19, 15, 59, 15, 11, 51, 11,  7, 43,  7,  7, 39,  7,
  0, 31,  0,  0, 27,  0,  0, 19,  0,  0, 11,  0,  0,  7,  0,  0,
211,199,187,203,191,179,195,183,171,191,175,163,183,167,155,175,
159,147,171,151,139,163,143,135,155,139,127,151,131,119,143,123,
111,135,115,107,131,107, 99,123,103, 95,115, 95, 87,111, 87, 83,
103, 83, 75, 95, 75, 71, 91, 67, 63, 83, 63, 59, 79, 55, 51, 71,
 51, 47, 63, 43, 43, 59, 39, 39, 51, 35, 31, 43, 27, 27, 39, 23,
 23, 31, 19, 19, 23, 15, 15, 19, 11, 11, 11,  7,  7,  7,  7,  0,
239,239,  0,231,215,  0,227,191,  0,219,171,  0,215,151,  0,211,
131,  0,203,111,  0,199, 91,  0,191, 75,  0,187, 59,  0,183, 43,
  0,255,  0,  0,223,  0,  0,191,  0,  0,159,  0,  0,127,  0,  0,
  0, 47, 47,139,199,103,107,171, 75, 79,143, 55, 55,115, 35, 35,
 87, 19, 19, 63, 11,215,223,255,187,203,247,143,167,219, 99,131,
195, 63, 91,167,203,203,203,215,215,215,223,223,223,235,235,235,
};

#pragma pack(1)
struct IHDR
{
	uint32_t		Width;
	uint32_t		Height;
	uint8_t		BitDepth;
	uint8_t		ColorType;
	uint8_t		Compression;
	uint8_t		Filter;
	uint8_t		Interlace;
};
#pragma pack()


uint8_t palette[768];
const uint8_t *ppal = NULL;
bool done;

const uint8_t *GetPalette(int flags)
{
	if (ppal) return ppal;
	int pplump = mainwad->FindLump("PLAYPAL");
	if (pplump >= 0)
	{
		WadItemList w(pplump);
		memcpy(palette, w.Address(), 768);
		ppal = palette;
	}
	else
	{
		if (flags&DO_HERETIC_PAL) ppal = hereticpal;
		else if (flags&DO_HEXEN_PAL) ppal = hexenpal;
		else if (flags&DO_STRIFE) ppal = strifepal;
		else ppal = doompal;
	}
	return ppal;
}

inline uint32_t BigLong(uint32_t nLongNumber)
{
	return (((nLongNumber & 0x000000FF) << 24) + ((nLongNumber & 0x0000FF00) << 8) +
		((nLongNumber & 0x00FF0000) >> 8) + ((nLongNumber & 0xFF000000) >> 24));
}

inline const uint32_t *GetCRCTable() { return (const uint32_t *)get_crc_table(); }
inline uint32_t CalcCRC32(const uint8_t *buf, unsigned int len)
{
	return crc32(0, buf, len);
}
inline uint32_t AddCRC32(uint32_t crc, const uint8_t *buf, unsigned int len)
{
	return crc32(crc, buf, len);
}
inline uint32_t CRC1(uint32_t crc, const uint8_t c, const uint32_t *crcTable)
{
	return crcTable[(crc & 0xff) ^ c] ^ (crc >> 8);
}


//==========================================================================
//
// M_AppendPNGChunk
//
// Writes a PNG-compliant chunk to the file.
//
//==========================================================================

bool M_AppendPNGChunk(FILE *file, uint32_t chunkID, const void *_chunkData, uint32_t len)
{
	const uint8_t *chunkData = (uint8_t*)_chunkData;
	uint32_t head[2] = { BigLong((unsigned int)len), chunkID };
	uint32_t crc;

	if (fwrite(head, 1, 8, file) == 8 &&
		(len == 0 || fwrite(chunkData, 1, len, file) == len))
	{
		crc = CalcCRC32((uint8_t *)&head[1], 4);
		if (len != 0)
		{
			crc = AddCRC32(crc, chunkData, len);
		}
		crc = BigLong((unsigned int)crc);
		return fwrite(&crc, 1, 4, file) == 4;
	}
	return false;
}


//==========================================================================
//
// M_SaveBitmap
//
// Given a bitmap, creates one or more IDAT chunks in the given file.
// Returns true on success.
//
//==========================================================================
#define PNG_WRITE_SIZE	32768
#define MAXWIDTH 8192

bool M_SaveBitmap(const uint8_t *from, int width, int height, int pitch, FILE *file)
{
	uint8_t temprow[1][1 + MAXWIDTH * 3];
	uint8_t buffer[PNG_WRITE_SIZE];
	z_stream stream;
	int err;
	int y;

	stream.next_in = Z_NULL;
	stream.avail_in = 0;
	stream.zalloc = Z_NULL;
	stream.zfree = Z_NULL;
	err = deflateInit(&stream, 9);

	if (err != Z_OK)
	{
		return false;
	}

	y = height;
	stream.next_out = buffer;
	stream.avail_out = sizeof(buffer);

	temprow[0][0] = 0;

	while (y-- > 0 && err == Z_OK)
	{
		memcpy(&temprow[0][1], from, width);
		// always use filter type 0 for paletted images
		stream.next_in = temprow[0];
		stream.avail_in = width + 1;

		from += pitch;

		err = deflate(&stream, (y == 0) ? Z_FINISH : 0);
		if (err != Z_OK)
		{
			break;
		}
		while (stream.avail_out == 0)
		{
			if (!M_AppendPNGChunk(file, MAKE_ID('I', 'D', 'A', 'T'), buffer, sizeof(buffer)))
			{
				return false;
			}
			stream.next_out = buffer;
			stream.avail_out = sizeof(buffer);
			if (stream.avail_in != 0)
			{
				err = deflate(&stream, (y == 0) ? Z_FINISH : 0);
				if (err != Z_OK)
				{
					break;
				}
			}
		}
	}

	while (err == Z_OK)
	{
		err = deflate(&stream, Z_FINISH);
		if (err != Z_OK)
		{
			break;
		}
		if (stream.avail_out == 0)
		{
			if (!M_AppendPNGChunk(file, MAKE_ID('I','D','A','T'), buffer, sizeof(buffer)))
			{
				return false;
			}
			stream.next_out = buffer;
			stream.avail_out = sizeof(buffer);
		}
	}

	deflateEnd(&stream);

	if (err != Z_STREAM_END)
	{
		return false;
	}
	return M_AppendPNGChunk(file, MAKE_ID('I', 'D', 'A', 'T'), buffer, sizeof(buffer) - stream.avail_out);
}


//==========================================================================
//
//
//
//==========================================================================

void WritePNG(const uint8_t *bufferRGBA, int width, int height, int leftofs, int topofs, const char *filename)
{
	int numpix = width * height;
	bool hastrans = false;
	TMap<uint32_t, uint8_t> palmap;
	uint8_t *bufferPaletted = new uint8_t[numpix];
	uint8_t palette[768];
	for (int i = 0; i < numpix; i++)
	{
		if (bufferRGBA[i * 4 + 3] == 0)
		{
			hastrans = true;
			break;
		}
	}
	int curcol = 0;
	if (hastrans)
	{
		palmap[0] = 0;
		palette[0] = palette[1] = palette[2] = 0;
		curcol++;
	}
	for (int i = 0; i < numpix; i++)
	{
		const uint8_t *pix = &bufferRGBA[i * 4];
		uint32_t rgba = pix[0] + 256 * pix[1] + 256 * 256 * pix[2] + 256 * 256 * 256 * pix[3];
		uint8_t *pIndex = palmap.CheckKey(rgba);
		uint8_t index;
		if (pIndex != NULL) index = *pIndex;
		else
		{
			palmap[rgba] = curcol;
			index = curcol;
			palette[curcol * 3] = pix[0];
			palette[curcol * 3+1] = pix[1];
			palette[curcol * 3+2] = pix[2];
			curcol++;
		}
		bufferPaletted[i] = index;
	}

	FILE * f = fopen(filename, "wb");
	if (f == NULL) return;

	const char sig[] = "\x89PNG\r\n\x1a\n";
	IHDR ihdr;

	ihdr.Width = BigLong(width);
	ihdr.Height = BigLong(height);
	ihdr.BitDepth = 8;
	ihdr.ColorType = 3;
	ihdr.Compression = 0;
	ihdr.Filter = 0;
	ihdr.Interlace = 0;

	fwrite(sig, 1, 8, f);
	M_AppendPNGChunk(f, MAKE_ID('I', 'H', 'D', 'R'), &ihdr, sizeof(ihdr));
	if (leftofs != 0 || topofs != 0)
	{
		uint32_t ofs[] = { BigLong(leftofs), BigLong(topofs) };
		M_AppendPNGChunk(f, MAKE_ID('g', 'r', 'A', 'b'), ofs, 8);
	}
	if (curcol > 0)
	{
		M_AppendPNGChunk(f, MAKE_ID('P', 'L', 'T', 'E'), palette, curcol * 3);
	}
	if (hastrans)
	{
		char nul = 0;
		M_AppendPNGChunk(f, MAKE_ID('t', 'R', 'N', 'S'), &nul, 1);
	}

	M_SaveBitmap(bufferPaletted, width, height, width, f);

	static const uint8_t iend[12] = { 0,0,0,0,73,69,78,68,174,66,96,130 };
	fwrite(iend, 1, 12, f);
	fclose(f);
}


bool FlatToPng(int options, const uint8_t *data, int length, const char *pngpath)
{
	int width, height;

	// This uses the same logic as ZDoom, except for the 320x200 case which is for Raven's fullscreen images
	switch (length)
	{
	default:
	case 64 * 64: width = height = 64;	break;
	case 8 * 8: width = height = 8;	break;
	case 16 * 16: width = height = 16;	break;
	case 32 * 32: width = height = 32;	break;
	case 128 * 128:	width = height = 128;	break;
	case 256 * 256:	width = height = 256;	break;
	case 64000: width = 320; height = 200; break;
	}

	uint8_t *image = new uint8_t[width * height * 4];

	const unsigned char *bytes = (const unsigned char *)data;
	const uint8_t *palette = GetPalette(options);

	for (int y = 0; y<height; y++) for (int x = 0; x<width; x++)
	{
		const unsigned char data = bytes[x + y*width];
		unsigned char *ddata = &image[(x + y*width) * 4];

		ddata[0] = palette[data * 3 + 0];
		ddata[1] = palette[data * 3 + 1];
		ddata[2] = palette[data * 3 + 2];
		ddata[3] = 255;
	}

	WritePNG(image, width, height, 0, 0, pngpath);
	delete[]image;
	return true;
}



/****************************
PatchToPng
*****************************/


bool IsHack(const uint8_t *data, int length)
{
	// Check if this patch is likely to be a problem.
	// It must be 256 pixels tall, and all its columns must have exactly
	// one post, where each post has a supposed length of 0.
	const patch_t *realpatch = (const patch_t *)data;
	const uint32_t *cofs = realpatch->columnofs;
	int x, x2 = realpatch->width;

	if (realpatch->height == 256)
	{
		for (x = 0; x < x2; ++x)
		{
			const column_t *col = (column_t*)((BYTE*)realpatch + cofs[x]);
			if (col->topdelta != 0 || col->length != 0)
			{
				return false;	// It's not bad!
			}
			col = (column_t *)((BYTE *)col + 256 + 4);
			if (col->topdelta != 0xFF)
			{
				return false;	// More than one post in a column!
			}
		}
		if (x == x2)
		{
			return true;
		}
	}
	return false;
}

bool PatchToPng(int options, const uint8_t *ldata, int length, const char *pngpath)
{
	const unsigned char *palette = GetPalette(options);

	const column_t *maxcol;
	int x;

	const patch_t *patch = (const patch_t *)ldata;

	int height = patch->height;
	int width = patch->width;
	uint8_t *image = new uint8_t[width * height * 4];
	memset(image, 0, width*height * 4);

	maxcol = (const column_t *)((const BYTE *)patch + length - 3);

	// detect broken patches
	if (IsHack(ldata, length))
	{
		// Draw the image to the buffer
		for (x = 0; x < width; ++x)
		{
			const BYTE *in = (const BYTE *)patch + patch->columnofs[x] + 3;

			for (int y = height; y > 0; --y)
			{
				unsigned char *ddata = &image[(x + y*width) * 4];
				ddata[0] = palette[*in * 3 + 0];
				ddata[1] = palette[*in * 3 + 1];
				ddata[2] = palette[*in * 3 + 2];
				ddata[3] = 255;
				in++;
			}
		}
	}
	else
	{

		// Draw the image to the buffer
		for (x = 0; x < width; ++x)
		{
			const column_t *column = (const column_t *)((const BYTE *)patch + patch->columnofs[x]);
			int top = -1;

			while (column < maxcol && column->topdelta != 0xFF)
			{
				if (column->topdelta <= top)
				{
					top += column->topdelta;
				}
				else
				{
					top = column->topdelta;
				}

				int len = column->length;

				if (len != 0)
				{
					if (top + len > height)	// Clip posts that extend past the bottom
					{
						len = height - top;
					}
					if (len > 0)
					{
						const BYTE *in = (const BYTE *)column + 3;
						for (int i = 0; i < len; ++i)
						{
							unsigned char *ddata = &image[(x + (top+i)*width) * 4];
							ddata[0] = palette[in[i] * 3 + 0];
							ddata[1] = palette[in[i] * 3 + 1];
							ddata[2] = palette[in[i] * 3 + 2];
							ddata[3] = 255;
						}
					}
				}
				column = (const column_t *)((const BYTE *)column + column->length + 4);
			}
		}
	}

	WritePNG(image, width, height, patch->leftoffset, patch->topoffset, pngpath);
	delete[]image;
	return true;
}


struct DoomSoundHead
{
	unsigned short three;
	unsigned short samplerate;
	unsigned short samples;
	unsigned short zero;
};

struct WavSoundHead
{
	char riff[4];
	unsigned size;
	char wave[4];
	char fmt[4];
	unsigned x10;
	unsigned short x01;
	unsigned short channels;
	unsigned rate;
	unsigned bytesPerSecond;
	unsigned short bytesPerSample;
	unsigned short bitsPerSample;
	char data[4];
	unsigned length;

	WavSoundHead()
	{
		riff[0] = 'R'; riff[1] = 'I'; riff[2] = 'F'; riff[3] = 'F';
		wave[0] = 'W'; wave[1] = 'A'; wave[2] = 'V'; wave[3] = 'E';
		fmt[0] = 'f'; fmt[1] = 'm'; fmt[2] = 't'; fmt[3] = ' ';
		x10 = 0x10; // length of format chunk
		x01 = 0x01;
		channels = 1;
		bytesPerSample = 1; // 8 bit mono
		bitsPerSample = 8; // 8 bit mono
		data[0] = 'd'; data[1] = 'a'; data[2] = 't'; data[3] = 'a';
	}

};

bool DoomSndToWav(const uint8_t *data, int length, const char *filename)
{

	DoomSoundHead *head = (DoomSoundHead *)data;
	uint32_t headSize = sizeof(DoomSoundHead);
	uint32_t dataSize = length - headSize;

	WavSoundHead wh;
	wh.length = dataSize;
	wh.size = dataSize + 36;
	wh.rate = wh.bytesPerSecond = head->samplerate;

	FILE *f = fopen(filename, "wb");
	if (f != NULL)
	{
		fwrite(&wh, 1, sizeof(WavSoundHead), f);
		fwrite(data + headSize, 1, dataSize, f);
		fclose(f);
		return true;
	}
	return false;
};
