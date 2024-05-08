#include "proj_common.h"
#include <stdio.h>
#include <omp.h>
#include <mpi.h>
#include <stdbool.h>
#include <time.h>

/**
 * @brief Compares two bird structures for equality.
 *
 * @param b1 Pointer to the first bird structure.
 * @param b2 Pointer to the second bird structure.
 * @return Returns true if the bird structures are the same, otherwise false.
 */
bool sameBirds(struct Bird *b1, struct Bird *b2) {
    if(b1->x != b2->x || b1->y != b2->y || b1->theta != b2->theta || b1->vx != b2->vx || b1->vy != b2->vy || b1->sx != b2->sx || b1->sy != b2->sy) return false;
    return true;
}

/**
 * @brief Tests the randd function.
 *
 * @return Returns 0 if the tests pass, otherwise returns a non-zero value indicating the failure code.
 */
int testRandd() {
    srand(10);
    double t1 = randd();
    srand(10);
    double t2 = randd();
    if (t1 != t2) return 1;     // Test so randomness repeats with same seed

    srand(10);
    t1 = randd();
    t2 = randd();
    if (t1 == t2) return 2;     // Test so randomness is different when not updating seed

    srand(time(NULL));
    double randdval;
    for(int i = 0; i < 10000; i++) {
        randdval = randd();
        if(randdval > 1 || randdval < 0) return 3;      // Test so randd only outputs values in [0, 1]
    }

    return 0;
}

/**
 * @brief Tests the modd function.
 *
 * @return Returns 0 if the tests pass, otherwise returns a non-zero value indicating the failure code.
 */
int testModd() {
    double res;
    if (modd(10, 4) != 2) return 1;
    if (modd(4, 10) != 4) return 2;
    if (modd(-10, 4) != 2) return 3;
    if (modd(-4, 10) != 6) return 4;
    if (modd(10, 10) != 0) return 5;
    if (modd(11, 10) != 1) return 6;
    if (modd(801752871234, 63456634563) != 40273256478) return 7;
    if (modd(10.5, 4) != 2.5) return 8;
    if (modd(4.5, 10) != 4.5) return 9;
    if (modd(10.5, 4.5) != 1.5) return 10;
    if (modd(12.12312512343246, 3.59969312478234892813) != 1.3240457490854145) return 11;
    if (modd(PI * 4, PI) != 0) return 12;
    return 0;
}

/**
 * @brief Tests the initBird function.
 *
 * @return Returns 0 if the tests pass, otherwise returns a non-zero value indicating the failure code.
 */
int testInitBird() {
    struct Bird *b = calloc(1, sizeof(struct Bird));
    if(b->x != 0 || b->y != 0 || b->theta != 0 || b->vx != 0 || b->vy != 0 || b->sx != 0 || b->sy != 0) return 1;   // Check so all values are 0 after allocation

    srand(10);
    initBird(b);
    if(b->x == 0 && b->y == 0 && b->theta == 0 && b->vx == 0 && b->vy == 0) return 2;   // Check so all values have been initalized (Small chance of failure if random hits 0)

    struct Bird *b2 = calloc(1, sizeof(struct Bird));
    srand(10);
    initBird(b2);
    if(!sameBirds(b, b2)) return 3;     // Check so birds have same value if generated using the same seed

    srand(time(NULL));
    for(int i = 0; i < 10; i++) {
        initBird(b);
        if (fabs(pow(b->vx, 2) + pow(b->vy, 2) - 1) > 0.0001) return 4;      // Check so directional velocity is of size V0
    }

    free(b);
    free(b2);

    return 0;
}

/**
 * @brief Tests the updateBirdPos function.
 *
 * @return Returns 0 if the tests pass, otherwise returns a non-zero value indicating the failure code.
 */
int testUpdateBirdPos() {
    struct Bird b1 = {.x=1, .y=1, .theta=1, .vx=1, .vy=1, .sx=0, .sy=0};
    struct Bird b1exp = {.x=1 + DT, .y=1 + DT, .theta=1, .vx=1, .vy=1, .sx=0, .sy=0};
    updateBirdPos(&b1);
    if (!sameBirds(&b1, &b1exp)) return 1;      // Check that movement works in positive direction

    struct Bird b2 = {.x=1, .y=1, .theta=1, .vx=-0.5, .vy=-0.8, .sx=0, .sy=0};
    struct Bird b2exp = {.x=1 - 0.5*DT, .y=1 - 0.8*DT, .theta=1, .vx=-0.5, .vy=-0.8, .sx=0, .sy=0};
    updateBirdPos(&b2);
    if (!sameBirds(&b2, &b2exp)) return 2;      // Check that movement works in negative direction

    struct Bird b3 = {.x=L-0.5, .y=L-0.5, .theta=1, .vx=1 / DT, .vy=1 / DT, .sx=0, .sy=0};
    struct Bird b3exp = {.x=0.5, .y=0.5, .theta=1, .vx=1 / DT, .vy=1 / DT, .sx=0, .sy=0};
    updateBirdPos(&b3);
    if (!sameBirds(&b3, &b3exp)) return 3;      // Check that position wraps in positive direction

    struct Bird b4 = {.x=0.5, .y=0.5, .theta=1, .vx=-1 / DT, .vy=-1 / DT, .sx=0, .sy=0};
    struct Bird b4exp = {.x=L-0.5, .y=L-0.5, .theta=1, .vx=-1 / DT, .vy=-1 / DT, .sx=0, .sy=0};
    updateBirdPos(&b4);
    if (!sameBirds(&b4, &b4exp)) return 4;      // Check that position wraps in negative direction

    srand(time(NULL));
    struct Bird b5 = {.x=L / 2, .y=L / 2, .theta=1, .vx=-(L / 10) / DT, .vy=(L / 10) / DT, .sx=0, .sy=0};
    struct Bird b5exp = {.x=L / 2, .y=L / 2, .theta=1, .vx=-(L / 10) / DT, .vy=(L / 10) / DT, .sx=0, .sy=0};
    for(int i = 0; i < 10; i++) {
        updateBirdPos(&b5);
    }
    if (!sameBirds(&b5, &b5exp)) return 5;      // Check that bird gets to expected value even after having its position updated 10 times

    struct Bird *b6 = calloc(1, sizeof(struct Bird));
    for(int i = 0; i < 1000; i++) {
        double x = randd() * 12;
        double y = randd() * 12;
        double vx = randd() * 4 - 2;
        double vy = randd() * 4 - 2;

        b6->x = x;
        b6->y = y;
        b6->vx = vx;
        b6->vy = vy;
        updateBirdPos(b6);

        if(b6->x != modd(x + vx * DT, L) || b6->y != modd(y + vy * DT, L)) return 6;    // Precalc movement with wrapping and check with random values if function gives right answer for 1000 iterations.
    }
    free(b6);
    
    return 0;
}

