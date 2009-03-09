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

int main(int argc, char *argv[]) {
    wav_data_t wav_data;
    wav_internal_data_t wav_struct;
    wav_struct.data = NULL;
    int ret = 0;

    printf("wav_struct.data = %p\n", wav_struct.data);

    if (io_wav_read("test.wav", &wav_data, &wav_struct) != 0) {
        printf("Read failed.\n");
        return 1;
    }

    printf("wav_struct.data = %p\n", wav_struct.data);

    if ((ret = io_wav_write("test_new.wav", &wav_struct)) != 0) {
        printf("Write failed. Error code: %d\n", ret);
        return 1;
    }

    if (io_wav_cleanup_internal(&wav_struct) != 0) {
        printf("Cleanup internal failed.\n");
        return 1;
    }

    if (io_wav_cleanup_data(&wav_data) != 0) {
        printf("Cleanup internal failed.\n");
        return 1;
    }

    return 0;
}
