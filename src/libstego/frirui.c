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
 
 
 
#include "libstego/frirui.h"

/**
 * Calculates the parity of each color in the colortable. Adds each red, green and blue component and calculates mod 2.
 * @param *parity Pointer to a struct that contains the parities of all colors 
 * @param *table Pointer to a struct that contains all colors with the 
 * rgb components
 * @param tbl_size Number of colors in table
 */
void frirui_calc_parity(uint8_t *parity, rgb_pixel_t *table, 
			    uint32_t tbl_size)
{
    printf("...start to calculate parities \n");
    for(uint32_t i = 0; i < tbl_size; i++){
	uint8_t red = table[i].comp.r;
	uint8_t green = table[i].comp.g;
	uint8_t blue = table[i].comp.b;
	parity[i] = (red + green + blue) % 2;
//	printf("......parity[%u] = %u \n", i, parity[i]);
    }
    printf("...calculated parities\n");
}
/**
 * Calculates the distances for all colors in the colortable.
 * @param *table Pointer to a struct that contains all colors with the rgb 
 * components
 * @param tbl_size Number of colors in table
 * @param *distances Pointer to a struct that contains the distances of all 
 * colors
 * @param dist_length Length of the distances struct
 * @return Returns LSTG_E_FRMTNOTSUPP if distances can not be calculated, 
 * LSTG_OK else
 */
uint32_t frirui_calc_distances(rgb_pixel_t *table, uint32_t tbl_size, 
				DISTANCE *distances, uint32_t dist_length)
{
    printf("...start to calculate distances \n");
    //calculate distances
    uint32_t ptr = 0;
    for(uint32_t i = 0; i < tbl_size; i++){
	for(uint32_t j = i + 1; j < tbl_size; j++){
	    uint32_t dist_red = 0; 
	    if(table[i].comp.r > table[j].comp.r){
		dist_red = table[i].comp.r - table[j].comp.r;
	    } 
	    else {
		dist_red = table[j].comp.r - table[i].comp.r;
	    }
	    uint32_t dist_green = 0;
	    if(table[i].comp.g > table[j].comp.g){
		dist_green = table[i].comp.g - table[j].comp.g;
	    }
	    else {
		dist_green = table[j].comp.g - table[i].comp.g;
	    }
	    uint32_t dist_blue = 0;
	    if(table[i].comp.b > table[j].comp.b){
		dist_blue = table[i].comp.b - table[j].comp.b;
	    }
	    else{
		dist_blue = table[j].comp.b - table[i].comp.b;
	    }
	    distances[ptr].distance = dist_red + dist_green + dist_blue;
	    distances[ptr].color1 = i;
	    distances[ptr].color2 = j;
	    ptr++;
	    if(ptr > dist_length){
		return LSTG_E_FRMTNOTSUPP;
	    }
	}
    }
    //rearrange distances from smallest to biggest
    DISTANCE helper;
    for(uint32_t i = 0; i < dist_length ; i++){
	for(uint32_t j = i + 1; j < dist_length; j++){
	    if(distances[i].distance > distances[j].distance){
		helper = distances[i];
		distances[i] = distances[j];
		distances[j] = helper;
	    }
	}
    }
//    for(uint32_t i = 0; i < dist_length; i++){
//	printf("---color %u : color %u : distance %u \n", distances[i].color1,
//		distances[i].color2, distances[i].distance);
//    }
    printf("...calculated distances\n");
    return LSTG_OK;
}

/**
 * Rearranges the indices of the colortable in a new array, eventually the 
 * parities are changed.
 * @param *distances Pointer to a struct that holds the distances of all colors 
 * in colortable
 * @param dist_length Length of the distances struct
 * @param *new_colortable Holds the rearranged indices of the colortable
 * @param tbl_size Number of colors in new_colortable
 * @param *parity Pointer to a struct that contains the parities of all colors 
 * @return Returns LSTG_E_FRMTNOTSUPP if new_colortable could not be build, else
 * LSTG_OK
 */
uint32_t frirui_build_colortable(DISTANCE *distances, uint32_t dist_length, 
				uint8_t *new_colortable, uint32_t tbl_size, 
				uint8_t *parity)
{
    printf("...started building colortable\n");
    uint32_t ptr = 0;
    for(uint32_t i = 0; i < dist_length; i++){//for every element in distance
	uint8_t holds_c1 = 0;
	uint8_t holds_c2 = 0;
	//look if some of the colors in the element is already saved
	for(uint32_t j = 0; j < ptr; j++){
	    if(ptr > tbl_size){
		printf("failed\n");
		return LSTG_E_FRMTNOTSUPP;
	    }
	    if(distances[i].color1 == new_colortable[j]){
//		printf("---color #%u = %u \n", i, new_colortable[i]);
		holds_c1 = 1;
	    }
	    if(distances[i].color2 == new_colortable[j]){
//		printf("---color #%u = %u \n", i, new_colortable[i]);
		holds_c2 = 1;
	    }
	}
	//change parity and add to new colortable
	if(holds_c1 == 0 && holds_c2 == 0){
	    if(parity[distances[i].color1] == parity[distances[i].color2]){
		if(parity[distances[i].color1] == 0){
		    parity[distances[i].color1] = 1;
		} else {
		    parity[distances[i].color1] = 0;
		}
	    }
	    new_colortable[ptr] = distances[i].color1;
	    ptr += 1;
	    new_colortable[ptr] = distances[i].color2;
	    ptr += 1; 
	} else
	if(holds_c1 == 0 && holds_c2 == 1){
	    if(parity[distances[i].color1] == parity[distances[i].color2]){
		if(parity[distances[i].color1] == 0){
		    parity[distances[i].color1] = 1;
		} else {
		    parity[distances[i].color1] = 0;
		}
	    }
	    new_colortable[ptr] = distances[i].color1;
	    ptr += 1;
	    holds_c2 = 0;
	} else
	if(holds_c2 == 0 && holds_c1 == 1){
	    if(parity[distances[i].color1] == parity[distances[i].color2]){
		if(parity[distances[i].color2] == 0){
		    parity[distances[i].color2] = 1;
		} else {
		    parity[distances[i].color2] = 0;
		}
	    }
	    new_colortable[ptr] = distances[i].color2;
	    ptr += 1;
	    holds_c1 = 0;
	}
    }
//    for(uint32_t i = 0; i < tbl_size; i++){
//	printf("---color #%u = %u \n", i, new_colortable[i]);
//    }
    printf("...done building colortable\n");
    return LSTG_OK;
} 

/**
 * Embeds one bit to a specified chain
 * @param bit Bit to embed
 * @param *chain Chain to embed to
 * @param ch_length Length of the chain
 * @param *parity Pointer to a struct that contains the parities of all colors 
 * @param *table Pointer to a struct that contains all colors with the 
 * rgb components
 * @param new_colortable Holds the rearranged indices of the colortable
 * @param tbl_size Number of colors in new_colortable
 * @return Returns LSTG_E_FRMTNOTSUPP if there is no color to change, 
 * LSTG_OK else
 * */
uint32_t embed_bit_to_chain(uint8_t bit, CHAIN *chain, uint32_t ch_length, 
	uint8_t *parity, rgb_pixel_t *table, uint8_t *new_colortable, 
	uint32_t tbl_size)
{
    printf("......started embed bit to chain\n");
    uint32_t distance = 255;
    uint32_t pixel_in_chain;
    uint32_t new_colorindex;
//    printf("chain_length %u \n", ch_length);
    for(uint32_t k = 0; k < ch_length; k ++){
	//search for the color which maps the parity and has 
	//the lowest distance
	uint32_t m = 0;
	while(!(chain->pixels[k] == new_colortable[m])){
//	    printf("chain %u : colortable %u \n", chain->pixels[k], new_colortable[m]);  
	    if(m > tbl_size){
		return LSTG_E_FRMTNOTSUPP;
	    }
	    m++;
	}
//	printf("index of orig %u \n",m);
	for(uint32_t l = m + 1; l < tbl_size; l ++){ 
	    if(distance <= l - m){
		break;
	    }
	    // till the end of the new_colortable
	    if(parity[new_colortable[l]] 
		    != parity[chain->pixels[k]]){
		distance = l - m;
		pixel_in_chain = k;
		new_colorindex = new_colortable[l];
		break;
	    }
	}
	uint32_t l = m;
	while(distance > m - l){
	    if(parity[new_colortable[l]] 
		    != parity[chain->pixels[k]]){
		distance = m - l;
		pixel_in_chain = k;
		new_colorindex = new_colortable[l];
		break;
	    }
	    l--;
	}
    }
//  printf(".........changed #%u from %u to %u distance is %u \n", pixel_in_chain, 
//	  chain->pixels[pixel_in_chain], new_colorindex ,distance);
    chain->pixels[pixel_in_chain] = new_colorindex;
    printf("......done embed bit to chain\n");
    return LSTG_OK;
}

/**
 * Randomizes the indices of a given array
 * @param *state Pointer to the state of the PRNG
 * @param num_of_indices Threshold for generated values
 * @param *randomised_index Pointer to randomised index 
 */
uint8_t randomize_indices(  prng_state *state,uint32_t num_of_indices,
			    uint32_t *randomised_index)
{
    uint32_t new_index;
    uint32_t is_empty;
    for (uint32_t i = 0; i < num_of_indices; i++) {
        new_index = random_next(state) % num_of_indices;
        is_empty = 0;
        while (is_empty == 0) {
            is_empty = 1;
            for(uint32_t j = 0; j < i; j++) {
                if (new_index == randomised_index[j]) {
                    is_empty = 0;
                    break;
                }
            }
            if (is_empty == 0 && new_index < num_of_indices - 1) {
                new_index += 1;
            }
            else if (is_empty == 0 && new_index == num_of_indices -1) {
                new_index = 0;
            }
        }
        randomised_index[i] = new_index;
//	printf("randomized[%u] = %u \n", i, new_index);
    }
    return LSTG_OK;
}

/**
 * Embeds message to chains
 * @param *message Pointer to the array that holds the message
 * @param msglen Length of the message
 * @param *chains Pointer to the struct that holds the chains
 * @param ch_length Number of pixels in every chain
 * @param num_of_chains Length of the struct that holds the chains
 * @param *password Pointer to the struct that holds the password
 * @param pwlen Length of the password in Byte
 * @param size_x The width of the picture 
 * @param size_y The heigth of the picture
 * @param *parity Pointer to a struct that contains the parities of all colors 
 * @param *img_data Pointer to a struct that contains the indizes of the image
 * @param *new_colortable Holds the rearranged indices of the colortable
 * @param tbl_size Size of the new_colortable
 * @param *table Pointer to a struct that contains all colors with the rgb 
 * components
 * @return Returns LSTG_E_INVALIDPARAM if length of chains is smaller than 2;
 *	   LSTG_E_INSUFFCAP if message is to long; LSTG_ERROR if a bit could not
 *	   be embedded to a chain
 * */

