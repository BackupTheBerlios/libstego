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
#include <stdio.h>

int main(int argc, char *argv[]) {
    palette_data_t palette_data;
    palette_data_t stego_data;
    gif_internal_data_t gif_struct;
    uint8_t * message = "H";
    uint32_t msg_length = 1;
    sortunsort_parameter para;   
    para.password = "pw";
    para.pwlen = 2;
    if (test_embed(&palette_data, &gif_struct, "test.gif", "test_new.gif",
                28, &stego_data, message, msg_length, &para) == 0) {
        printf("Embed Test passed.\n");
    }
    else {
        printf("Embed Test failed.\n");
    }
    if (test_extract(&palette_data, &gif_struct, "test_new.gif", 28, &para) == 0) {
        printf("Extract Test passed.\n");
    }
    else {
        printf("Extact Test failed.\n");
    }
    return 0;
    /*
    palette_data_t palette_data2;
    gif_internal_data_t gif_struct2;
    printf("\n");
    if (test(&palette_data2, &gif_struct2, "test2.gif", "test2_new.gif", 255) == 0) {
        printf("Test with test2.gif passed.\n");
    }
    else {
        printf("Test with test2.gif failed.\n");
    }
    return 0;*/
}

int test_embed(palette_data_t* palette_data, 
        gif_internal_data_t* gif_struct,
        const uint8_t* filename, 
        const uint8_t* dest_filename,
        uint32_t assert_unique_colours,
        palette_data_t *stego_data,
        uint8_t *message,
        uint32_t msg_length,
        sortunsort_parameter* para) {
    uint32_t ret; 
    printf("Starting Test with %s ...\n", filename);
    if (io_gif_read(filename, palette_data, gif_struct) != 0) {
        printf("  <-- Opening gif file failed. Error code: %d\n", ret);
        return 1;
    }
    else {
        printf("  --> Opening gif file worked.\n");
    }
    uint32_t num_unique_colors = 0;
    sortunsort_check_capacity(palette_data,&num_unique_colors, para);
    printf("unique_colors: %i\n",palette_data->unique_length);
    printf("sortunsort capacity: %i\n",num_unique_colors);
    if (num_unique_colors == assert_unique_colours) {
        printf("  --> Computation of capacity worked.\n");
    }
    else {
        printf("  <-- Computation of capacity did NOT work.\n");
    }

    printf ("Embedding!\n");
    if (sortunsort_embed(palette_data, stego_data, message, msg_length, para) != 0) {
        printf("  <-- Embedding message failed.\n");
    }
    else {
        printf("  --> Embedding message worked.\n");
    }
    
    if ((ret = io_gif_integrate(gif_struct, stego_data)) != 0) {
        printf("  <-- Integrating in gif struct failed. Error code: %d\n", ret);
        return 1;
    }
    else {
        printf("  --> Integrate gif struct worked.\n");
    }

    if ((ret = io_gif_write(dest_filename, gif_struct)) != 0) {
        printf("  <-- Writing gif file failed. Error code: %d\n", ret);
        return 1;
    }
    else {
        printf("  --> Writing gif file worked.\n");
    }
    if (ret = io_gif_cleanup_internal(gif_struct) != 0) {
        printf("  <-- Cleaning up gif type's internal data failed. Error code: %d\n", ret);
        return 1;
    }
    else {
        printf("  --> Cleaning up gif type's internal data worked.\n");
    }
    if (ret = io_gif_cleanup_data(palette_data) != 0) {
        printf("  <-- Cleaning up gif type's data failed. Error code: %d\n", ret);
        return 1;
    }
    else {
        printf("  --> Cleaning up gif type's data worked.\n");
    }
    if (ret = io_gif_cleanup_data(stego_data) != 0) {
        printf("  <-- Cleaning up stego data failed. Error code: %d\n", ret);
        return 1;
    }
    else {
        printf("  --> Cleaning up stego data worked.\n");
    }
    return 0;
}

int test_extract(palette_data_t* palette_data, 
        gif_internal_data_t* gif_struct, 
        uint8_t *filename,
        uint32_t assert_unique_colours,
        sortunsort_parameter* para) {
    uint8_t *message = 0;
    uint32_t msglen = 0;
    // Extract again and check what comes out of it
    printf("Checking message in %s ...\n", filename);
    if (io_gif_read(filename, palette_data, gif_struct) != 0) {
        printf("  <-- Opening gif file failed. \n");
        return 1;
    }
    else {
        printf("  --> Opening gif file worked.\n");
    }
    uint32_t num_unique_colors = 0;
    sortunsort_check_capacity(palette_data,&num_unique_colors,para);
    printf("unique_colors: %i\n",palette_data->unique_length);
    printf("sortunsort capacity: %i\n",num_unique_colors);
    if (num_unique_colors == assert_unique_colours) {
        printf("  --> Computation of capacity worked.\n");
    }
    else {
        printf("  <-- Computation of capacity did NOT work.\n");
    }

    printf ("Extracting!\n");
    if (sortunsort_extract(palette_data,&message, &msglen, para) != 0) {
        printf("  <-- Extracting message failed.\n");
    }
    else {
        printf("  --> Extracting message worked.\n");
        printf("  --> Message: %s\n", message);
    }
    if (io_gif_cleanup_internal(gif_struct) != 0) {
        printf("  <-- Cleaning up gif type's internal data failed.\n");
        return 1;
    }
    else {
        printf("  --> Cleaning up gif type's internal data worked.\n");
    }
    if (io_gif_cleanup_data(palette_data) != 0) {
        printf("  <-- Cleaning up gif type's data failed.\n");
        return 1;
    }
    else {
        printf("  --> Cleaning up gif type's data worked.\n");
    }

    return 0;
}
