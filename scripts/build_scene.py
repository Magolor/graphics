from pyheaven import *
import numpy as np

scene = ("""
Group {
    numObjects 6
    MaterialPreset resources/materials/mirror.txt
    Sphere {
	center 0 0 0
        radius 1 
    }
    MaterialPreset resources/materials/ivory.txt
    Sphere {
	center 1 1 1 
        radius 0.75
    }
    MaterialPreset resources/materials/glass.txt
    Sphere {
	center -1 -1 1 
        radius 0.75
    }
    MaterialPreset resources/materials/red_shiny.txt
    Sphere {
	center -1 1 -1 
        radius 0.75
    }
    MaterialPreset resources/materials/red_plastic.txt
    Sphere {
	center 1 -1 -1 
        radius 0.75 
    }
    MaterialPreset resources/materials/mirror.txt
    Plane {
	normal 0 1 0
        offset -2
    }
}
""")

if __name__=="__main__":
    args = HeavenArguments.from_parser([
        IntArgumentDescriptor("W",default=1080),
        IntArgumentDescriptor("H",default=720),
        StrArgumentDescriptor("color",default="0 0 255"),
        StrArgumentDescriptor("softlights"),
    ])
    CMD(f"python scripts/build_softlights.py {args.softlights} > resources/lights/softlights.txt")
    softlights = open("resources/lights/softlights.txt","r").read()
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
}

<scene>
""".replace("<W>",str(args.W)).replace("<H>",str(args.H)).replace("<softlights>",softlights).replace("<color>",args.color).replace("<scene>",scene)
)