uint32_t frirui_embed_message_to_chains(uint8_t *message, uint32_t msglen, 
	CHAIN *chains, uint32_t ch_length, uint32_t num_of_chains,
	uint8_t *password, uint32_t pwlen, uint32_t size_x, uint32_t size_y, 
	uint8_t *parity, uint8_t *img_data,uint8_t *new_colortable, 
	uint32_t tbl_size, rgb_pixel_t *table)
{

    printf("...starting embed message to chains\n"); 
    if (2 > ch_length){
	return LSTG_E_INVALIDPARAM; // parameter fault : chains must be greater or equal 2   
    }
    for(uint32_t i = 0; i < num_of_chains; i++){
	chains[i].pixels = (uint8_t*) malloc (sizeof(uint8_t) * ch_length);
	chains[i].id_in_stream = (uint32_t*) malloc (sizeof(uint32_t) * ch_length);
    }	    
    uint32_t ptr_img_data = 0;
//  uint32_t tmp = 0;
    for(uint32_t j = 0; j < num_of_chains; j++){
	chains[j].parity = 0;
	for(uint32_t i = 0; i < ch_length; i++){
	    chains[j].pixels[i] = img_data[ptr_img_data];
	    chains[j].id_in_stream[i] = ptr_img_data;
	    chains[j].parity += parity[chains[j].pixels[i]];
	    chains[j].parity &= 1;
	    ptr_img_data++;
	}
//	printf("chain %u : parity %u \n", j, chains[j].parity);
    }
    if(0 != password || 0 != pwlen){
	uint32_t *randomised_index = (uint32_t*) 
	    malloc (num_of_chains * sizeof(uint32_t));
	if(randomised_index == NULL){
	    for(uint32_t i = 0; i < num_of_chains; i++){
		SAFE_DELETE(chains[i].pixels);
		SAFE_DELETE(chains[i].id_in_stream);
	    }
	    SAFE_DELETE(randomised_index);	    
	    return LSTG_E_MALLOC;
	}
	prng_state *state = 0;
	if(1 == random_init(&state,password,pwlen)){
	    for(uint32_t i = 0; i < num_of_chains; i++){
		SAFE_DELETE(chains[i].pixels);
		SAFE_DELETE(chains[i].id_in_stream);
	    }
	    SAFE_DELETE(randomised_index);
	    return LSTG_E_MALLOC;
	}
	if(1 == randomize_indices(state,num_of_chains,randomised_index)){
	    for(uint32_t i = 0; i < num_of_chains; i++){
		SAFE_DELETE(chains[i].pixels);
		SAFE_DELETE(chains[i].id_in_stream);
	    }
	    SAFE_DELETE(randomised_index);
	    return LSTG_E_MALLOC;
	}
	CHAIN *helper = (CHAIN*) malloc (num_of_chains * sizeof(CHAIN));
	for(uint32_t i = 0; i < num_of_chains; i++){
	    helper[i].parity = chains[randomised_index[i]].parity;
	    helper[i].pixels = (uint8_t*) malloc (ch_length * sizeof(uint8_t));
	    helper[i].id_in_stream = (uint32_t*) malloc (ch_length * sizeof(uint32_t));
	    for(uint32_t j = 0; j < ch_length; j++){
		helper[i].pixels[j] = chains[randomised_index[i]].pixels[j];
		helper[i].id_in_stream[j] = chains[randomised_index[i]].id_in_stream[j];
	    }
	}
	SAFE_DELETE(randomised_index);
	for(uint32_t i = 0; i < num_of_chains; i++){
	    chains[i].parity = helper[i].parity;
	    for(uint32_t j = 0; j < ch_length;j++){
		chains[i].pixels[j] = helper[i].pixels[j];
		chains[i].id_in_stream[j] = helper[i].id_in_stream[j];
	    }
	    free(helper[i].pixels);
	    helper[i].pixels = 0;
	    free(helper[i].id_in_stream);
	    helper[i].id_in_stream = 0;
	}
	SAFE_DELETE(helper);
    }
    uint32_t len = msglen;
    for(uint32_t i = 0; i < 32; i++){//embed message length
	uint32_t bit = 0;
	if((len & 2147483648) == 2147483648){
	    bit = 1;
	}
//	    printf("bit %u \n",bit);
	if(chains[i].parity != bit){
	    if(embed_bit_to_chain(bit, &chains[i],ch_length, parity,
			table, new_colortable,tbl_size)){
		for(uint32_t i = 0; i < num_of_chains; i++){
		    SAFE_DELETE(chains[i].pixels);
		    SAFE_DELETE(chains[i].id_in_stream);
		}
		return LSTG_ERROR;
	    }
	}
	len = len << 1;
    }
    printf("...msglen embedded\n");

    uint8_t helper;
    uint32_t i = 32;
    while(i < num_of_chains){// embed message
	for(uint32_t j = 0; j < msglen; j++){
	    helper = message[j];
	    for(uint32_t k = 0; k < 8; k++){
//		    printf(" parity %u : message %u\n" , 
//		    chains[i].parity, ((helper & 128) == 128));
		if(chains[i].parity != ((helper & 128) == 128)){
		    if(embed_bit_to_chain(((helper & 128) == 128), 
			    &chains[i],ch_length, parity,table, 
			    new_colortable, tbl_size)){
			for(uint32_t i = 0; i < num_of_chains; i++){
			    SAFE_DELETE(chains[i].pixels);
			    SAFE_DELETE(chains[i].id_in_stream);
			}
			return LSTG_ERROR;
		    }
		}
		i++;
		helper <<= 1;
	    }
	}
	break;
    }
    if(i > num_of_chains){
	return LSTG_E_INSUFFCAP;
    }
    printf("...message embedded \n");
    //}
    printf("...done embed message to chains\n");
    return LSTG_OK;
}
/**
 * Embeds one bit to a specified 3X3BLOCK
 * @param bit Bit to embed
 * @param *block Block to embed to
 * @param tbl_size Number of colors in new_colortable
 * @param new_colortable Holds the rearranged indices of the colortable
 * @param *table Pointer to a struct that contains all colors with the
 * rgb components
 * @param *parity Pointer to a struct that contains the parities of all colors
 * @param threshold Holds the threshold for each block
 */
uint32_t embed_bit_to_3X3BLOCK(uint8_t bit, BLOCK3X3 *block, uint32_t tbl_size,
			    uint8_t *new_colortable, rgb_pixel_t *table, 
			    uint8_t *parity, uint32_t threshold)
{
    printf("......start of embed bit to 3BLOCK\n");
    uint32_t pos = 0;
    uint32_t distance = 256;
    uint32_t new_colorindex;
    uint32_t k;
    uint32_t l;
    uint32_t m = 0;
    for(k = 0; k < 9; k++){
	l = 0;
	while(block->pixels[k] != new_colortable[l]){
	    if(l > tbl_size){
		return LSTG_E_FRMTNOTSUPP;
	    }
	    l++;
	}
	for(m = l + 1; m < tbl_size; m++){
	    if(distance <= m - l){
		break;
	    }
	    if(parity[block->pixels[k]] != parity[new_colortable[m]]){
		distance = m - l;
		pos = k;
		new_colorindex = new_colortable[m];
		break;
	    }
	}		
	m = l;
	while(distance > m - l){
	    if(parity[block->pixels[k]] != parity[new_colortable[m]]){
		distance = l - m;
		pos = k;
		new_colorindex = new_colortable[m];
		break;
	    }
	    l--;
	}
    }
    if(distance > 255){
	printf("Something is wrong with the colors in the block \n");
	return LSTG_E_FRMTNOTSUPP;
    }
//  printf(".........changed pixel %u from %u to %u distance is %u \n", pos, 
//	block->pixels[pos], new_colorindex ,distance);

    block->pixels[pos] = new_colorindex;
    block->threshold = 0;
    for(uint32_t j = 0; j < 9; j++){
	block->threshold += block->pixels[j];
    }
    if(block->threshold <= threshold){
	printf("......done of embed bit to 3BLOCK\n");
	return LSTG_OK;
    } 
    else {
	printf("......failed to embed bit to 3BLOCK\n");
	return LSTG_ERROR;
    }
}
/**
 * Embeds message to 3X3BLOCKS
 * @param **matrix Represents the pixeldata like it is shown in the picture
 * @param rows Number of rows in the matrix
 * @param columns Number of columns in the matrix
 * @param *blocks Pointer to the struct that holds the 3X3Blocks
 * @param block_count Number of 3X3Blocks
 * @param threshold Holds the threshold for each block
 * @param *password Pointer to the struct that holds the password
 * @param pwlen Length of the password in Byte
 * @param *message Pointer to the array that holds the message
 * @param msglen Length of the message
 * @param *new_colortable Holds the rearranged indices of the colortable
 * @param tbl_size Size of the new_colortable
 * @param *table Pointer to a struct that contains all colors with the rgb 
 * components
 * @return LSTG_E_INSUFFCAP if message is to long; LSTG_OK else * */

uint32_t frirui_embed_message_to_3X3BLOCKS(uint8_t **matrix, uint32_t rows, 
				    uint32_t columns, BLOCK3X3 *blocks, 
				    uint32_t block_count,uint32_t threshold, 
				    uint8_t *password, uint32_t pwlen,
				    uint8_t *parity, uint8_t *message,
				    uint32_t msglen, uint8_t *new_colortable,
				    uint32_t tbl_size, 
				    rgb_pixel_t *table)
{
    printf("...starting embed message to 3BLOCKS\n");
    //fill 3x3 blocks
    uint32_t i = 0;
//    printf("block_count = %u \n", block_count);
    while(i < block_count){
	for(uint32_t j = 0; j < rows - 2; j+=3){
	    for(uint32_t k = 0; k < columns - 2; k+=3){
		blocks[i].pixels[0] = matrix[j][k];
//		printf("block[%u].pixels[0] = %u \n", i, blocks[i].pixels[0]);
		blocks[i].position_x[0] = j;
		blocks[i].position_y[0] = k;
		blocks[i].pixels[1] = matrix[j][k+1];
//		printf("block[%u].pixels[1] = %u \n", i, blocks[i].pixels[1]);
		blocks[i].position_x[1] = j;
		blocks[i].position_y[1] = k+1; 
		blocks[i].pixels[2] = matrix[j][k+2];
//		printf("block[%u].pixels[2] = %u \n", i, blocks[i].pixels[2]);
		blocks[i].position_x[2] = j;
		blocks[i].position_y[2] = k+2;
		blocks[i].pixels[3] = matrix[j+1][k];
//		printf("block[%u].pixels[3] = %u \n", i, blocks[i].pixels[3]);
		blocks[i].position_x[3] = j+1;
		blocks[i].position_y[3] = k;
		blocks[i].pixels[4] = matrix[j+1][k+1];
//		printf("block[%u].pixels[4] = %u \n", i, blocks[i].pixels[4]);
		blocks[i].position_x[4] = j+1;
		blocks[i].position_y[4] = k+1;
		blocks[i].pixels[5] = matrix[j+1][k+2];
//		printf("block[%u].pixels[5] = %u \n", i, blocks[i].pixels[5]);
		blocks[i].position_x[5] = j+1;
		blocks[i].position_y[5] = k+2;
		blocks[i].pixels[6] = matrix[j+2][k];
//		printf("block[%u].pixels[6] = %u \n", i, blocks[i].pixels[6]);
		blocks[i].position_x[6] = j+2;
		blocks[i].position_y[6] = k;
		blocks[i].pixels[7] = matrix[j+2][k+1];
//		printf("block[%u].pixels[7] = %u \n", i, blocks[i].pixels[7]);
		blocks[i].position_x[7] = j+2;
		blocks[i].position_y[7] = k+1;
		blocks[i].pixels[8] = matrix[j+2][k+2];
//		printf("block[%u].pixels[8] = %u \n", i, blocks[i].pixels[8]);
		blocks[i].position_x[8] = j+2;
		blocks[i].position_y[8] = k+2;
		i++;
	    }
	}
    }
    i = 0;
    while(i < block_count){
	for(uint32_t l = 0; l < 9; l++){
	    blocks[i].threshold += blocks[i].pixels[l];
	}
	for(uint32_t l = 0; l < 9; l++){
	    blocks[i].parity += parity[blocks[i].pixels[l]];
	}
//	printf("block %u : threshold %u : parity %u \n", i, blocks[i].threshold,
//		blocks[i].parity);
	blocks[i].parity = blocks[i].parity & 1;
	i++;
    }
    if(0 != password || 0 != pwlen){
	uint32_t *randomised_index = (uint32_t*) 
	    malloc (block_count * sizeof(uint32_t));
	if(randomised_index == NULL){
	    SAFE_DELETE(randomised_index);
	    return LSTG_E_MALLOC;
	}
	prng_state *state = 0;
	if(1 == random_init(&state,password,pwlen)){
	    SAFE_DELETE(randomised_index);
	    return LSTG_ERROR;
	}
	if(1 == randomize_indices(state,block_count,randomised_index)){
	    SAFE_DELETE(randomised_index);
	    return LSTG_ERROR;
	}
	BLOCK3X3 *helper = (BLOCK3X3*) malloc (block_count * sizeof(BLOCK3X3));
	for(uint32_t i = 0; i < block_count; i++){
	    helper[i].parity = blocks[randomised_index[i]].parity;
	    helper[i].threshold = blocks[randomised_index[i]].threshold;
	    for(uint32_t j = 0; j < 9; j++){
		helper[i].pixels[j] = blocks[randomised_index[i]].pixels[j];
		helper[i].position_x[j] = blocks[randomised_index[i]].position_x[j];		
		helper[i].position_y[j] = blocks[randomised_index[i]].position_y[j];
	    }
	}
	SAFE_DELETE(randomised_index);
	for(uint32_t i = 0; i < block_count; i++){
	    blocks[i].parity = helper[i].parity;
	    blocks[i].threshold = helper[i].threshold;
	    for(uint32_t j = 0; j < 9;j++){
		blocks[i].pixels[j] = helper[i].pixels[j];
		blocks[i].position_x[j] = helper[i].position_x[j];
		blocks[i].position_y[j] = helper[i].position_y[j];
	    }
	}
	SAFE_DELETE(helper);
    }
    //embed 
    uint32_t  len = msglen;
    i = 0;
    uint32_t j = 0;
    uint8_t bit = 0;
    for(j = 0; j < 32; j++){//embed message length
	bit = 0;
	if((len & 2147483648) == 2147483648){
	    bit = 1;
	}
	while(i < block_count){
	    if(blocks[i].threshold <= threshold){
		if(blocks[i].parity != bit){
//		    printf("lenbit = %u \n", bit);
//		    printf("...embedding to block %u in step %u \n", i, j); 
		    //zwei
		    if(!(embed_bit_to_3X3BLOCK(bit, &blocks[i], 
			tbl_size, new_colortable, table, parity, 
			threshold))){
			len = len << 1;
			i++;
			break;	
		    }
		} else {
//		    printf("lenbit = %u \n", bit);
//		    printf("...embedding to block %u in step %u \n", i, j);
		    len = len << 1;
		    i++;
		    break;
		}
	    } else {
		printf("block %u threshold to high \n", i);
	    }
	    i++;
	}
    }
    printf("...message length embedded\n");
    uint8_t helper;
    while(i < block_count){
    	for(j = 0; j < msglen; j++){
	    helper = message[j];
	    printf("message[%u] = %u \n", j, message[j]);
	    for(uint32_t k = 0; k < 8; k++){
		bit = ((helper & 128) == 128);
		if(blocks[i].threshold < threshold){
		    if(blocks[i].parity != bit){
			printf("a messagebit = %u \n", bit);
			printf("...embedding to block %u in step %u \n", i, j); 
			if(!(embed_bit_to_3X3BLOCK(bit, &blocks[i], 
					tbl_size, new_colortable, table, 
					parity, threshold))){
			    
			    helper <<= 1;
			} else {
			    k--;
			}
		    } else {
			printf("b messagebit = %u \n", bit);
			printf("...embedding to block %u in step %u \n", i, j); 
			helper <<= 1;
		    }
		} else {
		    printf("block %u threshold to high \n", i);
		    k--;
		}
		i++;
	    }
	}
	break;
    }
    if(i > block_count){
	return LSTG_E_INSUFFCAP;
    }
    printf("...message embedded\n");
    //set changes in matrix
    for(uint32_t i = 0; i < block_count; i++){
	for(uint32_t j = 0; j < 9; j++){
	    matrix[blocks[i].position_x[j]][blocks[i].position_y[j]] 
		= blocks[i].pixels[j];
	}
    }
    printf("...done embed message to 3BLOCKS\n");
    return LSTG_OK;
}
/**
 * Embeds one bit to a specified 2X2BLOCK
 * @param bit Bit to embed
 * @param *block Block to embed to
 * @param tbl_size Number of colors in new_colortable
 * @param *new_colortable Holds the rearranged indices of the colortable
 * @param *usable_colors Pointer to a struct that contains the indices of usablecolors
 * @param *table Pointer to a struct that contains all colors with the
 * rgb components
 * @param usable_length Number of usable colors
 * @param *parities Pointer to a struct that contains the parities of all colors
 * @return Returns LSTG_E_FRMTNOTSUPP if the color of a pixel could not be found * in the colortable; LSTG_ERROR if the bit could not be embedded; LSTG_OK else
 */

