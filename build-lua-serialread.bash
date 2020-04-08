export PATH=/opt/gcc-arm-none-eabi-4_7-2014q2/bin:$PATH
mkdir -p build-lua-serialread
cd build-lua-serialread
cmake -DPCB=X9D+ -DGVARS=YES -DLUA=YES -DUSB_SERIAL=YES -DCMAKE_BUILD_TYPE=Debug ..
make firmware
make libsimulator && make companion23
