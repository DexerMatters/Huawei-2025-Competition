cmake -DCMAKE_BUILD_TYPE=Release
cmake --build .
python ./docs/run.py ./docs/interactor/linux/interactor docs/data/sample_official.in ./code_craft --debug