uint32_t embed_bit_to_2X2BLOCK(uint8_t bit, BLOCK2X2 *block, uint32_t tbl_size,
	uint8_t *new_colortable, 
	uint32_t *usable_colors, rgb_pixel_t *table, uint32_t usable_length, 
	uint8_t *parities)
{
    printf("......start of embed bit to 2BLOCK\n"); 
    uint32_t distance[] = {256,256,256,256};
    uint32_t new_color[] = {0,0,0,0};	
    uint32_t able_to_exchange = 0;
    for(uint32_t i = 0; i < 4; i++){
	uint32_t found_index = 0;
	uint32_t index = 0;
	while(!found_index){
	    index++;
	    if(block->pixels[i] == new_colortable[index]){
		found_index = 1;
	    } else
	    if(index > tbl_size){
		printf("color of pixel not found in colortable \n");    
		return LSTG_E_FRMTNOTSUPP;
	    }
	}
	printf("......index of color %u in pixel %u is %u parity is %u \n", 
		block->pixels[i], i, index, parities[block->pixels[i]]);
	int32_t left = index - 1;
	int32_t right = index + 1;
	uint32_t parity = parities[block->pixels[i]];
	uint32_t found_color_to_exchange = 0;
	while(!found_color_to_exchange){
	    if(parity == parities[new_colortable[right]] && right < tbl_size){
		right++;
//		printf("step right to %i \n", right);
	    } else
	    if(parity != parities[new_colortable[right]] 
		    && new_colortable[right] != block->pixels[i] ){
//		printf("found parity right at %u \n", right);
		for(uint32_t j = 0; j < usable_length; j++){
		    if(new_colortable[right] == new_colortable[usable_colors[j]]
			    && (right - index) < distance[i]){
//			printf("found usable color right\n");
			distance[i] = right - index;
			new_color[i] = new_colortable[right];
			able_to_exchange = 1;
			found_color_to_exchange = 1;
		    } else {
			right++;
//			printf("step right to %i \n", right);
			break;
		    }
		}
	    } 
	    if(parity == parities[new_colortable[left]] && left > 0){
		left--;
//		printf("step left to %i \n", left);
	    } else 	    
	    if(parity != parities[new_colortable[left]]
		    && new_colortable[left] != block->pixels[i] ){
//		printf("found parity left at %u \n", left);
		for(uint32_t j = 0; j < usable_length; j++){
		    if(new_colortable[left] == new_colortable[usable_colors[j]]
			    && (index - left) < distance[i]){
//			printf("found usable color left\n");
			distance[i] = index - left;
			new_color[i] = new_colortable[left];
			able_to_exchange = 1;
			found_color_to_exchange = 1;
		    } else {
			left--;
//			printf("step left to %i \n", left);
			break;
		    }
		}
	    }
	    if(right >= tbl_size && left <= 0){
		break;
	    }
	}
    }
    if(!able_to_exchange){
	printf("......found no usable color to exchange \n");
	//make block unusable, have less than 3 different colors
	uint32_t block_is_usable = 1;
	uint32_t i = 1;
	while(block_is_usable){
	    if((block->pixels[0] == block->pixels[1]
			&& block->pixels[2] == block->pixels[3])
	    || (block->pixels[0] == block->pixels[2] 
			&& block->pixels[1] == block->pixels[3])
	    || (block->pixels[0] == block->pixels[3] 
			&& block->pixels[1] == block->pixels[2])
	    ){
		block_is_usable = 0;
	    } else {
		block->pixels[i] = block->pixels[0];
		i++;
	    }
	}
    }
    uint32_t distance_to_usable_color = 256;
    uint32_t pixel_to_change = 5;
    for(uint32_t i = 0; i < 4; i++){
	if(distance[i] < distance_to_usable_color){
	    pixel_to_change = i;
	    distance_to_usable_color = distance[i];
	}
    }
    printf(".........changed pixel %u from %u to %u and changed parity to %u \n",
	    pixel_to_change, block->pixels[pixel_to_change], 
	    new_color[pixel_to_change],
	    parities[new_color[pixel_to_change]]);
    block->pixels[pixel_to_change] = new_color[pixel_to_change];
    if((block->pixels[0] == block->pixels[1] 
	    && block->pixels[2] == block->pixels[3])
	|| (block->pixels[0] == block->pixels[2] 
	    && block->pixels[1] == block->pixels[3])
	|| (block->pixels[0] == block->pixels[3] 
	    && block->pixels[1] == block->pixels[2])
	||  !able_to_exchange){
	printf("......failed to embed bit to 2BLOCK \n");
	return LSTG_ERROR;
    }
    else {
	printf("......done embed bit to 2BLOCK\n");
	uint32_t parity = 0;
	for(uint32_t i = 0; i < 4; i++){
	    parity += parities[block->pixels[i]];
	}
	parity = parity & 1;
	printf("......new parity = %u \n", parity);
	return LSTG_OK;
    }
}
/**
 * Embeds message to 2X2BLOCKS
 * @param **matrix Represents the pixeldata like it is shown in the picture
 * @param rows Number of rows in the matrix
 * @param columns Number of columns in the matrix
 * @param *blocks Pointer to the struct that holds the 2X2Blocks
 * @param block_count Number of 2X2Blocks
 * @param *password Pointer to the struct that holds the password
 * @param pwlen Length of the password in Byte
 * @param *parity Pointer to a struct that contains the parities of all colors
 * @param *message Pointer to the array that holds the message
 * @param msglen Length of the message
 * @param *new_colortable Holds the rearranged indices of the colortable
 * @param tbl_size Size of the new_colortable
 * @param *table Pointer to a struct that contains all colors with the rgb 
 * components
 * @return LSTG_E_INSUFFCAP if message is to long; LSTG_OK else * */

uint32_t frirui_embed_message_to_2X2BLOCKS(uint8_t **matrix, uint32_t rows, 
				    uint32_t columns, BLOCK2X2 *blocks, 
				    uint32_t block_count, uint8_t *password,
				    uint32_t pwlen, uint8_t *parity, 
				    uint8_t *message,
				    uint32_t msglen, uint8_t *new_colortable, 
				    uint32_t tbl_size, rgb_pixel_t *table)


