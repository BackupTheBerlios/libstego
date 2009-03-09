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
 
 
 
#include "libstego/svg.h"
#include "libstego/errors.h"
#include <string.h>

/**
* Function to read a SVG-file from HDD and return a svg_data_t
*
* @param *filename the path and filename of the svg-file
* @param *data a struct to return the audio samples
* @param *int_dat a struct provided by the svg-library to hold the original svg-file
* @return LSTG_OK, or LSTG_ERROR when something failed.
*/
uint32_t io_svg_read(
        const uint8_t *filename,
        svg_data_t *data,
        svg_internal_data_t *int_dat) {
    xmlNodePtr cur = 0, child = 0;
    xmlXPathContextPtr xpathCtx;
    xmlXPathObjectPtr xpathObj;
    uint32_t i = 0;
    const uint8_t *xpathExpr = "//@gradientTransform";

    // Load XML document
    int_dat->doc = xmlParseFile(filename);
    if (int_dat->doc == NULL) {
		FAIL(LSTG_E_OPENFAILED)
    }

    // Create xpath evaluation context
    xpathCtx = xmlXPathNewContext(int_dat->doc);
    if(xpathCtx == NULL) {
        xmlFreeDoc(int_dat->doc);
        int_dat->doc= 0;
    	FAIL(LSTG_E_XPATH_CONT)
    }

    // Evaluate xpath expression
    xpathObj = xmlXPathEvalExpression(xpathExpr, xpathCtx);
    if(xpathObj == NULL) {
        xmlXPathFreeContext(xpathCtx);
        xpathCtx = 0;
        xmlFreeDoc(int_dat->doc);
        int_dat->doc = 0;
        FAIL(LSTG_E_XPATH_EXPR)
    }

    data->num_attribs = (xpathObj->nodesetval) ? xpathObj->nodesetval->nodeNr : 0;
    if (data->num_attribs > 0) {
        // allocate enough memory to hold all the attributes
        data->attributes = (svg_attrib_t*) malloc(sizeof(svg_attrib_t) * data->num_attribs);
        if (data->attributes == NULL) {
            xmlXPathFreeContext(xpathCtx);
            xpathCtx = 0;
            xmlXPathFreeObject(xpathObj);
            xpathObj = 0;
            xmlFreeDoc(int_dat->doc);
            int_dat->doc = 0;
            FAIL(LSTG_E_MALLOC);
        }

        // copy the values of the found attributes into our own structure
        for (i = 0; i < data->num_attribs; ++i) {
            // get current xmlNode
            cur = xpathObj->nodesetval->nodeTab[i];

            /* xmlNodes returned by the above xpathExpr are attributes, which
             * have exactly one child, namely the node holding the content of
             * the attribute (the part between the hyphens, e.g. fubar in
             * <foo bar="fubar" />)
             */

            // allocate enough memory to hold the attributes value in
            data->attributes[i].data = (uint8_t*) malloc(sizeof(uint8_t) * (strlen(cur->children->content)+1));
            if (data->attributes[i].data == NULL) {
                SAFE_DELETE(data->attributes);
                xmlXPathFreeContext(xpathCtx);
                xpathCtx = 0;
                xmlXPathFreeObject(xpathObj);
                xpathObj = 0;
                xmlFreeDoc(int_dat->doc);
                int_dat->doc = 0;
                FAIL(LSTG_E_MALLOC);
            }
            // copy the attributes value
            memcpy(data->attributes[i].data, cur->children->content, strlen(cur->children->content)+1);
            data->attributes[i].node = cur;
        }
    }

    xmlXPathFreeContext(xpathCtx);
    xmlXPathFreeObject(xpathObj);

    return LSTG_OK;
}

/**
* Function to integrate a changed svg_data_t into a svg_internal_data_t
*
* @param *int_dat a struct provided by the SVG-library to hold the original image
* @param *data a struct to give the changed svgdata (steganogram)
* @return LSTG_OK, or LSTG_ERROR when something failed.
*/
uint32_t io_svg_integrate(
        svg_internal_data_t *int_dat,
        const svg_data_t *data) {
    uint32_t i = 0;
    xmlNodePtr node = 0;
    int length = 0;

    for (i = 0; i < data->num_attribs; ++i) {
        node = (xmlNodePtr)data->attributes[i].node;
        // free the space of the nodes current content
        free(node->children->content);
        // allocate space for the new content
        length = strlen(data->attributes[i].data) + 1;
        node->children->content = (xmlChar*) malloc(sizeof(xmlChar) * length);
        if (node->children->content == NULL) {
            FAIL(LSTG_E_MALLOC);
        }
        // copy the content
        memcpy(node->children->content, data->attributes[i].data, length);
    }

    return LSTG_OK;
}

/**
* Function to write a svg_internal_data_t to HDD by using the SVG-library
*
* @param *filename the path and filename of the svg-file
* @param *int_dat a struct provided by the SVG-library to hold the original image
* @return LSTG_OK, or LSTG_ERROR when something failed.
*/
uint32_t io_svg_write(
        const uint8_t *filename,
        svg_internal_data_t *int_dat){
    xmlSaveCtxtPtr context = xmlSaveToFilename(filename, 0, 0);

    if (context == NULL) {
        FAIL(LSTG_E_OPENFAILED);
    }

    if (xmlSaveDoc(context, int_dat->doc) == -1) {
        FAIL(LSTG_E_WRITEFAILED);
    }

    if (xmlSaveClose(context) == -1) {
        FAIL(LSTG_E_WRITEFAILED);
    }

    return LSTG_OK;
}

/**
 * Performs the necessary cleanup operations (if any) for svg_data_t's that
 * are no longer used. This function _has_ to be called for every svg_data_t
 * instance.
 *
 * @param data The data structure that is to be cleaned up.
 * @return LSTG_OK, or LSTG_ERROR when something failed.
 */
uint32_t io_svg_cleanup_data(svg_data_t *data) {

    uint32_t i = 0;

    if (data == NULL) {
        // nothing to be done
        return LSTG_OK;
    }

    for (i = 0; i < data->num_attribs; ++i) {
        SAFE_DELETE(data->attributes[i].data);

        // the xmlNode this poins to is free'd by the libxml functions
        data->attributes[i].node = 0;
    }

    SAFE_DELETE(data->attributes);
    data->num_attribs = 0;

    return LSTG_OK;
}

/**
 * Performs the necessary cleanup operations (if any) for the SVG-library's
 * internal data structure. This function _has_ to be called for every instance
 * of such a structure.
 *
 * @param int_dat The internal data structure of the SVG-library.
 * @return LSTG_OK, or LSTG_ERROR when something failed.
 */
uint32_t io_svg_cleanup_internal(svg_internal_data_t *int_dat) {

    if (int_dat == NULL) {
        // nothing to do
        return LSTG_OK;
    }

    if (int_dat->doc != NULL) {
        xmlFreeDoc(int_dat->doc);
        int_dat->doc = NULL;
    }

    return LSTG_OK;
}
