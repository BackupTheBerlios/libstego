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
 
 
 
#ifndef __ECHO_HIDING_H__
#define __ECHO_HIDING_H__

#include <libstego/types.h>
#include <libstego/errors.h>
#include <libstego/random.h>
#include <libstego/utils.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

uint8_t eh_embed(const wav_data_t *src_data, wav_data_t *stego_data, 
 		 uint8_t *message, uint8_t msglen, eh_parameter *para); 

uint8_t eh_extract(wav_data_t *stego_data, uint8_t **message, 
 		   uint8_t *msglen, eh_parameter *para); 

uint32_t eh_get_message_length(wav_data_t *stego_data, eh_parameter *para);

uint32_t eh_check_capacity(const wav_data_t *src_data, const eh_parameter *para, uint32_t *capacity); 

uint32_t eh_generate_echo(wav_data_t *echo, uint32_t offset, float amplitude);

uint32_t eh_generate_mix(wav_data_t *mix_signal, wav_data_t *inverse_mix_signal, 
                          uint8_t *message, uint8_t msglen, uint32_t blocksize);

uint32_t eh_mix_echoes(wav_data_t *stego_signal, wav_data_t zero_echo, 
                        wav_data_t one_echo, wav_data_t mix_signal,
                        wav_data_t inverse_mix_signal);

uint32_t eh_auto_correlate(wav_data_t *stego_data, eh_parameter *para, uint8_t *msglen, uint8_t **message);

#endif // __ECHO_HIDING_H__
