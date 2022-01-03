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
        --W 512 \
        --H 512 \
        --softlights "-x 0.0 -y 2.4 -z 0.0 -n 6 -a 0.25 -i 1.8" \
        --scene "resources/scene/02_room_scene.txt" \
        --camera "resources/camera/standard.txt" \
        --texture "resources/textures/mlp.tga" \
        --color "0 0 255" \
        > testcases/scene.txt
bin/FINAL testcases/scene.txt output/scene.bmp