source emsdk/emsdk_env.fish

em++ main.cpp \
    -O3 \
    -flto \
    -msimd128 \
    -sUSE_SDL=3 \
    -sALLOW_MEMORY_GROWTH=1 \
    -o index.html \
    --shell-file shell.html

python3 -m http.server 8000