{
    printf("...start embed message to 2BLOCKS\n");
    //fill 2x2 blocks
    uint32_t i = 0;
    while(i < block_count){
	for(uint32_t j = 0; j < rows - 1; j+=2){
	    for(uint32_t k = 0; k < columns - 1; k+=2){
		blocks[i].pixels[0] = matrix[j][k];
		blocks[i].position_x[0] = j;
		blocks[i].position_y[0] = k;
		blocks[i].pixels[1] = matrix[j][k+1];
		blocks[i].position_x[1] = j;
		blocks[i].position_y[1] = k + 1;
		blocks[i].pixels[2] = matrix[j+1][k];
		blocks[i].position_x[2] = j + 1;
		blocks[i].position_y[2] = k;
		blocks[i].pixels[3] = matrix[j+1][k+1];
		blocks[i].position_x[3] = j + 1;
		blocks[i].position_y[3] = k + 1;
		i++;
	    }
	}
    }
    i = 0;
    while(i < block_count){
	blocks[i].parity = 0;
	for(uint32_t l = 0; l < 4; l++){
		blocks[i].parity += parity[blocks[i].pixels[l]];
	}
	blocks[i].parity = blocks[i].parity & 1;
	i++;
    }
    printf("......blocks prepared\n");
    if(0 != password || 0 != pwlen){
	uint32_t *randomised_index = (uint32_t*) 
	    malloc (block_count * sizeof(uint32_t));
	if(randomised_index == NULL){
	    SAFE_DELETE(randomised_index);
	    return LSTG_E_MALLOC;
	}
	prng_state *state = 0;
	if(1 == random_init(&state,password,pwlen)){
	    SAFE_DELETE(randomised_index);
	    return LSTG_E_MALLOC;
	}
	if(1 == randomize_indices(state,block_count,randomised_index)){
	    SAFE_DELETE(randomised_index);
	    return LSTG_E_MALLOC;
	}
	BLOCK2X2 *helper = (BLOCK2X2*) malloc (block_count * sizeof(BLOCK2X2));
	for(uint32_t i = 0; i < block_count; i++){
	    helper[i].parity = blocks[randomised_index[i]].parity;
	    for(uint32_t j = 0; j < 4; j++){
		helper[i].pixels[j] = blocks[randomised_index[i]].pixels[j];
		helper[i].position_x[j] = blocks[randomised_index[i]].position_x[j];		
		helper[i].position_y[j] = blocks[randomised_index[i]].position_y[j];
	    }
	}
	SAFE_DELETE(randomised_index);
	for(uint32_t i = 0; i < block_count; i++){
	    blocks[i].parity = helper[i].parity;
	    for(uint32_t j = 0; j < 4;j++){
		blocks[i].pixels[j] = helper[i].pixels[j];
		blocks[i].position_x[j] = helper[i].position_x[j];
		blocks[i].position_y[j] = helper[i].position_y[j];
	    }
	}
	SAFE_DELETE(helper);
    }
    //find usable 2x2 blocks
    i = 0;
    uint32_t j = 0;
    uint32_t k = 0;
    uint32_t *usable_blocks = 0;
    uint32_t *not_usable_blocks = 0;
    while(i < block_count){
	// if block does not contain 3 different colors
	if((blocks[i].pixels[0] == blocks[i].pixels[1] 
		    && blocks[i].pixels[2] == blocks[i].pixels[3])
	|| (blocks[i].pixels[0] == blocks[i].pixels[2] 
		    && blocks[i].pixels[1] == blocks[i].pixels[3])
	|| (blocks[i].pixels[0] == blocks[i].pixels[3] 
		    && blocks[i].pixels[1] == blocks[i].pixels[2])){
	    j += 1;
	    uint32_t *helper = ALLOCN(uint32_t, j);
	    if(not_usable_blocks != 0){
		for (uint32_t l = 0; l < j -1; l++){
		    helper[l] = not_usable_blocks[l];
		}	
		SAFE_DELETE(not_usable_blocks);
		not_usable_blocks = ALLOCN(uint32_t, j);
		for(uint32_t l = 0; l < j - 1; l++){
		    not_usable_blocks[l] = helper[l];
		}
		not_usable_blocks[j-1] = i; 
		//printf("not_usable_blocks[%u] %u \n", j - 1, i);
	    } else  
		if(not_usable_blocks == 0){
		    not_usable_blocks = ALLOC(uint32_t);
		    not_usable_blocks[0] = i;
		    //printf("not_usable_blocks[0] %u \n", i);
	    }
	    SAFE_DELETE(helper);
	} else {
	    //if block contains 3 different colors
	    k += 1;
	    uint32_t *helper = ALLOCN(uint32_t, k);
	    if(usable_blocks != 0){
		for (uint32_t l = 0; l < k -1; l++){
		    helper[l] = usable_blocks[l];
		}
		SAFE_DELETE(usable_blocks);
		usable_blocks = ALLOCN(uint32_t, k);
		for(uint32_t l = 0; l < k - 1; l++){
		usable_blocks[l] = helper[l];
		} 
		usable_blocks[k-1] = i;
		//printf("usable_blocks[%u] %u \n", k - 1, i);
	    } else 
		if(usable_blocks == 0){
		    usable_blocks = ALLOC(uint32_t);
		    usable_blocks[0] = i;
		    //printf("usable_blocks[0] %u \n", i);
	    }
	    SAFE_DELETE(helper);
	}
	i++;
    }
    uint32_t usable_block_count = k;
    uint32_t not_usable_block_count = j;
    printf("......(not) usable blocks found\n");
    
/*    for(uint32_t i = 0; i < usable_block_count; i++){
	printf("usable_blocks[%u] = %u \n", i, usable_blocks[i]);
    }
*/   
/*    for(uint32_t i = 0; i < not_usable_block_count; i++){
	printf("not_usable_blocks[%u] = %u \n", i, not_usable_blocks[i]);
    }
*/
    //find usable colors
    uint32_t *not_usable_colors = 0;
    uint32_t not_usable_length = 0;
//    printf("tbl_size = %u \n", tbl_size);
//    printf("block_count = %u \n", block_count);
//    printf("usable_block_count = %u \n", usable_block_count);
//    printf("not_usable_count = %u \n",not_usable_block_count);
    for(i = 0; i < tbl_size; i++){
	for(k = 0; k < not_usable_block_count; k++){
//	    printf("new_colortable[%u] = %u \n", i, new_colortable[i]);
//	    printf("blocks[not_usable_blocks[%u]].pixels[0] = %u \n", k, blocks[not_usable_blocks[k]].pixels[0]);
//	    printf("blocks[not_usable_blocks[%u]].pixels[1] = %u \n", k, blocks[not_usable_blocks[k]].pixels[1]);
//	    printf("blocks[not_usable_blocks[%u]].pixels[2] = %u \n", k, blocks[not_usable_blocks[k]].pixels[2]);
//	    printf("blocks[not_usable_blocks[%u]].pixels[3] = %u \n", k, blocks[not_usable_blocks[k]].pixels[3]);
	    if((new_colortable[i] == blocks[not_usable_blocks[k]].pixels[0])
	    || (new_colortable[i] == blocks[not_usable_blocks[k]].pixels[1])
	    || (new_colortable[i] == blocks[not_usable_blocks[k]].pixels[2])
	    || (new_colortable[i] == blocks[not_usable_blocks[k]].pixels[3])){
		//farbe ungeeignet
		not_usable_length += 1;
		uint32_t *helper = ALLOCN(uint32_t, not_usable_length);
		if(not_usable_colors != 0){
		    for (uint32_t l = 0; l < not_usable_length - 1; l++){
			helper[l] = not_usable_colors[l];
		    }	
		    SAFE_DELETE(not_usable_colors);
		    not_usable_colors = ALLOCN(uint32_t, not_usable_length);
		    for(uint32_t l = 0; l < not_usable_length - 1; l++){
			not_usable_colors[l] = helper[l];
		    }
		    not_usable_colors[j-1] = i; 
		} else
		    if(not_usable_colors == 0){
			not_usable_colors = ALLOC(uint32_t);
			not_usable_colors[0] = i;
		}
		not_usable_colors[not_usable_length-1] = new_colortable[i];
		SAFE_DELETE(helper);
		break;
	    }
        }
    }
    SAFE_DELETE(not_usable_blocks);
    uint32_t *usable_colors = 0;
    uint32_t usable_length = 0;
    //find usable colors
    for(i = 0; i < tbl_size; i++){
	uint32_t included = 0;
	for(k = 0; k < not_usable_length; k++){
	   if(new_colortable[i] == not_usable_colors[k]){
		included = 1;
		break;
	   }
	}
	if(included == 0){
	    usable_length += 1;
	    uint32_t *helper = ALLOCN(uint32_t, usable_length);
	    for (uint32_t l = 0; l < usable_length - 1; l++){
		helper[l] = usable_colors[l];
	    }	
	    SAFE_DELETE(usable_colors);
	    usable_colors = ALLOCN(uint32_t, usable_length);
	    for(uint32_t l = 0; l < usable_length - 1; l++){
		usable_colors[l] = helper[l];
	    }
	    usable_colors[usable_length - 1] = i;
	}
    }
    SAFE_DELETE(not_usable_colors);
    if(usable_colors == 0){
	SAFE_DELETE(usable_colors);
	SAFE_DELETE(usable_blocks);
	printf("******FAILURE : no usable colors found \n");
	return LSTG_E_INSUFFCAP;
    }
    printf("......(not) usable colors found \n"); 
    //embed
    uint32_t  len = msglen;
    i = 0;
    uint8_t bit = 0;
    for(j = 0; j < 32; j++){//embed message length
	bit = 0;
	if((len & 2147483648) == 2147483648){
	    bit = 1;
	}
	printf("...lenbit = %u \n", bit);
	while(i < usable_block_count){
	    if(blocks[usable_blocks[i]].parity != bit){
		printf("...parity != bit : embedding to block %u in step %u \n", usable_blocks[i], j); 
		if(!(embed_bit_to_2X2BLOCK(bit, &blocks[usable_blocks[i]], 
				tbl_size, new_colortable, 
				usable_colors, table, usable_length, parity))){
			len <<= 1;
			i++;
			break;
		} else {
		    printf("...failed to embed to block %u in step %u \n", 
			usable_blocks[i], j);
			i++;
		}
	    } else {
	        printf("...parity == bit : embedded to block %u in step %u \n",usable_blocks[i], j);
		len = len << 1;
	        i++;
		break;
	    }
	}
    }
    printf("......message length embedded\n");
    uint8_t helper;
    for(j = 0; j < msglen; j++){
	helper = message[j];
	for(uint32_t k = 0; k < 8; k++){ 
	    bit = ((helper & 128) == 128);
	    if(i > usable_block_count){
		SAFE_DELETE(usable_colors);
		SAFE_DELETE(usable_blocks);
		return LSTG_E_INSUFFCAP;
	    } else 
	    if(blocks[usable_blocks[i]].parity != bit){
		    printf("...a messagebit = %u \n", bit);
		    printf("...embedding to block %u in step %u \n", 
			usable_blocks[i], k);
		    if(!(embed_bit_to_2X2BLOCK(bit,
				    &blocks[usable_blocks[i]], tbl_size, 
				    new_colortable, 
				    usable_colors, table, usable_length, 
				    parity))){
			helper <<=  1;
			i++;
		    } else {
			printf("...failed to embed to block %u in step %u \n", 
				usable_blocks[i], k);
			i++;
			k--;
		    }
	    } else {
		printf("...b messagebit = %u \n", bit);
		printf("...embedding to block %u in step %u\n", 
			usable_blocks[i], k);
		helper <<= 1;
		i++;
	    }
	}
    }
    if(i > usable_block_count){
	SAFE_DELETE(usable_colors);
	SAFE_DELETE(usable_blocks);
	return LSTG_E_INSUFFCAP;
    }
    uint32_t used_blocks = i;
    printf("......message embedded\n");
    //set changes in matrix
    for(uint32_t i = 0; i < used_blocks; i++){
	for(uint32_t j = 0; j < 4; j++){
/*	    printf("matrix[%u][%u] = %u \n",
		  blocks[usable_blocks[i]].position_x[j],
		  blocks[usable_blocks[i]].position_y[j],
		  blocks[usable_blocks[i]].pixels[j]); 
*/	    matrix[blocks[usable_blocks[i]].position_x[j]]
		[blocks[usable_blocks[i]].position_y[j]] = 
		blocks[usable_blocks[i]].pixels[j];
	    
	}
    }
    SAFE_DELETE(usable_colors);
    SAFE_DELETE(usable_blocks);
    printf("...done embed message to 2BLOCKS\n");
    return LSTG_OK;   
}

/**
 * Copies a palette_data_t struct
 * @param *src_data Pointer to source struct
 * @param *stego_data Pointer to the struct that holds the copy
 * @return Returns LSTG_E_FRMTNOTSUPP or LSTG_OK
 */