/**
 * @brief Tests the calculateAngleEffects function.
 *
 * @return Returns 0 if the tests pass, otherwise returns a non-zero value indicating the failure code.
 */
int testCalculateAngleEffects() {
    int i;
    struct Bird *birds = calloc(NUMBER, sizeof(struct Bird));
    calculateAngleEffects(&birds[0], birds, 1);
    if (birds[0].sx != NUMBER || birds[0].sy != 0) return 1;    // If all angles are 0, then all birds only give cos to sx and zero sin to sy

    srand(time(NULL));
    double ed;
    double xval = 0;
    double yval = 0;
    for (i = 0; i < NUMBER; i++) {
        ed = randd() * 4 * PI - 2 * PI;
        birds[i].theta = ed;
        birds[i].sx = 0;
        birds[i].sy = 0;
        xval += cos(ed);
        yval += sin(ed);
    }
    calculateAngleEffects(&birds[0], birds, 1);
    if (birds[0].sx != xval || birds[0].sy != yval) return 2;         // Check that sx and sy are sums of all random angles

    xval = 0;
    yval = 0;
    double pos;
    for (i = 0; i < NUMBER; i++) {
        pos = i * (sqrt(2) / (double)NUMBER);
        birds[i].x = pos;
        birds[i].y = pos;
        ed = randd() * 4 * PI - 2 * PI;
        birds[i].theta = ed;
        birds[i].sx = 0;
        birds[i].sy = 0;
        if(i < NUMBER / 2) {
            xval += cos(ed);
            yval += sin(ed);
        }
    }
    calculateAngleEffects(&birds[0], birds, 1);
    if (birds[0].sx != xval || birds[0].sy != yval) return 3;         // Check so sum only counts birds within R.

    for (i = 0; i < NUMBER; i++) {
        if (birds[i].vx != 0 || birds[i].vy != 0) return 4;           // Check so vx and vy are untouched.
    }

    free(birds);
    return 0;
}

/**
 * @brief Tests the updateBirdAngle function.
 *
 * @return Returns 0 if the tests pass, otherwise returns a non-zero value indicating the failure code.
 */
int testUpdateBirdAngle() {
    struct Bird *b1 = calloc(1, sizeof(struct Bird));
    srand(1);
    double randval = ETA * (randd() - 0.5);

    srand(1);
    updateBirdAngle(b1);
    if(b1->theta != randval) return 1;      // Check so zero in both sx and sy leads to a pre-permutation value of theta=0

    int i;
    double sxv, syv, res_theta, vxv, vyv;
    for(i = 0; i < 1000; i++) {
        sxv = randd() * 1000 - 500;
        syv = randd() * 1000 - 500;
        struct Bird b = {.x=5, .y=5, .theta=5, .vx=5, .vy=5, .sx=sxv, .sy=syv};
        srand(i);
        res_theta = atan2(syv, sxv) + ETA * (randd() - 0.5);
        vxv = V0 * cos(res_theta);
        vyv = V0 * sin(res_theta);
        srand(i);
        updateBirdAngle(&b);
        if(b.x != 5 || b.y != 5 || b.theta != res_theta || b.vx != vxv || b.vy != vyv || b.sx != 0 || b.sy != 0) return 2;    // Test for correctness
    }
    
    return 0;
}

/**
 * @brief Main function to run all tests.
 * 
 * Function is run to test all of the functions in the proj_common.h header file.
 * If all functions work properly, each line should have a 0.
 * If any line is non-zero, then something is wrong with the respective function.
 * The non-zero value tells which test-case from the given function that failed. 
 *
 * @param argc Number of command-line arguments. Unused.
 * @param argv Command-line arguments array. Unused.
 * @return Returns 0 on successful run, regardless of test results.
 */
int main(int argc, char const *argv[])
{
    printf("%d\n", testRandd());
    printf("%d\n", testModd());
    printf("%d\n", testInitBird());
    printf("%d\n", testUpdateBirdPos());
    printf("%d\n", testCalculateAngleEffects());
    printf("%d\n", testUpdateBirdAngle());
    return 0;
}
