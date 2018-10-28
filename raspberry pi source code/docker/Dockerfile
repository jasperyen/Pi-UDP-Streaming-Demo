FROM sgtwilko/rpi-raspbian-opencv:stretch-3.4.3

# update apt
RUN apt-get update \
	&& apt-get -qy install git autoconf automake libtool \
	&& apt-get purge -y --auto-remove \
	&& rm -rf /var/lib/apt/lists/* \
# install raspicam
	&& cd /tmp \
	&& git clone https://github.com/cedricve/raspicam \
	&& cd raspicam/ \
	&& mkdir bulid \
	&& cd bulid \
	&& cmake .. \
	&& make -j3 \
	&& make install \
	&& ldconfig \
# install libjpeg-turbo-1.5.3
	&& cd /tmp \
	&& wget -c -N -nv -O libjpeg-turbo-1.5.3.tar.gz https://codeload.github.com/libjpeg-turbo/libjpeg-turbo/tar.gz/1.5.3 \
	&& tar -zxvf libjpeg-turbo-1.5.3.tar.gz \
	&& cd libjpeg-turbo-1.5.3 \
	&& mkdir build \
	&& autoreconf -fiv \
	&& cd build \
	&& export CFLAGS="-mcpu=cortex-a53 -mfpu=neon-fp-armv8 -mfloat-abi=hard -O3" \
	&& export CXXFLAGS="-mcpu=cortex-a53 -mfpu=neon-fp-armv8 -mfloat-abi=hard -O3" \
	&& sh /tmp/libjpeg-turbo-1.5.3/configure \
	&& make -j3 \
	&& make install \
	&& ln -s /opt/libjpeg-turbo/lib32/libturbojpeg.so.0.1.0 /usr/lib/libturbojpeg.so.0 \
# install pybind11
	&& cd /tmp \
	&& pip3 install pytest \
	&& wget -c -N -nv -O pybind11-2.2.4.tar.gz https://codeload.github.com/pybind/pybind11/tar.gz/v2.2.4 \
	&& tar -zxvf pybind11-2.2.4.tar.gz \
	&& cd pybind11-2.2.4 \
	&& mkdir build \
	&& cd build \
	&& cmake -DPYBIND11_PYTHON_VERSION=3.5 .. \
	&& make -j3 \
	&& make install \
	&& ldconfig \
	&& cd / \
	&& rm -rf /tmp/*

CMD ["/bin/bash"]
