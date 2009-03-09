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
 
 
 
#include "libstego/mb2.h"

/**
* Embeds a message in a jpegimage using MB2
* ... 
* @param *src_data a struct for jpegdata to provide the original image
* @param *stego_data a struct for jpegdata to return the steganogram
* @param *message returnpointer for the extracted message
* @param msglen the length of the message
* @param *para additional parameters for MB2, including passphrase
* @return an errorcode or 0 if success
* @todo implement
*/
uint8_t mb2_embed(const jpeg_data_t *src_data, jpeg_data_t *stego_data, uint8_t *message, int32_t msglen, const mb2_parameter *para)
{

}

/**
* Extracts a MB2-embedded-message from a stegonogram
* ...
* @param *stego_data a struct for jpegdata containing the stegonagram
* @param *message returnpointer for the extracted message
* @param msglen the length of the message
* @param *para additional parameters for MB2 including passphrase
* @return an errorcode or 0 if success
* @todo implement
*/
uint8_t mb2_extract(const jpeg_data_t *stego_data, uint8_t *message, int32_t msglen, const mb2_parameter *para)
{

}

/**
* Returns the length of an embedded message
* ...
* @param *stego_data a struct for jpegdata containing the stegonagram
* @param *para additional parameters for MB2 including passphrase
* @return length of embedded message
* @todo implement
*/
int32_t mb2_get_message_length(const jpeg_data_t *stego_data, const mb2_parameter *para)
{

}

/**
* Returns the capacity of a cover image
* ...
* @param *src_data a struct for jpegldata to provide the original image
* @param *para additional parameters for MB2 including passphrase
* @return capacity of cover image
* @todo implement
*/
int32_t mb2_check_capacity(const jpeg_data_t *src_data, const mb2_parameter *para)
{

}
