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
 
 
 
#include "libstego/bmp.h"


/**
* Function to read a bmp-file from HDD and return a rgb_data_t
* ...
* @param *filename the path and filename of the bmp-file
* @param *rgb_data a struct to return the pixeldata
* @param *bmp_struct a struct provided by the bmp-library to hold the original image
* @return an errorcode or 0 if success
* @todo implement
*/
uint8_t io_bmp_read(const uint8_t *filename, rgb_data_t *rgb_data, void *bmp_struct)
{

}

/**
* Function to integrate a changed rgb_data_t into a bmp_struct
* ...
* @param *bmp_struct a struct provided by the bmp-library to hold the original image
* @param *rgb_data a struct to give the changed pixeldata (steganogram)
* @return an errorcode or 0 if success
* @todo implement
*/
uint8_t io_bmp_integrate(void *bmp_struct, const rgb_data_t *rgb_data)
{

}

/**
* Function to write a bmp_struct to HDD by using the bmp-library
* ...
* @param *filename the path and filename of the bmp-file
* @param *bmp_struct a struct provided by the bmp-library to hold the original image
* @return an errorcode or 0 if success
* @todo implement
*/
uint8_t io_bmp_write(const uint8_t *filename, void *bmp_struct)
{

}

/**
* Function to get information about a bmp-file
* ...
* @param *filename the path and filename of the bmp-file
* @param *bmp_info a struct to return information about the bmp-file
* @return an errorcode or 0 if success
* @todo implement
*/
uint8_t io_bmp_get_info(const uint8_t *filename, void *bmp_info)
{

}

/**
* Makes a copy from a bmp_struct provided by the bmp-library
* ...
* @param *src_struct original structure
* @param *copy target structure
* @return an errorcode or 0 if success
* @todo implement
*/
uint8_t io_bmp_copy_info(const void *src_struct, void *copy)
{

}
