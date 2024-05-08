#include "proj_common.h"
#include <stdio.h>
#include <omp.h>

/**
 * @brief Main function of the simulation.
 *
 * This function performs the entire simulation loop for the Vicsek model of Flocking Birds in a serial manner.
 * Using functions in proj_common.h the model is solved and results are printed in one line per timestep. 
 * The resulting values can then be pasted into the Python program to visualize the flock.
 *
 * @param argc Number of command-line arguments. Unused.
 * @param argv Array of command-line argument strings. Unused.
 * @return Returns 0 upon successful completion.
 */
int main(int argc, char const *argv[])
{
    struct Bird *b; /**< Reusable pointer to a bird struct. */
    int i, j; /**< Loop counters. */

    double R = pow(R_INIT, 2); /**< Interaction radius squared, used in Pythagorean theorem. */

    printf("%d %d %f %f\n", NUMBER, TIMESTEPS, L, DT); /**< Print simulation parameters. */

    srand(SEED); /**< Seed the random number generator. */

    struct Bird *birds = calloc(NUMBER, sizeof(struct Bird)); /**< Allocate memory for bird structs. */

    double startTime = omp_get_wtime(); /**< Record start time of simulation. */
 
    for (i = 0; i < NUMBER; i++) { /**< Initialize birds. */
        initBird(&birds[i]);
    }

    for (i = 0; i < TIMESTEPS; i++) { /**< Main simulation loop. */
        for (j = 0; j < NUMBER; j++) { /**< Update positions of all birds. */
            updateBirdPos(&birds[j]);
        }

        for (j = 0; j < NUMBER; j++){ /**< Calculate angle effects for all birds. */
            calculateAngleEffects(&birds[j], birds, R);
        }

        for (j = 0; j < NUMBER; j++) { /**< Update angles of all birds. */
            b = &birds[j];
            updateBirdAngle(b);
            printf("[%f,%f,%f,%f],", b->x, b->y, b->vx, b->vy); /**< Print position and velocity of each bird. */
        }

        printf("\n"); /**< Print newline after each time step. */
    }

    printf("Time Taken: %f", omp_get_wtime() - startTime); /**< Print total simulation time. */

    free(birds); /**< Free memory allocated for bird structs. */

    return 0; /**< Return 0 to indicate successful completion. */
}