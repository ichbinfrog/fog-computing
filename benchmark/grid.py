#!/usr/bin/env python3
import numpy as np

def generate_grid(layers):
    height = 2*(layers)
    grid = - np.ones((layers, layers))

    for layer in range(layers):
        grid[layer:, layer] = layer
        grid[layer, layer:] = layer
    
    upper = np.hstack((grid, np.flip(grid, 1)))
    return np.vstack((upper, np.flip(upper, 0)))

def generate(layers, scale, out, dot=False):
    var_map = {
        0 : "csm",
        1 : "frt",
        2 : "buf0",
        3 : "buf1",
        4 : "fog",
    }

    count_map = {
        0 : 0,
        1 : 0,
        2 : 0,
        3 : 0,
        4 : 0,
    }

    grid = generate_grid(layers)
    (size, _) = grid.shape
    name = [["" for i in range(size)] for j in range(size)]

    if not dot:
        with open(out, "w+") as fd:
            fd.write("router\n# node  	comment     yPos    xPos\n")
            for i in range(size):
                for j in range(size):
                    count_map[grid[i,j]] += 1
                    name[i][j] = f"{var_map[grid[i,j]]}{count_map[grid[i,j]]}"
                    fd.write(f"{name[i][j]}\tNA\t{scale * i}\t{scale * j}\n")

            fd.write("\nlink\n# x 		y       capacity(kbps)  OSPF    Delay   MaxPackets\n")
            for i in range(size):
                for j in range(size):
                    if i + 1 < size:
                        fd.write(f"{name[i][j]}\t{name[i+1][j]}\t1Mbps\t1\t50ms\t20\n")                

                    if j + 1 < size:
                        fd.write(f"{name[i][j]}\t{name[i][j+1]}\t1Mbps\t1\t50ms\t20\n")                

                    if i + 1 < size and j + 1 < size:                
                        fd.write(f"{name[i][j]}\t{name[i+1][j+1]}\t1Mbps\t1\t50ms\t20\n")                

                    if i - 1 < size and j - 1 < size:                
                        fd.write(f"{name[i][j]}\t{name[i-1][j-1]}\t1Mbps\t1\t50ms\t20\n")                
    else:
        with open(out, "w+") as fd:
            for i in range(size):
                for j in range(size):
                    count_map[grid[i,j]] += 1
                    name[i][j] = f"{var_map[grid[i,j]]}{count_map[grid[i,j]]}"

            for i in range(size):
                for j in range(size):
                    if i + 1 < size:
                        fd.write(f'"{name[i][j]}" -> "{name[i+1][j]}"\n')                

                    if j + 1 < size:
                        fd.write(f'"{name[i][j]}" -> "{name[i][j+1]}"\n')                

                    if i + 1 < size and j + 1 < size:                
                        fd.write(f'"{name[i][j]}" -> "{name[i+1][j+1]}"\n')                

                    if j - 1 > 0 and i + 1 < size:
                        fd.write(f'"{name[i][j]}" -> "{name[i+1][j-1]}"\n')                

if __name__ == "__main__":
    generate(5, 1, "test_grid", False)