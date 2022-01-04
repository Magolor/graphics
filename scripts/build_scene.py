from pyheaven import *
import numpy as np

if __name__=="__main__":
    args = HeavenArguments.from_parser([
        IntArgumentDescriptor("W",default=1920),
        IntArgumentDescriptor("H",default=1080),
        StrArgumentDescriptor("scene",default="resources/scene/debug_scene_00.txt"),
        StrArgumentDescriptor("camera",default="resources/camera/standard.txt"),
        StrArgumentDescriptor("texture",default="resources/textures/envmap.jpg"),
        StrArgumentDescriptor("softlights",short="sl",default=""),
        StrArgumentDescriptor("customlights",short="cl",default=""),
        IntArgumentDescriptor("num_customlights",short="ncl",default=0),
        StrArgumentDescriptor("color",short="color",default="0 0 255"),
    ])
    CMD(f"python scripts/build_softlights.py {args.softlights} -e {args.customlights.count('Light')} > resources/lights/softlights.txt")
    softlights = open("resources/lights/softlights.txt","r").read()
    scene = open(args.scene, "r").read(); camera = open(args.camera, "r").read()
print(
"""
<camera>

<softlights>

Background {
    color <color>
    texture <texture>
}

<scene>
""".replace("<camera>",camera).replace("<W>",str(args.W)).replace("<H>",str(args.H)).replace("<softlights>",softlights).replace("<customlights>",args.customlights).replace("<texture>",args.texture).replace("<color>",args.color).replace("<scene>",scene)
)