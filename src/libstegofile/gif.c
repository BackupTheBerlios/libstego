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
 *  Authors: Matthias Kloppenborg, Ralf Treu
 *
 *  For more info visit <http://parsys.informatik.uni-oldenburg.de/~stego/>
 */
 
 
 
#include "libstego/gif.h"
#include "libstego/types.h"
#include "libstego/errors.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <gif_lib.h>

/**
* Function to read a gif-file from HDD and return a palette_data_t
* ...
* @param *filename the path and filename of the gif-file
* @param *palette_data a struct to return the palettedata
* @param *gif_struct a struct provided by the gif-library to hold the original image
* @return an errorcode or 0 if success
*/

uint32_t io_gif_read(const uint8_t *filename, palette_data_t *palette_data, gif_internal_data_t *gif_struct)
{
    printf("...start of io_gif_read \n");
    GifFileType* gif_file = NULL; // always initialze pointers to NULL
    if(palette_data == 0){
	return LSTG_E_INVALIDPARAM;
    }
    uint32_t error;
    gif_file = DGifOpenFileName(filename);
    if(gif_file == NULL){
	printf("DGIFOPENFILENAME...FAILED \n");
	return LSTG_E_OPENFAILED;
    }
    if((error = DGifSlurp(gif_file)) != 1){
	printf("DGIFSLURP...FAILED \n");
	return LSTG_E_READFAILED;
    }
    gif_struct->SWidth = gif_file->SWidth;
    gif_struct->SHeight = gif_file->SHeight;
    gif_struct->SColorResolution = gif_file->SColorResolution;
    gif_struct->SBackGroundColor = gif_file->SBackGroundColor;
    if(!gif_file->SColorMap){
	printf("error #1 \n");
	return LSTG_E_FRMTNOTSUPP;
    }
    gif_struct->SColorMap = (ColorMapObject*)malloc(sizeof(ColorMapObject));
    gif_struct->SColorMap->ColorCount = gif_file->SColorMap->ColorCount;
    gif_struct->SColorMap->BitsPerPixel = gif_file->SColorMap->BitsPerPixel;
    gif_struct->SColorMap->Colors = (GifColorType*)malloc(sizeof(GifColorType) 
	    * gif_struct->SColorMap->ColorCount);
    for(uint32_t i = 0; i < gif_struct->SColorMap->ColorCount; i++){
	gif_struct->SColorMap->Colors[i].Red = 
	    gif_file->SColorMap->Colors[i].Red;
	gif_struct->SColorMap->Colors[i].Green = 
	    gif_file->SColorMap->Colors[i].Green;
	gif_struct->SColorMap->Colors[i].Blue = 
	    gif_file->SColorMap->Colors[i].Blue;
    }
    printf("...SColorMap filled \n");
    gif_struct->ImageCount = gif_file->ImageCount;
    gif_struct->Image.Left = gif_file->Image.Left;
    gif_struct->Image.Top = gif_file->Image.Top;
    gif_struct->Image.Width = gif_file->Image.Width;
    gif_struct->Image.Height = gif_file->Image.Height;
    gif_struct->Image.Interlace = gif_file->Image.Interlace;
    if(gif_file->Image.ColorMap){
	gif_struct->Image.ColorMap = (ColorMapObject*) malloc 
	    (sizeof(ColorMapObject));
	gif_struct->Image.ColorMap->ColorCount = 
	    gif_file->Image.ColorMap->ColorCount;
	gif_struct->Image.ColorMap->BitsPerPixel =
	    gif_file->Image.ColorMap->BitsPerPixel;
	gif_struct->Image.ColorMap->Colors = (GifColorType*) 
	    malloc (sizeof(GifColorType) 
		    * gif_struct->Image.ColorMap->ColorCount);
	for(uint32_t i = 0; i < gif_struct->Image.ColorMap->ColorCount; i++){
	    gif_struct->Image.ColorMap->Colors[i].Red = 
		gif_file->Image.ColorMap->Colors[i].Red;
	    gif_struct->Image.ColorMap->Colors[i].Green = 
		gif_file->Image.ColorMap->Colors[i].Green;
	    gif_struct->Image.ColorMap->Colors[i].Blue = 
		gif_file->Image.ColorMap->Colors[i].Blue;
	}
    }
    gif_struct->SavedImages = (SavedImage*)malloc(sizeof(SavedImage) 
	    * gif_struct->ImageCount);
    for(uint32_t i = 0; i < gif_struct->ImageCount; i++){
	gif_struct->SavedImages[i].ImageDesc = 
	    gif_file->SavedImages[i].ImageDesc;
	gif_struct->SavedImages[i].RasterBits = 
	    (uint8_t*) malloc (sizeof(uint8_t) 
		* gif_struct->SavedImages[i].ImageDesc.Height 
		    * gif_struct->SavedImages[i].ImageDesc.Width);
	for(uint32_t j = 0; j < gif_struct->SavedImages[i].ImageDesc.Height 
		* gif_struct->SavedImages[i].ImageDesc.Width; j++){
	    gif_struct->SavedImages[i].RasterBits[j] = 
		gif_file->SavedImages[i].RasterBits[j];
	}
	gif_struct->SavedImages[i].Function = gif_file->SavedImages[i].Function;
	gif_struct->SavedImages[i].ExtensionBlockCount = 
	    gif_file->SavedImages[i].ExtensionBlockCount;

	gif_struct->SavedImages[i].ExtensionBlocks = (ExtensionBlock*) malloc
	    (sizeof(ExtensionBlock) 
	     * gif_struct->SavedImages[i].ExtensionBlockCount);
	for(uint32_t j = 0; j < gif_struct->SavedImages[i].ExtensionBlockCount;
	       	j++){
	    gif_struct->SavedImages[i].ExtensionBlocks[j].ByteCount = 
		gif_file->SavedImages[i].ExtensionBlocks[j].ByteCount;
            printf("READ BYTECOUNT: %i\n",gif_struct->SavedImages[i].ExtensionBlocks[j].ByteCount); 

	    gif_struct->SavedImages[i].ExtensionBlocks[j].Function = 
		gif_file->SavedImages[i].ExtensionBlocks[j].Function;
	    gif_struct->SavedImages[i].ExtensionBlocks[j].Bytes =
                (uint8_t*) malloc(sizeof(uint8_t) 
                        * gif_struct->SavedImages[i].ExtensionBlocks[j].ByteCount);
	    for(uint32_t x = 0; x < gif_struct->SavedImages[i].ExtensionBlocks[j].ByteCount; x++) {
                gif_struct->SavedImages[i].ExtensionBlocks[j].Bytes[x] =
                        gif_file->SavedImages[i].ExtensionBlocks[j].Bytes[x];
            }       
	}
    }
    gif_struct->UserData = gif_file->UserData;
    gif_struct->Private = gif_file->Private;
    // make a copy of the 'filename' string
    gif_struct->SrcFilename = 
	(uint8_t*)malloc(sizeof(uint8_t)*strlen(filename));
    if (gif_struct->SrcFilename == NULL) {
	printf("error #2 \n");
        return LSTG_E_MALLOC;
    }
    // copy string
    strcpy(gif_struct->SrcFilename, filename);
    DGifCloseFile(gif_file);
    gif_file = 0;
    printf("...gif_struct filled \n");
    palette_data->tbl_size = gif_struct->SColorMap->ColorCount;
    palette_data->size_x = gif_struct->SavedImages[0].ImageDesc.Width;
    palette_data->size_y = gif_struct->SavedImages[0].ImageDesc.Height;
    palette_data->table = (rgb_pixel_t *)malloc(sizeof(rgb_pixel_t) 
	    * palette_data->tbl_size);
    if (palette_data->table == 0){
	    printf("error #3 \n");
            return LSTG_E_MALLOC;            
    }
    for(uint32_t i = 0; i < palette_data->tbl_size; i++){
        palette_data->table[i].comp.r = gif_struct->SColorMap->Colors[i].Red;
        palette_data->table[i].comp.g = gif_struct->SColorMap->Colors[i].Green;
        palette_data->table[i].comp.b = gif_struct->SColorMap->Colors[i].Blue;
    }
    palette_data->img_data = (uint8_t*) malloc(sizeof(uint8_t) 
            * palette_data->size_x * palette_data->size_y);
    if(palette_data->img_data == 0){
	printf("error #4 \n");
	return LSTG_E_MALLOC;
    }
    for(uint32_t i = 0; i < palette_data->size_x * palette_data->size_y; i++){
        palette_data->img_data[i] = gif_struct->SavedImages[0].RasterBits[i];
    }
    /* get the unique and non unique colors in color table */
    uint32_t unique_length = 0;
    uint32_t nonunique_length = 0;
    uint32_t is_unique = 1;
    rgb_pixel_t *unique = (rgb_pixel_t *) malloc 
        (sizeof(rgb_pixel_t) * palette_data->tbl_size);
    rgb_pixel_t *nonunique = (rgb_pixel_t *) malloc
        (sizeof(rgb_pixel_t) * palette_data->tbl_size);
    for(uint32_t i = 0; i < palette_data->tbl_size; i++){
        is_unique = 1;
        for(uint32_t j = 0; j < palette_data->tbl_size; j++){
            if (i != j
                && palette_data->table[j].comp.r == 
                  palette_data->table[i].comp.r
                && palette_data->table[j].comp.g == 
                  palette_data->table[i].comp.g
                && palette_data->table[j].comp.b == 
                palette_data->table[i].comp.b){
                is_unique = 0;
                break;
            }
        }
        // move to unique colours:
        if (is_unique == 1) {
            unique[unique_length].comp.r = palette_data->table[i].comp.r;
            unique[unique_length].comp.g = palette_data->table[i].comp.g;
            unique[unique_length].comp.b = palette_data->table[i].comp.b;
            unique_length += 1;
        }
        // move to non unique colours:
        else {
            nonunique[nonunique_length].comp.r = palette_data->table[i].comp.r;
            nonunique[nonunique_length].comp.g = palette_data->table[i].comp.g;
            nonunique[nonunique_length].comp.b = palette_data->table[i].comp.b;
            nonunique_length += 1;
        }
    }
    palette_data->nonunique_colors = (rgb_pixel_t *) malloc(sizeof(rgb_pixel_t) * nonunique_length);
    palette_data->unique_colors = (rgb_pixel_t *) malloc (sizeof(rgb_pixel_t) * unique_length);
    if (palette_data->unique_colors == NULL 
            || palette_data->nonunique_colors == NULL){
	printf("error #5 \n");
        return LSTG_E_MALLOC; //not enough space available in heap
    }
    for(uint32_t i = 0; i < unique_length; i++){
        palette_data->unique_colors[i].comp.r = unique[i].comp.r;
        palette_data->unique_colors[i].comp.g = unique[i].comp.g;
        palette_data->unique_colors[i].comp.b = unique[i].comp.b;
    }
    for(uint32_t i = 0; i < nonunique_length; i++){
        palette_data->nonunique_colors[i].comp.r = nonunique[i].comp.r;
        palette_data->nonunique_colors[i].comp.g = nonunique[i].comp.g;
        palette_data->nonunique_colors[i].comp.b = nonunique[i].comp.b;
    }
    palette_data->unique_length = unique_length;
    palette_data->nonunique_length = nonunique_length;
    free(unique);
    unique = 0;
    free(nonunique);
    nonunique = 0;
    return 0;
}