uint32_t copy_data(const palette_data_t *src_data, palette_data_t *stego_data)
{
    stego_data->tbl_size = src_data->tbl_size;
    stego_data->unique_length = src_data->unique_length;
    stego_data->nonunique_length = src_data->nonunique_length;
    stego_data->size_x = src_data->size_x;
    stego_data->size_y = src_data->size_y;
    
    stego_data->table = (rgb_pixel_t*) malloc (sizeof(rgb_pixel_t) 
	    * stego_data->tbl_size);
    if(stego_data->table == 0){ 
	SAFE_DELETE(stego_data->table);
	return LSTG_E_FRMTNOTSUPP;
    }
    for(int i = 0; i < stego_data->tbl_size; i++){
	stego_data->table[i] = src_data->table[i];
    }
    stego_data->unique_colors = (rgb_pixel_t*) malloc(sizeof(rgb_pixel_t) 
	    * stego_data->unique_length);
    if(stego_data->unique_colors == 0){
	SAFE_DELETE(stego_data->unique_colors);
	return LSTG_E_FRMTNOTSUPP;
    }
    for(int i = 0; i < stego_data->unique_length; i++){
	stego_data->unique_colors[i] = src_data->unique_colors[i];
    }
    stego_data->nonunique_colors = (rgb_pixel_t*) malloc(sizeof(rgb_pixel_t) 
	    * stego_data->nonunique_length);
    if(stego_data->nonunique_colors == 0){
	SAFE_DELETE(stego_data->nonunique_colors);
	return LSTG_E_FRMTNOTSUPP;
    }
    for(int i = 0; i < stego_data->nonunique_length; i++){
	stego_data->nonunique_colors[i] = src_data->nonunique_colors[i];
    }
    stego_data->img_data = (uint8_t*) malloc (sizeof(uint8_t)
	    * (stego_data->size_x * stego_data->size_y));
    if(stego_data->img_data == 0){
	SAFE_DELETE(stego_data->img_data);
	return LSTG_E_FRMTNOTSUPP;
    }
    for(int i = 0; i < stego_data->size_x * stego_data->size_y; i++){
	stego_data->img_data[i] = src_data->img_data[i];
    }
    return LSTG_OK;
}    

/**
* Embeds a message in a paletteimage using FriRui.
* The parity for each colour in the given palette must be calculated.
*                   
*                   0 if R + G + B even   
* f(R + G + B) = 
*                   1 if R + G + B uneven
*
* For each possible pair of colours the distance has to be calculated in the 
* following way
* 
* distance = | (R_1 - R_2) + (G_1 - G_2) + (B_1 - B_2) |
* 
* With this distances triples are formed like that 
*
* (colour1, colour2, distance)
*
* Now the colours in these triples are pushed into an new palette by iteration 
* from the lowest to the highest distance.
*
* In this process the parities of the colours get changed, so that the colour
* that gets pushed to the new palette has another parity as the one that already
* is in the new palette. Never change a colour in the new palette.
*
* This process stops when all colours are in the new palette. 
* 
* The pixels to change are choosen by another algorithm. 
* (para->method, PRNG)
*
* To embed a message bit in the pixeldata the colour is viewed in the original 
* colourmap. Then a look ist taken at the new palette an the colour that arose 
* from that one in the original palette. The parity from this colour is the 
* important one. 
*
* The colour in the image is changed. Therefore the one with the appropriate 
* parity and the lowest distance in the new palette gets searched.
*
* The colour is changed to the colour in the original palette, where
* the colour in the new palette arose from.
*
* @param *src_data a struct for palettedata to provide the original image
* @param *stego_data a struct for pixeldata to return the steganogram
* @param *message returnpointer for the extracted message
* @param msglen the length of the message
* @param *para additional parameters for FriRui, including passphrase
* @return an errorcode or 0 if success
*/
uint32_t frirui_embed(const palette_data_t *src_data, palette_data_t *stego_data, uint8_t *message, uint32_t msglen, const frirui_parameter *para)
{
    printf("start of frirui embed\n");
    uint32_t error;
    //copy src_data => stego_data
    if((error = copy_data(src_data, stego_data)) != LSTG_OK){
       	FAIL(error); 
    }
    //calculate parities
    uint8_t *parity = (uint8_t *) malloc (sizeof(uint8_t) 
			* (src_data->tbl_size));
    frirui_calc_parity(parity,  src_data->table, 
		    src_data->tbl_size);

    //malloc for distances
    uint32_t dist_length = 0;
    for(uint32_t i = src_data->tbl_size; i > 0; i--){
	dist_length += i - 1;
    }
    DISTANCE *distances = (DISTANCE *)malloc(sizeof(DISTANCE) * dist_length);
    //calculate distances
    if((error = frirui_calc_distances(src_data->table, src_data->tbl_size, 
				    distances, dist_length)) != LSTG_OK){
	SAFE_DELETE(distances);
	SAFE_DELETE(parity);    
	FAIL(error);
    }
    //change parities and build "a new colortable"
    uint8_t *new_colortable = (uint8_t *) malloc (sizeof(uint8_t) 
				* src_data->tbl_size);
    if((error = frirui_build_colortable(distances, dist_length, new_colortable,
		    src_data->tbl_size, parity)) != LSTG_OK){
	SAFE_DELETE(new_colortable);
	SAFE_DELETE(parity);    
	SAFE_DELETE(distances);
	FAIL(error);
    }
    //distances are not longer needed
    SAFE_DELETE(distances);
    //ready to embed a message?
    if (0 == para->method){
	//embed in chains
	uint32_t number_of_pixels = src_data->size_x * src_data->size_y;
	uint32_t num_of_chains = number_of_pixels / para->size; 
	//forget the last pixels that dont fill a whole chain
	CHAIN *chains = (CHAIN *) malloc (sizeof(CHAIN) * num_of_chains);
	if((error = frirui_embed_message_to_chains(message,msglen, chains, 
			para->size, num_of_chains, para->password, para->pwlen,
			src_data->size_x, src_data->size_y, parity, 
			src_data->img_data,new_colortable,
			src_data->tbl_size, src_data->table)) != LSTG_OK){
	    SAFE_DELETE(chains);
	    SAFE_DELETE(parity);    
	    SAFE_DELETE(new_colortable);
	    FAIL(error);
	}
	
	//fill the structs
        uint32_t pos_to_write = 0;
	for(uint32_t i = 0; i < num_of_chains; i++){//write changed data
	    for(uint32_t j = 0; j < para->size; j++){
		if(stego_data->img_data[chains[i].id_in_stream[j]] 
			!= chains[i].pixels[j]){
		    printf("index: %u : orig %u : stego %u \n", 
			chains[i].id_in_stream[j],
			stego_data->img_data[chains[i].id_in_stream[j]], 
			chains[i].pixels[j]);
		}
		stego_data->img_data[chains[i].id_in_stream[j]]=
		    chains[i].pixels[j];
		pos_to_write++;
	    }
	}
	printf("...changed data added to struct\n");
	uint32_t end = src_data->size_x * src_data->size_y;
	while(pos_to_write < end){//write unchanged data
	    stego_data->img_data[pos_to_write] 
	    = src_data->img_data[pos_to_write];
	    pos_to_write += 1;
	}
	for(uint32_t i = 0; i < src_data->tbl_size; i++){
	    stego_data->table[i] = src_data->table[i];
	}
	//clean up
	printf("...free chains and parity\n");
	for(uint32_t i = 0; i < num_of_chains; i++){
	    SAFE_DELETE(chains[i].pixels);
	    SAFE_DELETE(chains[i].id_in_stream);
	}
	SAFE_DELETE(chains);
	SAFE_DELETE(new_colortable);
	SAFE_DELETE(parity);
    }else
    if(1 == para->method){//embed in 3x3 blocks
	//initialize data for an dynamic array
	uint32_t rows = src_data->size_x;
	uint32_t columns = src_data->size_y;    
	uint8_t **matrix = 0;
	uint32_t i,j,k = 0;
	//malloc heap for the array
	matrix = (uint8_t **) malloc (sizeof(uint8_t*) * rows);
	if (0 == matrix) {
	    SAFE_DELETE(matrix);
	    SAFE_DELETE(new_colortable);
	    SAFE_DELETE(parity);    
	    printf("error #1 in frirui.c\n");
	    FAIL(LSTG_E_MALLOC);
	}
	for (uint32_t i = 0; i < rows; i++){
	    matrix[i] = (uint8_t *) malloc (sizeof(uint8_t) * columns);
	    if (0 == matrix[i]){
		printf("error #2 in frirui.c\n");
		for(uint32_t i = 0; i < rows; i++){
		    SAFE_DELETE(matrix[i]);
		}
		SAFE_DELETE(matrix);
		SAFE_DELETE(new_colortable);
		SAFE_DELETE(parity);    
		FAIL(LSTG_E_MALLOC);	
	    }
	}
	//fill the array with the image data
	i = 0;
	uint32_t number_of_pixels = rows * columns;
	while (i < number_of_pixels){
	    for(j = 0; j < rows; j++){
		for (k = 0; k < columns; k++){
		    matrix[j][k] = src_data->img_data[i];
//		    printf("img_data %u = %u : matrix[%u][%u] = %u \n", i, 
//			    src_data->img_data[i], j, k, matrix[j][k]);
		    i++;
		}
	    }
	}
	//malloc heap for 3x3 blocks
	uint32_t available_x = src_data->size_x / 3 ;
	uint32_t available_y = src_data->size_y / 3 ;
	uint32_t block_count = available_y * available_x;
	BLOCK3X3 *blocks = (BLOCK3X3 *) malloc (sizeof(BLOCK3X3) * block_count);
	if((error = frirui_embed_message_to_3X3BLOCKS(matrix, rows, columns,
			blocks, block_count,para->threshold, para->password, 
			para->pwlen, parity, message, msglen, new_colortable,
		       	src_data->tbl_size, src_data->table)) != LSTG_OK){
	    SAFE_DELETE(blocks);
	    for(uint32_t i = 0; i < rows; i++){
		SAFE_DELETE(matrix[i]);
	    }
	    SAFE_DELETE(new_colortable);
	    SAFE_DELETE(matrix);
	    SAFE_DELETE(parity);    
	    FAIL(error);
	    printf("error\n");
	}
	//write changed data to structs
	i = 0;
	while(i < number_of_pixels){
	    for(uint32_t j = 0; j < rows; j++){
		for(uint32_t k = 0; k < columns; k++){
		    stego_data->img_data[i] = matrix[j][k];
		    i++;
		}
	    }
	}
	//clean the mess up
	SAFE_DELETE(blocks);
	for(uint32_t i = 0; i < rows; i++){
	    SAFE_DELETE(matrix[i]);
	}
	SAFE_DELETE(new_colortable);
	SAFE_DELETE(matrix);
	SAFE_DELETE(parity);
    } else
    if(2 == para->method){
	//initialize data for an dynamic array
	uint32_t rows = src_data->size_x;
	uint32_t columns = src_data->size_y;    
	uint8_t **matrix = 0;
	uint32_t i,j,k = 0;
	//malloc heap for the array
	matrix = (uint8_t **) malloc (sizeof(uint8_t *) * rows);
	if (NULL == matrix) {
	    SAFE_DELETE(matrix);
	    SAFE_DELETE(new_colortable);
	    SAFE_DELETE(parity);    
	    printf("error #3 in frirui.c\n");
	    FAIL(LSTG_E_MALLOC);
	}
	for (i = 0; i < rows; i++){
	    matrix[i] = (uint8_t *) malloc (sizeof(uint8_t) * columns);
	    if (NULL == matrix[i]){
		printf("error #4 in frirui.c\n");
		for(uint32_t i = 0; i < rows; i++){
		    SAFE_DELETE(matrix[i]);
		}
		SAFE_DELETE(matrix);
		SAFE_DELETE(new_colortable);
		SAFE_DELETE(parity);    
		FAIL(LSTG_E_MALLOC);	
	    }
	}
	//fill the array with the image data
	i = 0;
	uint32_t number_of_pixels = rows * columns;
	while (i < number_of_pixels){
	    for(j = 0; j < rows; j++){
		for (k = 0; k < columns; k++){
		    matrix[j][k] = src_data->img_data[i];
		    i++;
		}
	    }
	}
	//malloc heap for 2x2 blocks
	uint32_t available_x = src_data->size_x / 2 ;
	uint32_t available_y = src_data->size_y / 2 ;
	uint32_t block_count = available_y * available_x;
	BLOCK2X2 *blocks = (BLOCK2X2 *) malloc (sizeof(BLOCK2X2) 
		* block_count);
	
	if((error = frirui_embed_message_to_2X2BLOCKS(matrix, src_data->size_x,
		       src_data->size_y, blocks, block_count, para->password,
		       para->pwlen, parity, message,msglen, new_colortable, 
		       src_data->tbl_size, src_data->table)) != LSTG_OK){
	    SAFE_DELETE(parity);    
	    SAFE_DELETE(new_colortable);
	    FAIL(error);
	}
	//write changed data to structs
	i = 0;
	while(i < number_of_pixels){
	    for(uint32_t j = 0; j < rows; j++){
		for(uint32_t k = 0; k < columns; k++){
		    stego_data->img_data[i] = matrix[j][k];
		    i++;
		}
	    }
	}
	//clean the mess up
	for(uint32_t i = 0; i < rows; i++){
	    free(matrix[i]);
	    matrix[i] = 0;
	}
	SAFE_DELETE(new_colortable);
	free(matrix);
	matrix = 0;
	free(parity);
	parity = 0;
    }
    return LSTG_OK;
}

