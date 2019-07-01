/* Copyright (c) 2009-2014, Erik Lindahl & David van der Spoel
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 * this
 * list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 * this list of conditions and the following disclaimer in the documentation
 * and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include <math.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "xdrfile.h"

static void _die(char* msg, int line, char* file) {
    fprintf(stderr, "Fatal error: %s\n", msg);
    fprintf(stderr, "Death occurred at %s, line %d\n", file, line);
    exit(1);
}

#define die(msg) _die(msg, __LINE__, __FILE__)

#define EPSILON_1 1e-7f
#define EPSILON_2 1e-4
#define BUFLEN 37
#define NPREC 4

int main(void) {
    XDRFILE* xfp;
    int len, ncoord = BUFLEN / 3;
    char ptr[BUFLEN], *buf = "abcdefghijklmnopqrstuvwxyz";
    char* testfn = "test.xdr";
    unsigned char uptr[BUFLEN];
    short sptr[BUFLEN], sptr2[BUFLEN];
    unsigned short usptr[BUFLEN], usptr2[BUFLEN];
    int iptr[BUFLEN], iptr2[BUFLEN];
    unsigned int uiptr[BUFLEN], uiptr2[BUFLEN];
    float fptr[BUFLEN], fptr2[BUFLEN];
    double dptr[BUFLEN], dptr2[BUFLEN];
    char optr[BUFLEN], optr2[BUFLEN];

    float fprec[NPREC] = {234.45f, 1000.0f, 1e5f, 1.0f};
    double dprec[NPREC] = {234.45, 1000.0, 1e5, 1.0};

    // Can not write a string that's on the stack since all data is
    // treated as variables.
    len = (int)strlen(buf) + 1;
    if (len >= BUFLEN) {
        die("Increase BUFLEN");
    }
    strcpy(ptr, buf);
    strcpy((char*)uptr, buf);
    /* Initiate float arrays */
    for (int i = 0; i < BUFLEN; i++) {
        fptr[i] = cosf(i * 13.0f / (float)M_PI);
        dptr[i] = sin(i * 13.0 / M_PI);
    }
    /* Initiate opaque array */
    memcpy(optr, dptr, BUFLEN);

    /*************************************/
    /*           WRITING BIT             */
    /*************************************/

    if ((xfp = xdrfile_open("test.xdr", "w")) == NULL) {
        die("Can not open file for writing");
    }

    if (xdrfile_write_char(ptr, len, xfp) != len) {
        die("Writing char string");
    }
    if (xdrfile_write_uchar(uptr, len, xfp) != len) {
        die("Writing uchar string");
    }
    if (xdrfile_write_short(sptr, BUFLEN, xfp) != BUFLEN) {
        die("Writing short array");
    }
    if (xdrfile_write_ushort(usptr, BUFLEN, xfp) != BUFLEN) {
        die("Writing ushort array");
    }
    if (xdrfile_write_int(iptr, BUFLEN, xfp) != BUFLEN) {
        die("Writing int array");
    }
    if (xdrfile_write_uint(uiptr, BUFLEN, xfp) != BUFLEN) {
        die("Writing uint array");
    }
    if (xdrfile_write_float(fptr, BUFLEN, xfp) != BUFLEN) {
        die("Writing float array");
    }
    if (xdrfile_write_double(dptr, BUFLEN, xfp) != BUFLEN) {
        die("Writing double array");
    }
    if (xdrfile_write_string(buf, xfp) != len) {
        die("Writing string");
    }
    if (xdrfile_write_opaque(optr, BUFLEN, xfp) != BUFLEN) {
        die("Writing opaque");
    }
    for (int k = 0; k < NPREC; k++) {
        if (xdrfile_compress_coord_float(fptr, ncoord, fprec[k], xfp) != ncoord) {
            die("Writing compress_coord_float");
        }
        if (xdrfile_compress_coord_double(dptr, ncoord, dprec[k], xfp) != ncoord) {
            die("Writing compress_coord_double");
        }
    }
    if (xdrfile_close(xfp) != 0) {
        die("Can not close xdr file");
    }

    /*************************************/
    /*          READING BIT              */
    /*************************************/
    if ((xfp = xdrfile_open(testfn, "r")) == NULL) {
        die("Can not open file for reading");
    }

    if ((xdrfile_read_char(ptr, len, xfp)) != len) {
        die("Not the right number of chars read from string");
    }
    if (strcmp(ptr, buf) != 0) {
        die("did not read the expected chars");
    }
    if (xdrfile_read_uchar(uptr, len, xfp) != len) {
        die("Not the right number of uchars read from string");
    }
    if (strcmp((char*)uptr, buf) != 0) {
        die("did not read the expected uchars");
    }
    if (xdrfile_read_short(sptr2, BUFLEN, xfp) != BUFLEN) {
        die("Reading short array");
    }
    for (int i = 0; i < BUFLEN; i++) {
        if (sptr2[i] != sptr[i]) {
            fprintf(stderr, "i: %5d, wrote: %10d, read: %10d\n", i, sptr[i], sptr2[i]);
            die("Comparing short array");
        }
    }
    if (xdrfile_read_ushort(usptr2, BUFLEN, xfp) != BUFLEN) {
        die("Reading ushort array");
    }
    for (int i = 0; i < BUFLEN; i++) {
        if (usptr2[i] != usptr[i]) {
            fprintf(stderr, "i: %5d, wrote: %10d, read: %10d\n", i, usptr[i], usptr2[i]);
            die("Comparing ushort array");
        }
    }
    if (xdrfile_read_int(iptr2, BUFLEN, xfp) != BUFLEN) {
        die("Reading int array");
    }
    for (int i = 0; i < BUFLEN; i++) {
        if (iptr2[i] != iptr[i]) {
            fprintf(stderr, "i: %5d, wrote: %10d, read: %10d\n", i, iptr[i], iptr2[i]);
            die("Comparing int array");
        }
    }
    if (xdrfile_read_uint(uiptr2, BUFLEN, xfp) != BUFLEN) {
        die("Reading uint array");
    }
    for (int i = 0; i < BUFLEN; i++) {
        if (uiptr2[i] != uiptr[i]) {
            fprintf(stderr, "i: %5d, wrote: %10d, read: %10d\n", i, uiptr[i], uiptr2[i]);
            die("Comparing uint array");
        }
    }
    if (xdrfile_read_float(fptr2, BUFLEN, xfp) != BUFLEN) {
        die("Reading float array");
    }
    for (int i = 0; i < BUFLEN; i++) {
        if (fptr2[i] != fptr[i]) {
            fprintf(stderr, "i: %5d, wrote: %12g, read: %12g\n", i, fptr[i], fptr2[i]);
            die("Comparing float array");
        }
    }
    if (xdrfile_read_double(dptr2, BUFLEN, xfp) != BUFLEN) {
        die("Reading double array");
    }
    for (int i = 0; i < BUFLEN; i++) {
        if (dptr2[i] != dptr[i]) {
            fprintf(stderr, "i: %5d, wrote: %12g, read: %12g\n", i, dptr[i], dptr2[i]);
            die("Comparing double array");
        }
    }
    if (xdrfile_read_string(ptr, BUFLEN, xfp) != len) {
        die("Reading string");
    }
    if (strcmp(ptr, buf) != 0) {
        die("Comparing strings");
    }
    if (xdrfile_read_opaque(optr2, BUFLEN, xfp) != BUFLEN) {
        die("Reading opaque array");
    }
    for (int i = 0; i < BUFLEN; i++) {
        if (optr2[i] != optr[i]) {
            fprintf(stderr, "i: %5d, wrote: %2d, read: %2d\n", i, optr[i], optr2[i]);
            die("Comparing opaque array");
        }
    }
    for (int k = 0; k < NPREC; k++) {
        float ff, fx;
        double dd, dx;
        int nc = ncoord;
        if (xdrfile_decompress_coord_float(fptr2, &nc, &ff, xfp) != ncoord) {
            die("Reading compress_coord_float");
        }
        if (fabsf(ff - fprec[k]) > EPSILON_1) {
            fprintf(stderr, "Found precision %f, expected %f\n", ff, fprec[k]);
            die("Float precision");
        }
        if (ff <= 0) {
            ff = 1000;
        }

        for (int i = 0; i < ncoord; i++) {
            for (int j = 0; j < 3; j++) {
                fx = rintf(fptr[3 * i + j] * ff) / ff;
                if (fabsf(fx - fptr2[3 * i + j]) > EPSILON_1) {
                    fprintf(stderr, "prec: %10g, i: %3d, j: %d, fx: %10g, fptr2: %12g, fptr: %12g\n", ff, i,
                           j, fx, fptr2[3 * i + j], fptr[3 * i + j]);
                    die("Reading decompressed float coordinates");
                }
            }
        }
        if (xdrfile_decompress_coord_double(dptr2, &nc, &dd, xfp) != ncoord) {
            die("Reading compress_coord_double");
        }

        if (fabs(dd - dprec[k]) > EPSILON_2) {
            die("Double precision");
        }

        for (int i = 0; i < ncoord; i++) {
            for (int j = 0; j < 3; j++) {
                dx = rint(dptr[3 * i + j] * dd) / dd;
                if (fabs(dx - dptr2[3 * i + j]) > EPSILON_2) {
                    fprintf(stderr, "prec: %10g, i: %3d, j: %d, dx: %10g, dptr2: %12g, dptr: "
                           "%12g\n",
                           dd, i, j, dx, dptr2[3 * i + j], dptr[3 * i + j]);
                    die("Reading decompressed double coordinates");
                }
            }
        }
    }

    if (xdrfile_close(xfp) != 0) {
        die("Can not close xdr file");
    }

    return 0;
}
