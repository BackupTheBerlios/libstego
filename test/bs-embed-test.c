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
 
 
 
#include <libstego/png.h>
#include <stdio.h>
#include <libstego/errors.h>
#include <libstego/libstego.h>

int main(int argc, char *argv[]) {
    rgb_data_t rgb_data;
    filter_map_t filter_map;
    png_internal_data_t png_internal;
    int ret = 0;

    uint8_t error = io_png_read("bild.png", &rgb_data, &png_internal);
    if(error != 0) {
        printf("Error: %d\n", lstg_errno);
        return 1;   
    }
    uint32_t width = rgb_data.size_x;
    printf("Size:  %dx%dpx\n", rgb_data.size_x, rgb_data.size_y);

    battlesteg_parameter para;
    para.startbit = 4;
    //para.endbit = 7;
    para.range = 5;
    para.move_away = 2;
    para.password = "foobar2111111111";
    para.pwlen = 16;
    uint32_t capa = 0;
    battlesteg_check_capacity(&rgb_data, &capa, &para);
    printf("Capacity = %d Bits = %d Bytes\n", capa, capa/8);
    rgb_data_t stego_data;
    char *message = "Hallo Welt!";
    printf("Embedding \"%s\"\n", message);
    error = battlesteg_embed(&rgb_data, &stego_data, message, 12, &para);
       if(error != 0) {
        printf("Error: %d\n", lstg_errno);
        return 1;   
    }
    io_png_integrate(&png_internal, &stego_data);
    io_png_write("new.png", &png_internal);
    printf("Capacity = %d Bits = %d Bytes\n", capa, capa/8);
    io_png_cleanup_data(&rgb_data);
    io_png_cleanup_data(&stego_data);
    io_png_cleanup_internal(&png_internal);
    return 0;
}

