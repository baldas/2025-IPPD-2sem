#!/usr/bin/env python3

import sys
from PIL import Image, ImageDraw


# command line parsing
WIDTH = HEIGHT = int(sys.argv[1]) # image size
MAX_ITER = int(sys.argv[2])
file = open(sys.argv[3])  # filename with the points


im = Image.new('RGB', (WIDTH, HEIGHT), (0, 0, 0))
draw = ImageDraw.Draw(im)

line = file.readline()


#for x in range(0, WIDTH):
x=0
while line:
    numbers = line.split()
    for y in range(0, HEIGHT):
        # The color depends on the number of iterations
        m = int(numbers[y])
        color = 255 - int(m * 255 / MAX_ITER)
        # Plot the point
        draw.point([y, x], (color, color, color))
    line = file.readline()
    x = x+1

im.save('output.png', 'PNG')