/**
* Function to integrate a changed palette_data_t into a gif_struct
* @param *gif_struct a struct provided by the gif-library to hold the original image
* @param *palette_data a struct to give the changed palettedata (steganogram)
* @return an errorcode or 0 if success
*/
uint32_t io_gif_integrate(gif_internal_data_t *gif_struct, const palette_data_t *palette_data)
{
    if(gif_struct == 0 || palette_data == 0){
        return LSTG_E_INVALIDPARAM;
    }
    gif_struct->SColorMap->ColorCount = palette_data->tbl_size;
    for(uint32_t i = 0; i < palette_data->tbl_size; i++){
        gif_struct->SColorMap->Colors[i].Red = palette_data->table[i].comp.r;
        gif_struct->SColorMap->Colors[i].Green = palette_data->table[i].comp.g;
        gif_struct->SColorMap->Colors[i].Blue = palette_data->table[i].comp.b;
    }
    for(uint32_t i = 0; i < palette_data->size_x * palette_data->size_y; i++) {
	gif_struct->SavedImages[0].RasterBits[i] = palette_data->img_data[i];
    }
    return 0;
}

/**
* Function to write a gif_struct to HDD by using the gif-library
* @param *filename the path and filename of the gif-file
* @param *gif_struct a struct provided by the gif-library to hold the original image
* @return an errorcode or 0 if success
*/
uint32_t io_gif_write(const uint8_t *filename, gif_internal_data_t *gif_struct)
{   
    GifFileType *gif_file = 0;
    gif_file = EGifOpenFileName(filename, 0);
    if(gif_file == 0){
	printf("EGifOpenFileName...failed \n");
	return LSTG_E_OPENFAILED;
    }
    //Cannot write the original Version, caused by libgif functionality
    //EGifSetGifVersion("GIF89a");
    uint32_t error;
    if(error = EGifPutScreenDesc(gif_file,
		    gif_struct->SWidth,gif_struct->SHeight,
		    gif_struct->SColorResolution,gif_struct->SBackGroundColor,
		    gif_struct->SColorMap) != 1){
	printf(" %i \n",gif_struct->SWidth);
	printf(" %i \n",gif_struct->SHeight);
	printf(" %i \n",gif_struct->SColorResolution);
	printf(" %i \n",gif_struct->SBackGroundColor);
	printf("EGifPutSCREENDESC...FAILED %i \n",error);
	return 1;
    }
    for (uint32_t i = 0; i < gif_struct->ImageCount; i++) {
	if((error = EGifPutImageDesc(gif_file,
	    gif_struct->SavedImages[i].ImageDesc.Left,
	    gif_struct->SavedImages[i].ImageDesc.Top,
	    gif_struct->SavedImages[i].ImageDesc.Width,
	    gif_struct->SavedImages[i].ImageDesc.Height,
	    gif_struct->SavedImages[i].ImageDesc.Interlace,
	    gif_struct->SavedImages[i].ImageDesc.ColorMap)) != 1){
	    printf("EGifIMAGEDESC...FAILED \n");
	    return 1;
	}
	for(uint32_t j = 0; 
		j < gif_struct->SavedImages[i].ImageDesc.Width 
		* gif_struct->SavedImages[i].ImageDesc.Height; 
		j++){
	    if((error = EGifPutPixel(gif_file, 
			    gif_struct->SavedImages[i].RasterBits[j])) != 1){
	    	printf("EGifPutPIXEL...FAILED \n");
		return 1;
	    }
	}
	//don't blame me for copying from giflib 
        if (gif_struct->SavedImages[i].ExtensionBlockCount > 0) {
	    uint32_t bOff = 0;
	    uint32_t j = 0;
	    for(j = 0; 
		    j < gif_struct->SavedImages[i].ExtensionBlockCount; j++){
		ExtensionBlock *ep = 
		    &gif_struct->SavedImages[i].ExtensionBlocks[j];
                if (j == gif_struct->SavedImages[i].ExtensionBlockCount - 1 
			|| (ep+1)->Function != 0) {
                    if (EGifPutExtension(gif_file,
                                         (ep->Function != 0) 
					 ? ep->Function : '\0',
                                         ep->ByteCount,
                                         ep->Bytes) == GIF_ERROR) {
                        return (GIF_ERROR);
                    }
                } else {
                    EGifPutExtensionFirst(gif_file, ep->Function, ep->ByteCount, ep->Bytes);
                    for (bOff = j+1; bOff < gif_struct->SavedImages[i].ExtensionBlockCount; bOff++) {
                        ep = &gif_struct->SavedImages[i].ExtensionBlocks[bOff];
                        if (ep->Function != 0) {
                            break;
                        }
                        EGifPutExtensionNext(gif_file, 0,
                                ep->ByteCount, ep->Bytes);
                    }
                    EGifPutExtensionLast(gif_file, 0, 0, NULL);
                    j = bOff-1;
                }
            }
	}
    }
    if((error = EGifCloseFile(gif_file)) != 1){
	return 1;
    }
    return 0;
}

