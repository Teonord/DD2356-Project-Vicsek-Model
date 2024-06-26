#include <stdlib.h>
#include <math.h>
#include <stdio.h>

#define VERIF 0         // If Verification is to be run

#define PI 3.14159265358979323846

#if VERIF
    #define TIMESTEPS 50   // Amount of Timesteps
    #define NUMBER 25      // Amount of Birds
    #define SEED 621        // Seed for Randomness

    #define V0 1.0          // Velocity
    #define ETA 0.5         // Random Fluctuation in Angle (Radians)
    #define L 4.0          // Size of Box
    #define R_INIT 1.0      // Interaction Radius
    #define DT 1.0          // Time Step 

    int bn = 0; /**< Global variable to keep track of bird number during verification. */
    const double initxpos[25] = {0,0,0,0,0,1,1,1,1,1,2,2,2,2,2,3,3,3,3,3,4,4,4,4,4}; /**< Initial x positions for birds during verification. */
    const double initypos[25] = {0,1,2,3,4,0,1,2,3,4,0,1,2,3,4,0,1,2,3,4,0,1,2,3,4}; /**< Initial y positions for birds during verification. */
    const double initheta[25] = {0,1,2,3,4,5,6,0,1,2,3,4,5,6,0,1,2,3,4,5,6,0,1,2,3}; /**< Initial angles for birds during verification. */

#else
    #define TIMESTEPS 200   // Amount of Timesteps
    #define NUMBER 500      // Amount of Birds
    #define SEED 621        // Seed for Randomness

    #define V0 1.0          // Velocity
    #define ETA 0.5         // Random Fluctuation in Angle (Radians)
    #define L 10.0          // Size of Box
    #define R_INIT 1.0      // Interaction Radius
    #define DT 0.2          // Time Step 

#endif

/**
 * @brief Struct to represent a bird.
 *
 * This struct represents a bird in the simulation, containing its position,
 * angle, velocity, and sum of sine and cosine components for angle calculation.
 */
struct Bird
{
    double x; /**< X coordinate of the bird. */
    double y; /**< Y coordinate of the bird. */
    double theta; /**< Angle of the bird. */
    double vx; /**< X component of velocity of the bird. */
    double vy; /**< Y component of velocity of the bird. */
    double sx; /**< Sum of cosine components of neighboring bird angles. */
    double sy; /**< Sum of sine components of neighboring bird angles. */
};

/** @brief Generates and returns a random double-precision float in range [0.0, 1.0]
*
*   Function uses the function rand() from <stdlib.h> to generate a random integer value between 0 and RAND_MAX.
*   This value is then cast to a double-precision float and divided by RAND_MAX to get a random value between 0 and 1.
*   Values used in numerous positions in the code.
*   If in verification mode, always return a value of 0.55.
*   
* @return Random double-precision float in range [0.0, 1.0]
*/
double randd() {
    #if VERIF
        return 0.55; /**< Placeholder for randomness in case of verification*/
    #else
        return (double)rand() / (double)RAND_MAX; /**< Generate and return a value in range [0.0, 1.0]*/
    #endif    
}

/** @brief Performs a modulo operation that works in the proper way for negative values.
*
*   The double modulo function "fmod()" from <math.h> returns negative values if input value is negative
*   fmod(-1, 3) = -1
*   We want a modulo that instead returns the positive modulo of a value.
*   ourmod(-1, 3) = 2
*   This is done by adding the value of modulo to the value returned by fmod. 
*
*   @param val Numerator double-precision float value to get remainder from. 
*   @param modulo Denominator double-precision float value to get remainder from. 
*   @return A positive modulo value.
*/
double modd(double val, double modulo) {
    if (val < 0) return fmod(val, modulo) + modulo; /**< Return positive modulo if value is negative. */
    return fmod(val, modulo); /**< Return standard modulo if value is positive. */
}

/**
 * @brief Initializes the position and velocity of a bird.
 *
 * This function initializes the position and velocity of a bird struct.
 * If in verification mode, it sets the position based on predefined arrays.
 * Otherwise, it sets random positions and angles.
 *
 * @param b Pointer to the bird struct to be initialized.
 */
void initBird(struct Bird *b) {
    #if VERIF
        #pragma omp critical
        {
            b->x = initxpos[bn]; /**< Initial x position of the bird. */
            b->y = initypos[bn]; /**< Initial y position of the bird. */

            b->theta = initypos[bn]; /**< Initial angle of the bird. */

            bn++; /**< Increment the bird number. */
        }
    #else
        b->x = randd() * L; /**< Random initial x position within a range. */
        b->y = randd() * L; /**< Random initial y position within a range. */

        b->theta = 2 * PI * randd(); /**< Random initial angle within 0 to 2*PI. */
    #endif
    b->vx = V0 * cos(b->theta); /**< Initial velocity along x direction. */
    b->vy = V0 * sin(b->theta); /**< Initial velocity along y direction. */
}

/**
 * @brief Updates the position of a bird based on its velocity.
 *
 * This function updates the position of a bird struct based on its current
 * velocity and the time step (DT).
 *
 * @param b Pointer to the bird struct to update its position.
 */
void updateBirdPos(struct Bird *b) {
    b->x += b->vx * DT; /**< Update x position based on velocity. */
    b->y += b->vy * DT; /**< Update y position based on velocity. */

    b->x = modd(b->x, L); /**< Apply periodic boundary conditions for x position. */
    b->y = modd(b->y, L); /**< Apply periodic boundary conditions for y position. */
}

/**
 * @brief Calculates the effects of neighboring birds on the current bird's angle.
 *
 * This function calculates the effects of neighboring birds within a certain radius (R)
 * on the angle of the current bird. It updates the sum of sines (sy) and cosines (sx)
 * of the angles of neighboring birds.
 *
 * @param b Pointer to the bird struct to update its angle effects.
 * @param birds Array of all birds in the simulation.
 * @param R Pre-squared radius within which neighboring birds are considered.
 */
void calculateAngleEffects(struct Bird *b, struct Bird *birds, double R) {
    struct Bird *nb; /**< Pointer to a neighboring bird. */
    for (int k = 0; k < NUMBER; k++) {
        nb = &birds[k];
        if (pow(nb->x - b->x, 2) + pow(nb->y - b->y, 2) < R) /**< Check if bird is within squared radius R. */
        {
            b->sx += cos(nb->theta); /**< Update sum of cosines. */
            b->sy += sin(nb->theta); /**< Update sum of sines. */
        }
    }
}

/**
 * @brief Updates the angle of a bird based on the effects of neighboring birds.
 *
 * This function updates the angle of a bird struct based on the sum of sines (sy)
 * and cosines (sx) of the angles of neighboring birds, with added random perturbation.
 *
 * @param b Pointer to the bird struct to update its angle.
 */
void updateBirdAngle(struct Bird *b) {
    b->theta = atan2(b->sy, b->sx) + ETA * (randd() - 0.5); /**< Update angle with noise. */

    b->sx = 0; /**< Reset sum of cosines. */
    b->sy = 0; /**< Reset sum of sines. */

    b->vx = V0 * cos(b->theta); /**< Update x component of velocity based on new angle. */
    b->vy = V0 * sin(b->theta); /**< Update y component of velocity based on new angle. */
}