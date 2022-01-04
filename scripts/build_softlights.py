from pyheaven import *
import numpy as np

if __name__=="__main__":
    args = HeavenArguments.from_parser([
        FloatArgumentDescriptor("x"),
        FloatArgumentDescriptor("y"),
        FloatArgumentDescriptor("z"),
        IntArgumentDescriptor("n",default=6),
        IntArgumentDescriptor("e",default=6),
        FloatArgumentDescriptor("a",default=0.25),
        FloatArgumentDescriptor("i",default=0.9),
        StrArgumentDescriptor("color",default="0.9 0.9 0.9"),
    ])

    lights = []; intensity = args.i/args.n/args.n; d = 2*args.a/(args.n-1)
    for i in range(args.n):
        for j in range(args.n):
            lights.append((
                str(args.x + d * (i-(args.n-1)/2) ),
                str(args.y),
                str(args.z + d * (j-(args.n-1)/2)),
            ))
    
    print(
"""
Lights {
    numLights <nLights>
<lights>
<customlights>
}
""".replace("<nLights>",str(len(lights)+args.e)).replace("<lights>","".join([
"""\tPointLight {
        position <x> <y> <z>
        color <color>
        intensity <i>
    }
""".replace("<x>",l[0]).replace("<y>",l[1]).replace("<z>",l[2]).replace("<color>",args.color).replace("<i>",str(intensity))
for l in lights]))
    )