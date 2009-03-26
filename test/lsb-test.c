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
 
 
 
#include <libstego/lsb.h>
#include <libstego/png.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int test_indirect(lsb_parameter_t *param) {
    int ret = 0, i = 0;
	FILE *fp = 0;
	uint32_t capacity = 0, length = 0;
	uint8_t message[] = "王冠";
	uint8_t *buffer = 0;
	uint8_t *cover = 0;
	uint8_t **bytes = 0;
	uint32_t size = 128;

	printf("Embedding \"%s\"\n", message);

	cover = (uint8_t*) malloc(sizeof(uint8_t) * size);
	bytes = (uint8_t**)malloc(sizeof(uint8_t*) * size);

	fp = fopen("lsb.txt", "r");

	// lese data.size * 1 byte aus der datei
	size = fread(cover, 1, size, fp);

	fclose(fp);

	fp = 0;

	printf("Bytes read: %d\n", size);

	// get pointers
	for (i = 0; i < size; ++i) {
	    bytes[i] = &cover[i];
	}

	if (lsb_check_capacity_indirect(bytes, size, &capacity, param) != LSTG_OK) {
		printf("lsb_check_capacity_indirect failed with error code %d\n", lstg_errno);
		return 1;
	} else {
		printf("Capacity: %d\n", capacity);
	}

    printf("lsb_embed_indirect...\n");

    if (lsb_embed_indirect(bytes, size, message, strlen(message)+1, param)!= LSTG_OK) {
        printf("lsb_embed_indirect failed with error code %d\n", lstg_errno);
        return 1;
    } else {
    	printf("Embedded successfully\n");
    }

	fp = fopen("lsb_out.txt", "w+");

	fwrite(cover, 1, size, fp);

	fclose(fp);

	free(cover);
	free(bytes);
	cover = 0;
	bytes = 0;

	printf("Extracting...\n");

	cover = (uint8_t*) malloc(sizeof(uint8_t) * size);
	bytes = (uint8_t**)malloc(sizeof(uint8_t*) * size);

	fp = fopen("lsb_out.txt", "r");

	size = fread(cover, 1, size, fp);

	fclose(fp);

	printf("Bytes read: %d\n", size);

	// get pointers
	for (i = 0; i < size; ++i) {
	    bytes[i] = &cover[i];
	}

	if (lsb_get_message_length_indirect(bytes, size, &length, param) != LSTG_OK) {
		printf("lsb_get_message_length_indirect failed with error code %d\n", lstg_errno);
		return 1;
	} else {
		printf("Message length: %d\n", length);
	}

	if (lsb_extract_indirect(bytes, size, &buffer, &length, param) != LSTG_OK) {
		printf("lsb_extract_indirect failed with error code %d\n", lstg_errno);
		return 1;
	} else {
		printf("Message: %s\n", buffer);
	}

	free(buffer);
	free(cover);
	free(bytes);

	return 0;
}

int test(lsb_parameter_t *param) {
    int ret = 0, i = 0;
	FILE *fp = 0;
	lsb_data_t data, stego;
	uint32_t capacity = 0, length = 0;
	uint8_t message[] = "王冠";
	uint8_t *buffer = 0;
	uint8_t *cover = 0;

	printf("Embedding \"%s\"\n", message);

	data.size = 128;
	data.data = (uint8_t*) malloc(sizeof(uint8_t) * data.size);
	stego.size = 128;
	stego.data = (uint8_t*) malloc(sizeof(uint8_t) * data.size);

	fp = fopen("lsb.txt", "r");

	// lese data.size * 1 byte aus der datei
	data.size = fread(data.data, 1, data.size, fp);

	fclose(fp);

	fp = 0;

	printf("Bytes read: %d\n", data.size);

	if (lsb_check_capacity(&data, &capacity, param) != LSTG_OK) {
		printf("lsb_check_capacity failed with error code %d\n", lstg_errno);
		return 1;
	} else {
		printf("Capacity: %d\n", capacity);
	}

    printf("lsb_embed...\n");

    if (lsb_embed(&data, &stego, message, strlen(message)+1, param)!= LSTG_OK) {
        printf("lsb_embed failed with error code %d\n", lstg_errno);
        return 1;
    } else {
    	printf("Embedded successfully\n");
    }

	fp = fopen("lsb_out.txt", "w+");

	fwrite(stego.data, 1, stego.size, fp);

	fclose(fp);

	free(data.data);
	free(stego.data);
	data.data = 0;
	stego.data = 0;

	printf("Extracting...\n");

	data.data = (uint8_t*) malloc(sizeof(uint8_t)*data.size);

	fp = fopen("lsb_out.txt", "r");

	data.size = fread(data.data, 1, data.size, fp);

	fclose(fp);

	printf("Bytes read: %d\n", data.size);

	if (lsb_get_message_length(&data, &length, param) != LSTG_OK) {
		printf("lsb_get_message_length failed with error code %d\n", lstg_errno);
		return 1;
	} else {
		printf("Message length: %d\n", length);
	}

	if (lsb_extract(&data, &buffer, &length, param) != LSTG_OK) {
		printf("lsb_extract failed with error code %d\n", lstg_errno);
		return 1;
	} else {
		printf("Message: %s\n", buffer);
	}

	free(buffer);
	free(data.data);

	return 0;
}