/**
* Extracts a FriRui-embedded-message from a stegonogram.
* The parity for each colour in the given palette must be calculated.
*                   
*                   0 if R + G + B even   
* f(R + G + B) = 
*                   1 if R + G + B uneven
*
* For each possible pair of colours the distance has to be calculated in the 
* following way
* 
* distance = | (R_1 - R_2) + (G_1 - G_2) + (B_1 - B_2) |
* 
* With this distances triples are formed like that 
*
* (colour1, colour2, distance)
*
* Now the colours in these triples are pushed uinto an new palette by iteration 
* from the lowest to the highest distance.
*
* In this process the parities of the colours get changed, so that the colour
* that gets pushed to the new palette has another parity as the one that already
* is in the new palette. Never change a colour in the new palette.
*
* This process stops when all colours are in the new palette. 
* 
* The pixels to extract the message are choosen. (para->method,PRNG)
*
* For every pixel the parity of the colour in the new palette that arose from
* the viewed one is examined as one bit of the secret message.
*
* @param *stego_data a struct for pixeldata containing the stegonagram
* @param *message returnpointer for the extracted message
* @param msglen the length of the message
* @param *para additional parameters for FriRui including passphrase
* @return an errorcode or 0 if success
*/
uint32_t frirui_extract(const palette_data_t *stego_data, uint8_t **message, uint32_t *msglen, const frirui_parameter *para)
{
    printf("start of frirui extract\n");
    uint32_t error;
    msglen = (uint32_t*) malloc (sizeof(uint32_t));
    //calculate parities
    uint8_t *parity = (uint8_t *) malloc (sizeof(uint8_t) 
			* (stego_data->tbl_size));
    frirui_calc_parity(parity,  stego_data->table, 
		    stego_data->tbl_size);
    //malloc for distances
    uint32_t dist_length = 0;
    for(uint32_t i = stego_data->tbl_size; i > 0; i--){
	dist_length += i - 1;
    }
    DISTANCE *distances = (DISTANCE *)malloc(sizeof(DISTANCE) * dist_length);
    //calculate distances
    if((error = frirui_calc_distances(stego_data->table, stego_data->tbl_size,
		   distances, dist_length)) != LSTG_OK){
	SAFE_DELETE(distances);
	SAFE_DELETE(parity);
	FAIL(error);
    }
    //change parities and build "a new colortable"
    uint8_t *new_colortable = (uint8_t *) malloc (sizeof(uint8_t) 
				* stego_data->tbl_size);
    if((error = frirui_build_colortable(distances, dist_length, new_colortable,
		   stego_data->tbl_size, parity)) != LSTG_OK){
	SAFE_DELETE(new_colortable);
	SAFE_DELETE(distances);
	SAFE_DELETE(parity);
	FAIL(error);
    }
    //distances are not longer needed
    SAFE_DELETE(distances);
    if (0 == para->method){
	printf("...starting extract message from chains\n"); 
	uint32_t number_of_pixels = stego_data->size_x * stego_data->size_y;
	uint32_t num_of_chains = number_of_pixels / para->size; 
	//forget the last pixels that dont fill a whole chain
	CHAIN *chains = (CHAIN *) malloc (sizeof(CHAIN) * num_of_chains);	
	for(uint32_t i = 0; i < num_of_chains; i++){
	    chains[i].pixels = (uint8_t*) malloc (sizeof(uint8_t) * para->size);
	}
	//calc parity of cahins
	uint32_t ptr_img_data = 0;
	uint32_t ptr_chain = 0;
	while(ptr_chain < num_of_chains){
	    uint32_t par_of_chain = 0;
	    for(uint32_t i = 0; i < para->size; i++){
		chains[ptr_chain].pixels[i] = 
		    stego_data->img_data[ptr_img_data];
		par_of_chain = (par_of_chain + 
				    parity[chains[ptr_chain].pixels[i]]);
		ptr_img_data++;
		chains[ptr_chain].parity = par_of_chain & 1;
		if(ptr_img_data % para->size == 0){
		    ptr_chain++;
		}
	    }
	}
	SAFE_DELETE(parity);
	if(0 != para->password || 0 != para->pwlen){
		uint32_t *randomised_index = (uint32_t*) 
		    malloc (num_of_chains * sizeof(uint32_t));
	    if(randomised_index == NULL){
		SAFE_DELETE(randomised_index);
		for(uint32_t i = 0; i < num_of_chains; i++){
		    SAFE_DELETE(chains[i].pixels);
		}
		SAFE_DELETE(chains);
		FAIL(LSTG_E_MALLOC);
	    }
	    prng_state *state = 0;
	    if(1 == random_init(&state,para->password,para->pwlen)){
		SAFE_DELETE(randomised_index);
		for(uint32_t i = 0; i < num_of_chains; i++){
		    SAFE_DELETE(chains[i].pixels);
		}
		SAFE_DELETE(chains);
		FAIL(LSTG_E_MALLOC);
	    }
	    if(1 == randomize_indices(state,num_of_chains,randomised_index)){
		SAFE_DELETE(randomised_index);
		for(uint32_t i = 0; i < num_of_chains; i++){
		    SAFE_DELETE(chains[i].pixels);
		}
		SAFE_DELETE(chains);
		FAIL(LSTG_E_MALLOC);
	    }
	    CHAIN *helper = (CHAIN*) malloc (num_of_chains * sizeof(CHAIN));
	    for(uint32_t i = 0; i < num_of_chains; i++){
		helper[i].parity = chains[randomised_index[i]].parity;
		helper[i].pixels = (uint8_t*) malloc 
		    (para->size * sizeof(uint8_t));
		for(uint32_t j = 0; j < para->size; j++){
			    helper[i].pixels[j] = 
				chains[randomised_index[i]].pixels[j];
		}
	    }
	    SAFE_DELETE(randomised_index);
	    for(uint32_t i = 0; i < num_of_chains; i++){
		chains[i].parity = helper[i].parity;
		for(uint32_t j = 0; j < para->size;j++){
		    chains[i].pixels[j] = helper[i].pixels[j];
		}
		SAFE_DELETE(helper[i].pixels);
	    }
	    SAFE_DELETE(helper);
	}
	for(uint32_t i = 0; i < 32; i++){
	    if(chains[i].parity == 0){
		*msglen <<= 1;
	    } 
	    else
	    if(chains[i].parity == 1){
		*msglen <<= 1;
		*msglen+= 1;
	    }
	}
	printf("...extracted message length = %u \n",*msglen);
	//initiate message
	*message = (uint8_t*)malloc(sizeof(uint8_t) * (*msglen + 1));
	if(*message == NULL){
	    for(uint32_t i = 0; i < num_of_chains; i++){
		SAFE_DELETE(chains[i].pixels);
	    }
	    SAFE_DELETE(chains);
	    FAIL(LSTG_E_MALLOC); 
	}
	int32_t ptr_message_byte = -1;
	for(uint32_t i = 32; i < *msglen * 8 + 32; i++){
	    if(i % 8 == 0){
		ptr_message_byte += 1;
	    }
	    if(chains[i].parity == 0){
		(*message)[ptr_message_byte] <<= 1;
	    } 
	    else
	    if(chains[i].parity == 1){
		(*message)[ptr_message_byte] <<= 1;
		(*message)[ptr_message_byte] += 1;
	    }
	}
	(*message)[*msglen] = '\0';
	printf("...extracted message = %s \n",*message);
	for(uint32_t i = 0; i < num_of_chains; i++){
	    SAFE_DELETE(chains[i].pixels);
	}
	SAFE_DELETE(chains);
	SAFE_DELETE(new_colortable);
    } 
    else 
    if(para->method == 1){
	printf("...starting extract message from 3BLOCKS\n"); 
	//initialize data for an dynamic array
	uint32_t rows = stego_data->size_x;
//	printf("rows = %u \n", rows);
	uint32_t columns = stego_data->size_y;    
//	printf("columns = %u \n", columns);
	uint8_t **matrix = 0;
	uint32_t i,j,k;
	//malloc heap for the array
	matrix = (uint8_t **) malloc (sizeof(uint8_t*) * rows);
	if (0 == matrix) {
//	    printf("error #1 in frirui.c\n");
	    SAFE_DELETE(matrix);
	    SAFE_DELETE(parity);
	    FAIL(LSTG_E_MALLOC);
	}
	for(i = 0; i < rows; i++){
	    matrix[i] = (uint8_t *) malloc (sizeof(uint8_t) * columns);
	    if (0 == matrix[i]){
//		printf("error #2 in frirui.c\n");
		for(i = 0; i < rows; i++){
		    SAFE_DELETE(matrix[i]);
		}
		SAFE_DELETE(matrix);
		SAFE_DELETE(parity);
		FAIL(LSTG_E_MALLOC);	
	    }
	}
	//fill the array with the image data
	i = 0;
	uint32_t number_of_pixels = rows * columns;
//	printf("number_of_pixels %u : rows %u : columns %u \n", 
//		number_of_pixels, rows, columns);
	while (i < number_of_pixels){
	    for(j = 0; j < rows; j++){
		for (k = 0; k < columns; k++){
		    //printf("stego_data->img_data[%u] = %u \n", 
		    //	    i, stego_data->img_data[i]);
		    //printf("matrix[%u][%u] = %u \n",
		    //	    j,k, matrix[j][k]);
		    matrix[j][k] = stego_data->img_data[i];
		    i++;
		}
	    }
	}
	printf("...starting to build 3BLOCKS\n");
	//malloc heap for 3x3 blocks
	uint32_t available_x = stego_data->size_x / 3 ;
	uint32_t available_y = stego_data->size_y / 3 ;
	uint32_t block_count = available_x * available_y;
	BLOCK3X3 *blocks = (BLOCK3X3 *) malloc (sizeof(BLOCK3X3) 
		* block_count);
	if(blocks == 0){
	    SAFE_DELETE(blocks);
	    for(i = 0; i < rows; i++){
		SAFE_DELETE(matrix[i]);
	    }
	    SAFE_DELETE(matrix);
	    SAFE_DELETE(parity);
	    FAIL(LSTG_E_MALLOC);
	}
	i = 0;
	while(i < block_count){
	    for(uint32_t j = 0; j < rows - 2; j+=3){
		for(uint32_t k = 0; k < columns - 2; k+=3){
		    blocks[i].pixels[0] = matrix[j][k];
		    blocks[i].pixels[1] = matrix[j][k+1];
		    blocks[i].pixels[2] = matrix[j][k+2];
		    blocks[i].pixels[3] = matrix[j+1][k];
		    blocks[i].pixels[4] = matrix[j+1][k+1];
		    blocks[i].pixels[5] = matrix[j+1][k+2];
		    blocks[i].pixels[6] = matrix[j+2][k];
		    blocks[i].pixels[7] = matrix[j+2][k+1];
		    blocks[i].pixels[8] = matrix[j+2][k+2];
		    i++;
		}
	    }
	}
	for(i = 0; i < rows; i++){
	    SAFE_DELETE(matrix[i]);
	}
	SAFE_DELETE(matrix);
	i = 0;
	while(i < block_count){
	    for(uint32_t l = 0; l < 9; l++){
		blocks[i].threshold += blocks[i].pixels[l];	
	    }
//	    printf("block %u : threshold %u \n", i, blocks[i].threshold);
	    for(uint32_t l = 0; l < 9; l++){
		blocks[i].parity += parity[blocks[i].pixels[l]];
	    }
	    blocks[i].parity = blocks[i].parity & 1;
	    i++;
	}
	SAFE_DELETE(parity);
	printf("...3BLOCKS build\n");
	if(0 != para->password || 0 != para->pwlen){
	uint32_t *randomised_index = (uint32_t*) 
	    malloc (block_count * sizeof(uint32_t));
	if(randomised_index == NULL){
	    SAFE_DELETE(randomised_index);
	    SAFE_DELETE(blocks);
	    FAIL(LSTG_E_MALLOC);
	}
	prng_state *state = 0;
	if(1 == random_init(&state,para->password,para->pwlen)){
	    SAFE_DELETE(randomised_index);
	    SAFE_DELETE(blocks);
	    FAIL(LSTG_E_MALLOC);
	}
	if(1 == randomize_indices(state,block_count,randomised_index)){
	    SAFE_DELETE(randomised_index);
	    SAFE_DELETE(blocks);
	    FAIL(LSTG_E_MALLOC);
	}
	BLOCK3X3 *helper = (BLOCK3X3*) malloc (block_count * sizeof(BLOCK3X3));
	for(uint32_t i = 0; i < block_count; i++){
	    helper[i].parity = blocks[randomised_index[i]].parity;
	    helper[i].threshold = blocks[randomised_index[i]].threshold;
	    for(uint32_t j = 0; j < 9; j++){
		helper[i].pixels[j] = 
		    blocks[randomised_index[i]].pixels[j];
		helper[i].position_x[j] = 
		    blocks[randomised_index[i]].position_x[j];		
		helper[i].position_y[j] = 
		    blocks[randomised_index[i]].position_y[j];
	    }
	}
	SAFE_DELETE(randomised_index);
	for(uint32_t i = 0; i < block_count; i++){
	    blocks[i].parity = helper[i].parity;
	    blocks[i].threshold = helper[i].threshold;
	    for(uint32_t j = 0; j < 9;j++){
		blocks[i].pixels[j] = helper[i].pixels[j];
		blocks[i].position_x[j] = helper[i].position_x[j];
		blocks[i].position_y[j] = helper[i].position_y[j];
	    }
	}
	SAFE_DELETE(helper);
	}
	j = 0;
	*msglen = 0;
//	printf("block_count = %u \n", block_count);
	for(i = 0; i < 32; i++){
//	    printf("msglen[%u] \n", i);
//	    printf(" %u : %u \n", j, block_count);
	    while(j < block_count){
//  		printf("block %u : threshold %u \n", j, blocks[j].threshold);
		if(blocks[j].threshold <= para->threshold){
		    if(blocks[j].parity == 0){
			*msglen <<= 1;
			printf("block %u : bit = 0 \n",j);
			j++;
			break;
		    } else {
			*msglen <<= 1;
			*msglen += 1;
			printf("block %u : bit = 1 \n", j);
			j++;
			break;
		    }
		} else {
		    printf("block %u threshold to high \n", j);
		}
		j++;
	    }
	}
	printf("...extracted message length %u \n", *msglen);
	//initiate message
	*message = (uint8_t*)malloc(sizeof(uint8_t) * (*msglen));
	if(*message == NULL){
	    SAFE_DELETE(blocks);
	    FAIL(LSTG_E_MALLOC); 
	}
	for(uint32_t i = 0; i < *msglen; i++){
	    (*message)[i] = 0;
	}
	int32_t ptr_message_byte = -1;
	uint32_t offset = j;
	uint32_t l = 0;
	for(k = 0; k < *msglen * 8 + offset; k++){
	    if(blocks[j].threshold <= para->threshold){
		if(l % 8 == 0){
		    ptr_message_byte += 1;
		    printf("l = %u , pointer changed to %i \n", l, ptr_message_byte);
		}
		if(blocks[j].parity == 0){
		    (*message)[ptr_message_byte] <<= 1;
		    printf("block %u : bit = 0 \n",j);
		    j++;
		    l++;
		} else 
		if(blocks[j].parity == 1){
		    (*message)[ptr_message_byte] <<= 1;
		    (*message)[ptr_message_byte] += 1;
		    printf("block %u : bit = 1 \n",j);
		    j++;
		    l++;
		}
		if(j == *msglen * 8 + offset|| ptr_message_byte > (int32_t)*msglen){
		    break;
		}
	    } else {
		printf("block %u treshold to high \n", j);
		j++;
		offset++;
	    }
	}
	(*message)[*msglen] = '\0';
	for(uint32_t i = 0; i < *msglen; i++){
	    printf("message[%u] = %u \n", i, (*message)[i]);
	}
	printf("...extracted message = %s \n", *message);
	SAFE_DELETE(blocks);
	SAFE_DELETE(new_colortable);
    } 
    else
    if(para->method == 2){
	printf("...starting extract message from 2BLOCKS\n"); 
	uint32_t rows = stego_data->size_x;
	uint32_t columns = stego_data->size_y;    
	uint8_t **matrix = 0;
	uint32_t i,j,k = 0;
	//malloc heap for the array
	matrix = (uint8_t **) malloc (sizeof(uint8_t *) * rows);
	if (NULL == matrix) {
	    SAFE_DELETE(matrix);
	    SAFE_DELETE(parity);
	    printf("error #3 in frirui.c\n");
	    FAIL(LSTG_E_MALLOC);
	}
	for (i = 0; i < rows; i++){
	    matrix[i] = (uint8_t *) malloc (sizeof(uint8_t) * columns);
	    if (NULL == matrix[i]){
		for(i = 0; i < rows; i++){
		    SAFE_DELETE(matrix[i]);
		}
		SAFE_DELETE(matrix);
		SAFE_DELETE(parity);
		printf("error #4 in frirui.c\n");
		FAIL(LSTG_E_MALLOC);	
	    }
	}
	//fill the array with the image data
	i = 0;
	uint32_t number_of_pixels = rows * columns;
	while (i < number_of_pixels){
	    for(j = 0; j < rows; j++){
		for (k = 0; k < columns; k++){
		    matrix[j][k] = stego_data->img_data[i];
		    i++;
		}
	    }
	}
	//malloc heap for 2x2 blocks
	printf("...starting to build 2BLOCKS\n");
	uint32_t available_x = stego_data->size_x / 2 ;
	uint32_t available_y = stego_data->size_y / 2 ;
	uint32_t block_count = available_x * available_y;
	BLOCK2X2 *blocks = (BLOCK2X2 *) malloc (sizeof(BLOCK2X2) 
				    * block_count);
	if(blocks == 0){
	    SAFE_DELETE(blocks);
	    for(i = 0; i < rows; i++){
		SAFE_DELETE(matrix[i]);
	    }
	    SAFE_DELETE(matrix);
	    SAFE_DELETE(parity);
	    FAIL(LSTG_E_MALLOC);
	}
	//fill 2x2 blocks
	i = 0;
	while(i < block_count){
	    for(j = 0; j < rows - 1; j+=2){
		for(k = 0; k < columns - 1; k+=2){
		    blocks[i].pixels[0] = matrix[j][k];
		    blocks[i].pixels[1] = matrix[j][k+1];
		    blocks[i].pixels[2] = matrix[j+1][k];
		    blocks[i].pixels[3] = matrix[j+1][k+1];
		    i++;
		}
	    }
	}
	for(i = 0; i < rows; i++){
	    SAFE_DELETE(matrix[i]);
	}
	SAFE_DELETE(matrix);
	i = 0;
    	while(i < block_count){
	    blocks[i].parity = 0;
	    for(k = 0; k < 4; k++){
		    blocks[i].parity += parity[blocks[i].pixels[k]];
	    }
	    blocks[i].parity = blocks[i].parity & 1;
 	    i++;
	}
	SAFE_DELETE(parity);
	printf("...2BLOCKS build \n");
	if(0 != para->password || 0 != para->pwlen){
	uint32_t *randomised_index = (uint32_t*) 
	    malloc (block_count * sizeof(uint32_t));
	if(randomised_index == NULL){
	    SAFE_DELETE(randomised_index);
	    SAFE_DELETE(blocks);
	    FAIL(LSTG_E_MALLOC);
	}
	prng_state *state = 0;
	if(1 == random_init(&state,para->password,para->pwlen)){
	    SAFE_DELETE(randomised_index);
	    SAFE_DELETE(blocks);
	    FAIL(LSTG_E_MALLOC);
	}
	if(1 == randomize_indices(state,block_count,randomised_index)){
	    SAFE_DELETE(randomised_index);
	    SAFE_DELETE(blocks);
	    FAIL(LSTG_E_MALLOC);
	}
	BLOCK2X2 *helper = (BLOCK2X2*) malloc (block_count * sizeof(BLOCK2X2));
	for(uint32_t i = 0; i < block_count; i++){
	    helper[i].parity = blocks[randomised_index[i]].parity;
	    for(uint32_t j = 0; j < 4; j++){
		helper[i].pixels[j] = blocks[randomised_index[i]].pixels[j];
		helper[i].position_x[j] = blocks[randomised_index[i]].position_x[j];		
		helper[i].position_y[j] = blocks[randomised_index[i]].position_y[j];
	    }
	}
	SAFE_DELETE(randomised_index);
	for(uint32_t i = 0; i < block_count; i++){
	    blocks[i].parity = helper[i].parity;
	    for(uint32_t j = 0; j < 4;j++){
		blocks[i].pixels[j] = helper[i].pixels[j];
		blocks[i].position_x[j] = helper[i].position_x[j];
		blocks[i].position_y[j] = helper[i].position_y[j];
	    }
	}
	SAFE_DELETE(helper);
	}
	//find usable 2x2 blocks
	i = 0;
	j = 0;
	k = 0;
	uint32_t *usable_blocks = 0;
	uint32_t *not_usable_blocks = 0;
	while(i < block_count){
	    // if block does not contain 3 different colors
	    if((blocks[i].pixels[0] == blocks[i].pixels[1] 
			&& blocks[i].pixels[2] == blocks[i].pixels[3])
	    || (blocks[i].pixels[0] == blocks[i].pixels[2] 
			&& blocks[i].pixels[1] == blocks[i].pixels[3])
	    || (blocks[i].pixels[0] == blocks[i].pixels[3] 
			&& blocks[i].pixels[1] == blocks[i].pixels[2])){
		j += 1;
		uint32_t *helper = ALLOCN(uint32_t, j);
		if(not_usable_blocks != 0){
		    for (uint32_t l = 0; l < j - 1; l++){
			helper[l] = not_usable_blocks[l];
		    }	
		    SAFE_DELETE(not_usable_blocks);
		    not_usable_blocks = ALLOCN(uint32_t, j);
		    for(uint32_t l = 0; l < j - 1; l++){
			not_usable_blocks[l] = helper[l];
		    }
		    not_usable_blocks[j-1] = i; 
		} else
		    if(not_usable_blocks == 0){
			not_usable_blocks = ALLOC(uint32_t);
			not_usable_blocks[0] = i;
		    }
		SAFE_DELETE(helper);
    	    } else {
		//if block contains 3 different colors
		k += 1;
		uint32_t *helper = ALLOCN(uint32_t, k);
		if(usable_blocks != 0){
		    for (uint32_t l = 0; l < k -1; l++){
			helper[l] = usable_blocks[l];
		    }
		    SAFE_DELETE(usable_blocks);
		    usable_blocks = ALLOCN(uint32_t, k);
		    for(uint32_t l = 0; l < k - 1; l++){
		    usable_blocks[l] = helper[l];
		    } 
		    usable_blocks[k-1] = i;
		} else
		    if(usable_blocks == 0){
			usable_blocks = ALLOC(uint32_t);
			usable_blocks[0] = i;
		}
		SAFE_DELETE(helper);
	    }
	    i++;
	}
	uint32_t not_usable_block_count = j;
	printf("...(not) usable blocks found \n");
	//find usable colors
	uint32_t *not_usable_colors = 0;
	uint32_t not_usable_length = 0;
//	printf("tbl_size = %u \n", stego_data->tbl_size);
//	printf("block_count = %u \n", block_count);
	for(i = 0; i < stego_data->tbl_size; i++){
	    for(k = 0; k < not_usable_block_count; k ++){
		if(new_colortable[i] == blocks[not_usable_blocks[k]].pixels[0]
		|| new_colortable[i] == blocks[not_usable_blocks[k]].pixels[1]
		|| new_colortable[i] == blocks[not_usable_blocks[k]].pixels[2]
		|| new_colortable[i] == blocks[not_usable_blocks[k]].pixels[3]){
		    //farbe ungeeignet
		    not_usable_length += 1;
		    uint32_t *helper = ALLOCN(uint32_t, not_usable_length);
		    if(not_usable_colors != 0){
			for (uint32_t l = 0; l < not_usable_length - 1; l++){
			    helper[l] = not_usable_colors[l];
			}	
			SAFE_DELETE(not_usable_colors);
			not_usable_colors = ALLOCN(uint32_t, not_usable_length);
			for(uint32_t l = 0; l < not_usable_length - 1; l++){
			    not_usable_colors[l] = helper[l];
			}
			not_usable_colors[j-1] = i; 
		    } else 
			if(not_usable_colors == 0){
			    not_usable_colors = ALLOC(uint32_t);
			    not_usable_colors[0] = i;
		    }
		    SAFE_DELETE(helper);
		    break;
		}
	    }
	}
	SAFE_DELETE(not_usable_blocks);
	uint32_t *usable_colors = 0;
	uint32_t usable_length = 0;
	//find usable colors
	for(i = 0; i < stego_data->tbl_size; i++){
	    uint32_t included = 0;
	    for(k = 0; k < not_usable_length; k++){
	       if(new_colortable[i] == not_usable_colors[k]){
		    included = 1;
		    break;
	       }
	    }
	    if(included == 0){
		usable_length += 1;
		uint32_t *helper = ALLOCN(uint32_t, usable_length);
		for (uint32_t l = 0; l < usable_length - 1; l++){
		    helper[l] = usable_colors[l];
		}	
		SAFE_DELETE(usable_colors);
		usable_colors = ALLOCN(uint32_t, usable_length);
		for(uint32_t l = 0; l < usable_length - 1; l++){
		    usable_colors[l] = helper[l];
		}
		usable_colors[usable_length - 1] = i;
	    }
	}
	SAFE_DELETE(not_usable_colors);
	printf("...(not) usable colors found \n");
	//extract message length
	i = 0;
	j = 0;
	*msglen = 0;
	for(i = 0; i < 32; i++){
/*
	    for(uint32_t x = 0; x < 4; x++){
		printf("pixel %u color %u parity %u \n", x, blocks[usable_blocks[j]].pixels[x], parity[blocks[usable_blocks[j]].pixels[x]]);
	    }
*/
	    while(j < usable_length){
		if(blocks[usable_blocks[j]].parity == 0){
		    *msglen <<= 1;
		    printf("block %u : bit = 0 \n", j);
		    j++;
		    break;
		} else {
		    *msglen <<= 1;
		    *msglen += 1;
		    printf("block %u : bit = 1 \n", j);
		    j++;
		    break;
		}
	    }
	}
	printf("...extracted message length %u \n", *msglen);
	//initiate message
	*message = (uint8_t*)malloc(sizeof(uint8_t) * (*msglen));
	if(*message == NULL){
	    SAFE_DELETE(message);
	    SAFE_DELETE(usable_blocks);
	    SAFE_DELETE(blocks);
	    FAIL(LSTG_E_MALLOC);
	}
	int32_t ptr_message_byte = -1;
	uint32_t offset = j;
	for(k = 0 ; k < *msglen * 8 + offset; k++){
	    if(j % 8 == 0){
		ptr_message_byte += 1;
		printf("j = %u, pointer changed to %i \n", j, ptr_message_byte);
	    }
	    if(blocks[usable_blocks[j]].parity == 0){
		(*message)[ptr_message_byte] <<= 1;
		printf("block %u : bit = 0 \n", j);
		j++;
	    }
	    else if(blocks[usable_blocks[j]].parity == 1){
		(*message)[ptr_message_byte] <<= 1;
		(*message)[ptr_message_byte] += 1;
		printf("block %u : bit = 1 \n", j);
		j++;
	    }
	    if(j == *msglen * 8 + offset || ptr_message_byte >= *msglen){
		break;
	    }
	}
	(*message)[*msglen] = '\0';
	for(uint32_t i = 0; i < *msglen; i++){
	    printf("message[%u] = %u \n", i, (*message)[i]);
	}
	printf("...extracted message = %s \n", *message);
	//clean up
	SAFE_DELETE(blocks);
	SAFE_DELETE(usable_blocks);
	SAFE_DELETE(usable_colors);
	SAFE_DELETE(new_colortable);
    }	
    return LSTG_OK;
}


