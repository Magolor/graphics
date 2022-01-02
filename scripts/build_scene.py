from pyheaven import *
import numpy as np

if __name__=="__main__":
    args = HeavenArguments.from_parser([
        IntArgumentDescriptor("W",default=1920),
        IntArgumentDescriptor("H",default=1080),
        StrArgumentDescriptor("scene",default="resources/scene/debug_scene_00.txt"),
        StrArgumentDescriptor("texture",default="resources/textures/envmap.jpg"),
        StrArgumentDescriptor("softlights",short="sl",default=""),
        StrArgumentDescriptor("color",default="0 0 255"),
    ])
    CMD(f"python scripts/build_softlights.py {args.softlights} > resources/lights/softlights.txt")
    softlights = open("resources/lights/softlights.txt","r").read(); scene = open(args.scene, "r").read();
print(
"""
PerspectiveCamera {
    center 0 0 10
    direction 0 0 -1
    up 0 1 0
    angle 30
    width <W>
    height <H>
}

<softlights>

Background {
    color <color>
    texture <texture>
}

<scene>
""".replace("<W>",str(args.W)).replace("<H>",str(args.H)).replace("<softlights>",softlights).replace("<texture>",args.texture).replace("<color>",args.color).replace("<scene>",scene)
)