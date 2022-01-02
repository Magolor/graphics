from pyheaven import *
import numpy as np

if __name__=="__main__":
    args = HeavenArguments.from_parser([
        FloatArgumentDescriptor("x"),
        FloatArgumentDescriptor("y"),
        FloatArgumentDescriptor("z"),
        IntArgumentDescriptor("n",default= 25),
        FloatArgumentDescriptor("s",default=0.15),
        FloatArgumentDescriptor("i",default=0.9),
        StrArgumentDescriptor("color",default="0.9 0.9 0.9"),
    ])

    lights = []; intensity = args.i/args.n
    for _ in range(args.n):
        lights.append((
            str(args.x + np.random.randn()*args.s),
            str(args.y + np.random.randn()*args.s),
            str(args.z + np.random.randn()*args.s),
        ))
    
    print(
"""
Lights {
    numLights <nLights>
<lights>
}
""".replace("<nLights>",str(args.n)).replace("<lights>","".join([
"""\tPointLight {
        position <x> <y> <z>
        color <color>
        intensity <i>
    }
""".replace("<x>",l[0]).replace("<y>",l[1]).replace("<z>",l[2]).replace("<color>",args.color).replace("<i>",str(intensity))
for l in lights]))
    )