int test_png(lsb_parameter_t *param) {
    rgb_data_t rgb_data;
    png_internal_data_t png_internal;
    uint8_t **bytes = 0;
    uint32_t size = 0, capacity = 0, length = 0;
    uint8_t *buffer = 0;
    uint8_t message[] = "王冠";

    printf("Embedding \"%s\"\n", message);

    printf("Reading 'stego.png'\n");

    if(io_png_read("bild.png", &rgb_data, &png_internal) != LSTG_OK) {
        printf("io_png_read failed with error code %d\n", lstg_errno);
        return 1;
    }

    if (lsb_convert_png(&rgb_data, &bytes, &size) != LSTG_OK) {
        printf("lsb_convert_png failed with error code %d\n", lstg_errno);
        return 1;
    }

    printf("Converted %d bytes of the PNG to be used for LSB into %p\n", size, bytes);

	if (lsb_check_capacity_indirect(bytes, size, &capacity, param) != LSTG_OK) {
		printf("lsb_check_capacity_indirect failed with error code %d\n", lstg_errno);
		return 1;
	} else {
		printf("Capacity: %d\n", capacity);
	}

    printf("lsb_embed_indirect...\n");

    if (lsb_embed_indirect(bytes, size, message, strlen(message)+1, param)!= LSTG_OK) {
        printf("lsb_embed_indirect failed with error code %d\n", lstg_errno);
        return 1;
    } else {
    	printf("Embedded successfully\n");
    }

    io_png_integrate(&png_internal, &rgb_data);
    io_png_write("stego_bild.png", &png_internal);

    printf("Output written to 'stego_bild.png'\n");

    free(buffer);
    buffer = 0;
    lsb_cleanup_converted_data(bytes);
    bytes = 0;
    io_png_cleanup_data(&rgb_data);
    io_png_cleanup_internal(&png_internal);


	printf("\nExtracting...\n");

    printf("Reading 'stego_bild.png'\n");
    if(io_png_read("stego_bild.png", &rgb_data, &png_internal) != LSTG_OK) {
        printf("io_png_read failed with error code %d\n", lstg_errno);
        return 1;
    }

    if (lsb_convert_png(&rgb_data, &bytes, &size) != LSTG_OK) {
        printf("lsb_convert_png failed with error code %d\n", lstg_errno);
        return 1;
    }

    printf("Converted %d bytes of the PNG to be used for LSB\n", size);

	if (lsb_get_message_length_indirect(bytes, size, &length, param) != LSTG_OK) {
		printf("lsb_get_message_length_indirect failed with error code %d\n", lstg_errno);
		return 1;
	} else {
		printf("Message length: %d\n", length);
	}

	if (lsb_extract_indirect(bytes, size, &buffer, &length, param) != LSTG_OK) {
		printf("lsb_extract_indirect failed with error code %d\n", lstg_errno);
		return 1;
	} else {
		printf("Message: %s\n", buffer);
	}

    free(buffer);
    lsb_cleanup_converted_data(bytes);
    io_png_cleanup_data(&rgb_data);
    io_png_cleanup_internal(&png_internal);
}


int main(int argc, char *argv[]) {
	//int8_t lsb_embed(uint8_t *message, uint32_t length, lsb_data_t *data, lsb_parameter_t *param);

    lsb_parameter_t param;

	param.password = strdup("Test");
	param.pwlen = 4;
	param.use_msb = 1;

    printf("Direct: Testing continuous selecting\n\n");
    param.select_mode = LSB_SELECT_CONTINUOUS;
	test(&param);

	remove("lsb_out.txt");

	printf("\n\nDirect: Testing random selecting\n\n");
	param.select_mode = LSB_SELECT_RANDOM;
	test(&param);

    printf("\n\nIndirect: Testing continuous selecting\n\n");
    param.select_mode = LSB_SELECT_CONTINUOUS;
	test_indirect(&param);

	remove("lsb_out.txt");

	printf("\n\Indirect: Testing random selecting\n\n");
	param.select_mode = LSB_SELECT_RANDOM;
	test_indirect(&param);

    /*printf("Indirect: Convert from PNG, embed continuous\n\n");
    param.select_mode = LSB_SELECT_CONTINUOUS;
    test_png(&param);*/

	free(param.password);

    return 0;
}
