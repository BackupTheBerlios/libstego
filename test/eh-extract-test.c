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
 
 
 
#include <libstego/wav.h>
#include <stdio.h>
#include <libstego/errors.h>
#include <libstego/libstego.h>

int main(int argc, char *argv[]) {
    wav_data_t wav_data;
    wav_internal_data_t wav_struct;
    wav_struct.data = NULL;
    int ret = 0;
    uint8_t *message = NULL;
    uint8_t msglen;
    

    if (io_wav_read("2s.wav", &wav_data, &wav_struct) != LSTG_OK) {
        printf("Read failed: %d\n", lstg_errno);
        return 1;
    }

    eh_parameter para;
    para.amplitude = 0.6;//0.1f
    para.zero_offset = 1000;
    para.one_offset = 3000;
    para.block_size = 100000;
    uint32_t capacity = 0;

    if ( eh_extract(&wav_data, &message, &msglen, &para) != LSTG_OK) {
        printf("echo-hiding extracting failed:  %d\n", lstg_errno);
        return 1;
    } 
    
    printf("Extract vorbei\n");
    printf("%s\n", message);

    if (io_wav_cleanup_internal(&wav_struct) != LSTG_OK) {
        printf("Cleanup internal failed: %d\n", lstg_errno);
        return 1;
    }

    if (io_wav_cleanup_data(&wav_data) != LSTG_OK) {
        printf("Cleanup internal failed: %d\n", lstg_errno);
        return 1;
    }


    return 0;
}
