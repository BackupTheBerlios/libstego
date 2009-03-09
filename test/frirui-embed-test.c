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
#include <libstego/frirui.h>
#include <libstego/types.h>
#include <stdio.h>
#include <libstego/errors.h>
#include <libstego/libstego.h>

int main(int argc, char *argv[]) {
    const char *src_filename = "test.gif";
    palette_data_t palette_data;
    gif_internal_data_t gif_struct;
    
    const char *dest_filename = "test_stego.gif";
    palette_data_t stego_data;
    gif_struct.SrcFilename = NULL;
    uint32_t ret = 0;
    frirui_parameter f_param;
    uint8_t *message = "Hallo Welt";
    uint32_t msglen = 10;
    uint32_t error = 0;
    uint32_t capacity = 0;
    //gif_struct and palette_data are allocated without substructs
  
    if (io_gif_read(src_filename, &palette_data, &gif_struct) != 0) {
        printf("Read failed.\n");
        return;
    }
    //gif struct is allocated with all substructs
    //palette_data is allocated with all substructs

    //param set
    f_param.method = 1;
    f_param.size = 10;
    f_param.threshold = 1138;
    f_param.password = "test";
    f_param.pwlen = 4;

    frirui_check_capacity(&palette_data, &capacity, &f_param);
    printf("capacity is %i bits.\n", capacity);
    
    //stego_data is an allocated struct without substructs
    if((error = frirui_embed(&palette_data, &stego_data, message, msglen, &f_param)) != 0){
	printf("embedding with method %i... failed\n", f_param.method);
	return;
    } 
    else{
	printf("embedding with method %i... OK\n", f_param.method);
    }
    if((error = io_gif_integrate(&gif_struct, &stego_data)) != 0){
	printf("integrate failed \n");
    }
    else {
	printf("integrate done \n");
    }
    if((error = io_gif_write(dest_filename, &gif_struct)) != 0) {
        printf("Write failed. Error code: %d\n", ret);
	return;
    }
    else {
	printf("Write... DONE\n");
    }
    if((error = io_gif_cleanup_internal(&gif_struct)) != 0){
    	printf("Cleaning up gif types's internal data failed. Err # %d\n", error);
	return;
    }
    else {
	printf("Cleaning up gif type's internal data worked.\n");
    }
    if((error = io_gif_cleanup_data(&palette_data)) != 0){
	printf("Cleaning up gif types's palette_data failed. Err # %d\n", error);
	return;
    }
    else {
	printf("Cleaning up gif type's palette_data worked.\n");
    }
    if((error = io_gif_cleanup_data(&stego_data)) != 0){
	printf("Cleaning up gif types's stego_data failed. Err # %d\n", error);
	return;
    }
    else {
	printf("Cleaning up gif type's stego_data worked.\n");
    }
    printf("READ STEGANOGRAMM\n");
    gif_internal_data_t gif_file;
    if (io_gif_read(dest_filename, &stego_data, &gif_file) != 0) {
        printf("Read failed.\n");
        return;
    }
    uint32_t msglen2 = 0;
    uint8_t *message2 = 0;
    if((error = frirui_extract(&stego_data,&message2, &msglen2, &f_param) != 0)){
	printf("FAIL:Message was %s , extracted %s \n", message, message2);
        return;
    } else {
	printf("OK:Message was %s , extracted %s \n", message, message2);
    }
    
    if((error = io_gif_cleanup_internal(&gif_file)) != 0){
    	printf("Cleaning up gif types's internal data failed. Err # %d\n", error);
        return;
    }
    else {
	printf("Cleaning up gif type's internal data worked.\n");
    }
    
    if((error = io_gif_cleanup_data(&stego_data)) != 0){
	printf("Cleaning up gif types's stego_data failed. Err # %d\n", error);
        return;
    }
    else {
	printf("Cleaning up gif type's stego_data worked.\n");
    }
}
	


    