/**
* Always returns LSTG_OK, cause the function is integrated to the extract 
* function
* @param *stego_data a struct for palettedata containing the stegonagram
* @param *para additional parameters for FriRui including passphrase
* @return length of embedded message
*/
uint32_t frirui_get_message_length(const palette_data_t *stego_data, const frirui_parameter *para)
{
    return LSTG_OK;
}

/**
* Returns the capacity of a cover image
* The capacity is depending on the process of choosing the pixels to embed, so
* this function must call other functions for capacity check.
* @param *src_data a struct for palettedata to provide the original image
* @param *para additional parameters for FriRui including passphrase
* @return capacity of cover image
*/
uint32_t frirui_check_capacity(const palette_data_t *src_data, uint32_t *capacity, const frirui_parameter *para)
{
    *capacity = 0;
    uint32_t number_of_pixels = src_data->size_x * src_data->size_y;
    if (0 == para->method){
	if (2 > para->size){
	    FAIL(LSTG_E_INVALIDPARAM); // parameter fault : chains must be greater or equal 2
	} else {
	    uint32_t length_of_chain = para->size;
	    *capacity = number_of_pixels / length_of_chain;
	    return LSTG_OK;
	}
    } else if (1 == para->method){
	uint32_t rows = src_data->size_x;
	uint32_t columns = src_data->size_y;    
	uint8_t **matrix = 0;
	uint32_t i,j,k = 0;
	matrix = (uint8_t **) malloc (sizeof(uint8_t*) * rows);
	if (0 == matrix) {
	FAIL(LSTG_E_MALLOC);
	}
	for (uint32_t i = 0; i < rows; i++){
	    matrix[i] = (uint8_t *) malloc (sizeof(uint8_t) * columns);
	    if (0 == matrix[i]){
		FAIL(LSTG_E_MALLOC);	
	    }
	}
	i = 0;
	for(j = 0; j < rows; j++){
	    for(k = 0; k < columns; k++){
		if(i > rows * columns){
		    FAIL(LSTG_E_MALLOC);
		}
		matrix[j][k] = src_data->img_data[i];
		i++;
	    }
	}
	uint32_t available_x = src_data->size_x / 3 ;
	uint32_t available_y = src_data->size_y / 3 ;
	uint32_t block_count = available_y * available_x;
	BLOCK3X3 *blocks = (BLOCK3X3 *) malloc (sizeof(BLOCK3X3) * block_count);
	i = 0;
	while(i < block_count){
	    for(uint32_t j = 0; j < rows - 2; j+=3){
		for(uint32_t k = 0; k < columns - 2; k+=3){
		    blocks[i].pixels[0] = matrix[j][k];
		    blocks[i].position_x[0] = j;
		    blocks[i].position_y[0] = k;
		    blocks[i].pixels[1] = matrix[j][k+1];
		    blocks[i].position_x[1] = j;
		    blocks[i].position_y[1] = k+1; 
		    blocks[i].pixels[2] = matrix[j][k+2];
		    blocks[i].position_x[2] = j;
		    blocks[i].position_y[2] = k+2;
		    blocks[i].pixels[3] = matrix[j+1][k];
		    blocks[i].position_x[3] = j+1;
		    blocks[i].position_y[3] = k;
		    blocks[i].pixels[4] = matrix[j+1][k+1];
		    blocks[i].position_x[4] = j+1;
		    blocks[i].position_y[4] = k+1;
		    blocks[i].pixels[5] = matrix[j+1][k+2];
		    blocks[i].position_x[5] = j+1;
		    blocks[i].position_y[5] = k+2;
		    blocks[i].pixels[6] = matrix[j+2][k];
		    blocks[i].position_x[6] = j+2;
		    blocks[i].position_y[6] = k;
		    blocks[i].pixels[7] = matrix[j+2][k+1];
		    blocks[i].position_x[7] = j+2;
		    blocks[i].position_y[7] = k+1;
		    blocks[i].pixels[8] = matrix[j+2][k+2];
		    blocks[i].position_x[8] = j+2;
		    blocks[i].position_y[8] = k+2;
		    i++;
		}
	    }
	}
	i = 0;
	uint32_t usable_blocks = 0;
	while(i < block_count){
	    for(uint32_t l = 0; l < 9; l++){
		blocks[i].threshold += blocks[i].pixels[l];
	    }
	    if(blocks[i].threshold <= para->threshold){
		usable_blocks += 1;
	    }
	    i++;
	}
	*capacity = usable_blocks;
	return LSTG_OK;
    } else if (2 == para->method){
	//matrix bauen
	uint32_t rows = src_data->size_x;
	uint32_t columns = src_data->size_y;    
	uint32_t i,j,k = 0;
	uint8_t **matrix = 0;
	matrix = (uint8_t **) malloc (sizeof(uint8_t*) * rows);
	if (0 == matrix) {
	printf("error Z in frirui.c\n");
	FAIL(LSTG_E_MALLOC);
	}
	for(uint32_t i = 0; i < rows; i++){
	    matrix[i] = (uint8_t *) malloc (sizeof(uint8_t) * columns);
	    if (0 == matrix[i]){
		printf("error Y in frirui.c\n");
		FAIL(LSTG_E_MALLOC);	
	    }
	}
	for(j = 0; j < rows; j++){
	    for (k = 0; k < columns; k++){
		matrix[j][k] = src_data->img_data[i];
		i++;
	    }
	}
	//2BLOCKS bauen
	uint32_t available_x = src_data->size_x / 2 ;
	uint32_t available_y = src_data->size_y / 2 ;
	uint32_t block_count = available_y * available_x;
	BLOCK2X2 *blocks = (BLOCK2X2 *) malloc (sizeof(BLOCK2X2) 
		* block_count);
	i = 0;
	while(i < block_count){
	    for(uint32_t j = 0; j < rows - 1; j+=2){
		for(uint32_t k = 0; k < columns - 1; k+=2){
		    blocks[i].pixels[0] = matrix[j][k];
		    blocks[i].position_x[0] = j;
		    blocks[i].position_y[0] = k;
		    blocks[i].pixels[1] = matrix[j][k+1];
		    blocks[i].position_x[1] = j;
		    blocks[i].position_y[1] = k + 1;
		    blocks[i].pixels[2] = matrix[j+1][k];
		    blocks[i].position_x[2] = j + 1;
		    blocks[i].position_y[2] = k;
		    blocks[i].pixels[3] = matrix[j+1][k+1];
		    blocks[i].position_x[3] = j + 1;
		    blocks[i].position_y[3] = k + 1;
		    i++;
		}
	    }
	}
	//find usable 2x2 blocks
	i = 0;
	uint32_t usable_blocks = 0;
	while(i < block_count){
	    // if block does not contain 3 different colors
	    if(0 == (blocks[i].pixels[0] == blocks[i].pixels[1] 
			&& blocks[i].pixels[2] == blocks[i].pixels[3])
	    || (blocks[i].pixels[0] == blocks[i].pixels[2] 
			&& blocks[i].pixels[1] == blocks[i].pixels[3])
	    || (blocks[i].pixels[0] == blocks[i].pixels[3] 
			&& blocks[i].pixels[1] == blocks[i].pixels[2])){
	    usable_blocks++;
	    }
	    i++;
	}
	*capacity = usable_blocks;
	return LSTG_OK;
    } else {
	FAIL(LSTG_ERROR); // capacity can not be calculated
    }
}
