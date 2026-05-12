mkdir build
cd Borek/vendor/mruby &&
./minirake &&
cd ../../.. &&
cd build &&
cmake ./.. &&
make -j$(nproc --all) &&
sudo cp Stavitel/Stavitel /usr/local/bin/borek-stavitel &&
sudo cp Sandbox/SandboxApp /usr/local/bin/borek
