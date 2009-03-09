/*
 *  This file is part of libstego.
 *
 *  libstego is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  libstego is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with libstego.  If not, see <http://www.gnu.org/licenses/>.
 */

/*
 *  Copyright 2008 2009 by Jan C. Busch, René Frerichs, Lars Herrmann,
 *  Matthias Kloppenborg, Marko Krause, Christian Kuka, Sebastian Schnell,
 *  Ralf Treu.
 *
 *  For more info visit <http://parsys.informatik.uni-oldenburg.de/~stego/>
 */
 
 
 
#ifndef __GIFSHUFFLE_H__
#define __GIFSHUFFLE_H__

#include "libstego/types.h"
#include "libstego/errors.h"
#include "libstego/utils.h"
#include "libstego/gif_helpers.h"
#include "libstego/epi.h"
#include <stdio.h>
#include <stdlib.h>

const int32_t capacity_lookup[256] = {0,1,2,4,6,9,12,15,18,21,25,28,32,36,40,44,48,52,56,61,65,69,74,79,83,88,93,97,102,107,112,117,122,127,132,138,143,148,153,159,164,169,175,180,186,191,197,202,208,214,219,225,231,237,242,248,254,260,266,272,278,284,289,295,302,308,314,320,326,332,338,344,350,357,363,369,375,382,388,394,401,407,413,420,426,433,439,446,452,458,465,472,478,485,491,498,504,511,518,524,531,538,544,551,558,564,571,578,585,591,598,605,612,619,626,632,639,646,653,660,667,674,681,688,695,702,709,716,723,730,737,744,751,758,765,772,779,786,793,801,808,815,822,829,836,844,851,858,865,872,880,887,894,901,909,916,923,931,938,945,952,960,967,975,982,989,997,1004,1011,1019,1026,1034,1041,1049,1056,1064,1071,1078,1086,1093,1101,1108,1116,1123,1131,1139,1146,1154,1161,1169,1176,1184,1192,1199,1207,1214,1222,1230,1237,1245,1253,1260,1268,1276,1283,1291,1299,1306,1314,1322,1329,1337,1345,1353,1360,1368,1376,1384,1391,1399,1407,1415,1423,1430,1438,1446,1454,1462,1470,1477,1485,1493,1501,1509,1517,1525,1532,1540,1548,1556,1564,1572,1580,1588,1596,1604,1612,1620,1628,1636,1644,1652,1660,1668,1675,1683};

uint32_t gifshuffle_embed(const palette_data_t *src_data, palette_data_t *stego_data, uint8_t *message, int32_t msglen, const gifshuffle_parameter *para);

uint32_t gifshuffle_extract(const palette_data_t *stego_data, uint8_t **message, uint32_t *msglen, const gifshuffle_parameter *para);

int32_t gifshuffle_check_capacity(const palette_data_t *src_data,uint32_t* capacity, const gifshuffle_parameter *para);

#endif // __GIFSHUFFLE_H__
