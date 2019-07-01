/* Copyright (c) 2009-2014, Erik Lindahl & David van der Spoel
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice, this
 * list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 * this list of conditions and the following disclaimer in the documentation
 * and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include <math.h>
#include <stdio.h>
#include <stdlib.h>

#include "xdrfile_trr.h"

static void _die(char* msg, int line, char* file) {
    fprintf(stderr, "Fatal error: %s\n", msg);
    fprintf(stderr, "Death occurred at %s, line %d\n", file, line);
    exit(1);
}

static void _die_r(char* msg, int result, int line, char* file) {
    fprintf(stderr, "Fatal error: %s\n", msg);
    fprintf(stderr, "result = %d\n", result);
    fprintf(stderr, "Death occurred at %s, line %d\n", file, line);
    exit(1);
}

#define die(msg) _die(msg, __LINE__, __FILE__)
#define die_r(msg, res) _die_r(msg, res, __LINE__, __FILE__)

#define EPSILON 1e-3f

int main(void) {
    char* testfn = "test.trr";
    XDRFILE* xd;
    int result, nframes = 13;
    int natoms2, natoms1 = 173;
    int step2, step1 = 1993;
    float time2, time1 = 1097.23f;
    matrix box2, box1;
    rvec *x2, *x1;
    float lambda2, lambda1 = 0.4f;

    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 3; j++) {
            box1[i][j] = (i + 1) * 3.7f + (j + 1);
        }
    }
    x1 = calloc(natoms1, sizeof(*x1));
    if (NULL == x1) {
        die("Allocating memory for x1 in test_xtc");
    }

    for (int i = 0; i < natoms1; i++) {
        for (int j = 0; j < 3; j++) {
            x1[i][j] = (i + 1) * 3.7f + (j + 1);
        }
    }
    xd = xdrfile_open(testfn, "w");
    if (NULL == xd) {
        die("Opening trr file for writing");
    }
    for (int k = 0; k < nframes; k++) {
        result = write_trr(xd, natoms1, step1 + k, time1 + k, lambda1, box1, x1, NULL, NULL);
        if (0 != result) {
            die_r("Writing trr file", result);
        }
    }
    xdrfile_close(xd);

    result = read_trr_natoms(testfn, &natoms2);
    if (exdrOK != result) {
        die_r("read_trr_natoms", result);
    }
    if (natoms2 != natoms1) {
        die("Number of atoms incorrect when reading trr");
    }
    x2 = calloc(natoms2, sizeof(x2[0]));
    if (NULL == x2) {
        die("Allocating memory for x2");
    }

    xd = xdrfile_open(testfn, "r");
    if (NULL == xd) {
        die("Opening trr file for reading");
    }

    for (int k = 0; k < nframes; k++) {
        result = read_trr(xd, natoms2, &step2, &time2, &lambda2, box2, x2, NULL, NULL);
        if (exdrOK != result) {
            die_r("read_trr", result);
        }
        if (natoms2 != natoms1) {
            die("natoms2 != natoms1");
        }
        if (step2 - step1 != k) {
            die("incorrect step");
        }
        if (fabsf(time2 - time1 - k) > EPSILON) {
            die("incorrect time");
        }
        if (fabsf(lambda2 - lambda2) > EPSILON) {
            die("incorrect lambda");
        }
        for (int i = 0; i < 3; i++) {
            for (int j = 0; j < 3; j++) {
                if (fabsf(box2[i][j] - box1[i][j]) > EPSILON) {
                    die("box incorrect");
                }
            }
        }
        for (int i = 0; i < natoms1; i++) {
            for (int j = 0; j < 3; j++) {
                if (fabsf(x2[i][j] - x1[i][j]) > EPSILON) {
                    die("x incorrect");
                }
            }
        }
    }

    xdrfile_close(xd);
    free(x1);
    free(x2);

    return 0;
}
