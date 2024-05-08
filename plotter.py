import ast
import matplotlib.pyplot as plt
import numpy as np

"""
Plotter for Vicsek Model for Flocking Birds
File by Teo Nordström

Based on work by Philip Mocz (2021)
"""

filename = "res"

fig = plt.figure(figsize=(4,4), dpi=160)
ax = plt.gca()

with open(filename, "r") as file:
    init_arr = file.readline().split(" ")  # First line has Nr. of Timesteps, amount of birds, domain size and timestep.
    for line in file:
        if line.startswith("Time"):  # Time line tells runtime of code.
            print(line)
            continue
        bird_list = np.array(ast.literal_eval("[" + line + "]"))  # Make each line into a Python list

        plt.cla()
        plt.quiver(bird_list[:, 0], bird_list[:, 1], bird_list[:, 2], bird_list[:, 3])  # Make a plot with arrows.
        ax.set(xlim=(0, float(init_arr[2])), ylim=(0, float(init_arr[2])))
        ax.set_aspect('equal')
        ax.get_xaxis().set_visible(False)
        ax.get_yaxis().set_visible(False)
        plt.pause(float(init_arr[3]) / 5)   # Animates Plot

plt.savefig("activematter.png", dpi=240)    # Saves final plot to file.
plt.show()