/**
* Function to cleanup the internal data struct
* @param *gif_file the struct for the internal data of a gif file
* @return an errorcode or 0 if success
*/
uint32_t io_gif_cleanup_internal(gif_internal_data_t *gif_file)
{   
    free(gif_file->SColorMap->Colors);
    gif_file->SColorMap->Colors = 0;
    for(uint32_t i = 0; i < gif_file->ImageCount; i++){
	free(gif_file->SavedImages[i].RasterBits);
	gif_file->SavedImages[i].RasterBits = 0;
	for(uint32_t j = 0; j < gif_file->SavedImages[i].ExtensionBlockCount;
		j++){
	    free(gif_file->SavedImages[i].ExtensionBlocks[j].Bytes);
	    gif_file->SavedImages[i].ExtensionBlocks[j].Bytes = 0;
	}
	free(gif_file->SavedImages[i].ExtensionBlocks);
	gif_file->SavedImages[i].ExtensionBlocks = 0;
    }
    free(gif_file->SColorMap);
    gif_file->SColorMap = 0;
    free(gif_file->SrcFilename);
    gif_file->SrcFilename = 0;
    return 0;
}

/**
* Function to cleanup the data struct used by libstego
* @param *palette_data the struct used by libstego
* @return an errorcode or 0 if success
*/
uint32_t io_gif_cleanup_data(palette_data_t *palette_data)
{
    free(palette_data->table);
    palette_data->table = 0;
    free(palette_data->unique_colors);
    palette_data->unique_colors = 0;
    free(palette_data->nonunique_colors);
    palette_data->nonunique_colors = 0;
    free(palette_data->img_data);
    palette_data->img_data = 0;
    return 0;
}
