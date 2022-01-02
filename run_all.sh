#!/usr/bin/env bash
clear

# If project not ready, generate cmake file.
if [[ ! -d build ]]; then
    echo "good"
else
    rm -rf build
fi
mkdir -p build
cd build
cmake ..
make -j
cd ..

# Run all testcases. 
# You can comment some lines to disable the run of specific examples.
rm -rf output
mkdir -p output
# bin/FINAL testcases/scene00_debug.txt output/scene00.bmp
# bin/FINAL testcases/scene01_basic.txt output/scene01.bmp
# bin/FINAL testcases/scene02_cube.txt output/scene02.bmp
# bin/FINAL testcases/scene03_sphere.txt output/scene03.bmp
# bin/FINAL testcases/scene04_axes.txt output/scene04.bmp
# bin/FINAL testcases/scene05_bunny_200.txt output/scene05.bmp
# # bin/FINAL testcases/scene06_bunny_1k.txt output/scene06.bmp
# bin/FINAL testcases/scene07_shine.txt output/scene07.bmp

python  scripts/build_scene.py \
        --W 1920 \
        --H 1080 \
        --softlights "-x 0 -y 5 -z 5 -n 36 -s 0.2 -i 1.2" \
        --scene "resources/scene/debug_scene_00.txt" \
        --texture "resources/textures/mlp.tga" \
        --color "0 0 255" \
        > testcases/scene.txt
bin/FINAL testcases/scene.txt output/scene.bmp