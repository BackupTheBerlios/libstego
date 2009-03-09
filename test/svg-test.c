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
 
 
 
#include <libstego/svg.h>
#include <libstego/svg_steg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(int argc, char *argv[]) {

    svg_data_t svg_data, svg_stego;
    svg_internal_data_t internal;
    svg_parameter_t param;
    uint8_t message[] = "Hallo Welt";
    uint8_t *msg_buf = 0;
    uint32_t msglen = strlen(message) + 1, capacity = 0;

    param.password = (uint8_t*)malloc(sizeof(uint8_t)*5);
    memcpy(param.password, "test", 5);
    param.pwlen = strlen(param.password);
    param.first_embed_digit = 1;

    printf("Embedding '%s'...\n", message);

    if (io_svg_read("penguin.svg", &svg_data, &internal) != LSTG_OK) {
        printf("io_svg_read failed with error code %d\n", lstg_errno);
        return 1;
    }

    if (svg_check_capacity(&svg_data, &capacity, &param) != LSTG_OK) {
        printf("svg_check_capacity failed with error code %d\n", lstg_errno);
        return 1;
    }

    printf("Capacity is %d bits\n", capacity);

    if (svg_embed(&svg_data, &svg_stego, message, msglen, &param) != LSTG_OK) {
        printf("svg_embed failed with error code %d\n", lstg_errno);
        return 1;
    }

    if (io_svg_integrate(&internal, &svg_stego) != LSTG_OK) {
        printf("io_svg_integrate failed with error code %d\n", lstg_errno);
        return 1;
    }

    if (io_svg_write("stego_penguin.svg", &internal) != LSTG_OK) {
        printf("io_svg_write failed with error code %d\n", lstg_errno);
        return 1;
    }

    io_svg_cleanup_data(&svg_data);
    io_svg_cleanup_data(&svg_stego);
    io_svg_cleanup_internal(&internal);

    msglen = 0;

    printf("\nExtracting...\n");

    if (io_svg_read("stego_penguin.svg", &svg_stego, &internal) != LSTG_OK) {
        printf("io_svg_read failed with error code %d\n", lstg_errno);
        return 1;
    }

    if (svg_extract(&svg_stego, &msg_buf, &msglen, &param) != LSTG_OK) {
        printf("svg_extract failed with error code %d\n", lstg_errno);
        return 1;
    }

    printf("Message length: %d\n", msglen);
    printf("Message: '%s'\n", msg_buf);

    io_svg_cleanup_data(&svg_stego);
    io_svg_cleanup_internal(&internal);

    free(param.password);
    free(msg_buf);

    return 0;
}
