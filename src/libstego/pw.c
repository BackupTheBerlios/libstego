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
 
 
 
#include "libstego/pw.h"
#include <stdlib.h>
#include <math.h>

uint32_t* pw_create_histogram(const wav_data_t *src_data);

/**
* Embeds a message in a wav-file using PW
* ... 
* @param *src_data a struct for wavdata to provide the original audio-source
* @param *stego_data a struct for wavdata to return the steganogram
* @param *message pointer to the message
* @param msglen the length of the message
* @param *para additional parameters for PW, including passphrase
* @return an errorcode or 0 if success
* @todo implement
*/
uint8_t pw_embed(const wav_data_t *src_data, wav_data_t *stego_data, uint8_t *message, int32_t msglen,  const pw_parameter *para)
{

}

/**
* Extracts a PW-embedded-message from a steganogram
* ...
* @param *stego_data a struct for wavdata containing the steganogram
* @param *message pointer for the extracted message
* @param msglen the length of the message
* @param *para additional parameters for PW including passphrase
* @return an errorcode or 0 if success
* @todo implement
*/
uint8_t pw_extract(const wav_data_t *stego_data, uint8_t *message, int32_t msglen, const pw_parameter *para)
{

}

/**
* Returns the length of an embedded message
* ...
* @param *stego_data a struct for wavdata containing the steganogram
* @param *para additional parameters for PW including passphrase
* @return length of embedded message
* @todo implement
*/
int32_t pw_get_message_length(const wav_data_t *stego_data, const pw_parameter *para)
{

}

/**
* Returns the capacity of a cover wav
* ...
* @param *src_data a  wavdata-struct to the audio data
* @param *para additional parameters for PW including passphrase
* @return capacity of cover image
* @todo make it work
*/
int32_t pw_check_capacity(const wav_data_t *src_data, const pw_parameter *para)
{
    uint32_t* histogram = pw_create_histogram(src_data);
    int32_t capacity = 0;
    for(uint32_t ampl = 0; ampl < 0x10000;ampl++)
    {
	if(ampl * para->gain - floor(ampl * para->gain) < para->epsilon)
	capacity += histogram[ampl];
    }
    return capacity;
}

/**
* Returns the histogram of a wav
* ...
* @param *src_data a  wavdata-struct to the audio data
* @return 16-bit-histogram
*/

uint32_t* pw_create_histogram(const wav_data_t *src_data)
{
    //allocate memory for the histogram and initialize it
    uint32_t* hist = (uint32_t*) malloc(sizeof(uint32_t) * 0x10000);
    for(int i = 0;i < 0x10000;i++)
    { 
      hist[i] = 0;
    }

    //increment histogram-entry for each corresponding sample
    for(int chan = 0;chan < src_data->num_channels;chan++)
    {
      uint16_t* samples = (uint16_t*) src_data->data[chan];
      for(int s = 0;s < src_data->size;s++)
      {
        hist[samples[s]]++;
      }
    }
}
