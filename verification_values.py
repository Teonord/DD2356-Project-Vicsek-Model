import ast
import numpy as np

"""@package docstring
Verification file for comparing results in Vicsek Model for Flocking Birds.

Verification file for comparing results in Vicsek Model for Flocking Birds.
VERIF constant has to be set to 1 in C files for it to be used properly.
File by Teo Nordström
"""

filename1 = "res_verify_serial"
filename2 = "res_verify"

with open(filename1, "r") as file1:
    with open(filename2, "r") as file2:
        init_arr = file1.readline().split(" ")
        if init_arr != file2.readline().split(" "):  # All parameters have to be the same.
            print("Wrong Headers")
        for line1 in file1:
            line2 = file2.readline()
            if line1.startswith("Time"):
                print(line1)
                continue

            # Make strings into Python lists
            bird_list1 = np.array(ast.literal_eval("[" + line1 + "]"))
            bird_list2 = np.array(ast.literal_eval("[" + line2 + "]"))

            for i in range(len(bird_list1)):
                if bird_list1[i] not in bird_list2:
                    print(f"Bird {i} from {filename1} not in {filename2}.")
                if bird_list2[i] not in bird_list1:
                    print(f"Bird {i} from {filename2} not in {filename1}.")