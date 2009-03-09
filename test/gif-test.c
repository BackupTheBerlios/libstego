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
 
 
 
#include <libstego/gif.h>
#include <libstego/sort_unsort.h>
#include <libstego/gifshuffle.h>
#include <stdio.h>

int main(int argc, char *argv[]) {
    palette_data_t palette_data;
    gif_internal_data_t gif_struct;
    const char * dest_filename = "test_new.gif";
    gif_struct.SrcFilename = NULL;
    int ret = 0;
    frirui_parameter f_param;
    f_param.method = 0;
    f_param.size = 2;


    printf("Testing Read...\n");
    if (io_gif_read("test.gif", &palette_data, &gif_struct) != 0) {
        printf("Read failed.\n");
        return 1;
    }
    int i = 0;
    for(i=0; i<palette_data.tbl_size;i++) {
        printf("%u %u %u\n",
            palette_data.table[i].comp.r,
            palette_data.table[i].comp.g,
            palette_data.table[i].comp.b);
    }

    printf("------------\n");
    for(i = 0; i < palette_data.unique_length; i++) {
        printf("%u %u %u\n",
            palette_data.unique_colors[i].comp.r,
            palette_data.unique_colors[i].comp.g,
            palette_data.unique_colors[i].comp.b);
    }


    /*printf("So we have %i unique colors...\n",palette_data.unique_length);
    printf("Testing Capacity with Sort_Unsort...\n");
    int num_unique_colors = sortunsort_check_capacity(&palette_data,NULL);
    printf("Sort_Unsort Capacity is %i Bits.\n",num_unique_colors);

    printf("Testing Capacity with GIFShuffle...\n");
    num_unique_colors = gifshuffle_check_capacity(&palette_data,NULL);
    printf("GIFShuffle Capacity is %i Bits.\n",num_unique_colors);
    
    printf("Testing Capacity with frirui...\n");
    int capacity = frirui_check_capacity(&palette_data, &f_param);
    printf("Frirui Capacity is %i Bits.\n", capacity);

    printf("Testing Write...\n");
    if ((ret = io_gif_write(dest_filename, &gif_struct)) != 0) {
        printf("Write failed. Error code: %d\n", ret);
        return 1;
    }*/
    printf("Testing Cleanup Internal\n");
    if (io_gif_cleanup_internal(&gif_struct) != 0) {
        printf("Cleanup internal failed.\n");
        return 1;
    }

    printf("Testing Cleanup Data\n");
    if (io_gif_cleanup_data(&palette_data) != 0) {
        printf("Cleanup internal failed.\n");
        return 1;
    }
    
    return 0;
}
