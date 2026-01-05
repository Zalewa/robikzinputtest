# Canonical name for this image is:
# com.github.zalewa/robikzinputtest-linux-packaging:latest
FROM ubuntu:22.04

# Essentials.
RUN apt-get update && \
	DEBIAN_FRONTEND=noninteractive TZ=Etc/UTC apt-get -y --no-install-recommends install \
	ca-certificates \
	cmake \
	curl \
	file \
	g++-9 \
	gcc-9 \
	git \
	make \
	&& rm -rf /var/lib/apt/lists/*

# SDL3 dependencies.
# <https://github.com/libsdl-org/SDL/blob/release-3.2.30/docs/README-linux.md#build-dependencies>
RUN apt-get update && \
	DEBIAN_FRONTEND=noninteractive TZ=Etc/UTC apt-get -y --no-install-recommends install \
	pkg-config gnome-desktop-testing libasound2-dev libpulse-dev \
	libaudio-dev libfribidi-dev libjack-dev libsndio-dev libx11-dev libxext-dev \
	libxrandr-dev libxcursor-dev libxfixes-dev libxi-dev libxss-dev libxtst-dev \
	libxkbcommon-dev libdrm-dev libgbm-dev libgl1-mesa-dev libgles2-mesa-dev \
	libegl1-mesa-dev libdbus-1-dev libibus-1.0-dev libudev-dev libthai-dev \
	libpipewire-0.3-dev libwayland-dev libdecor-0-dev liburing-dev \
	&& rm -rf /var/lib/apt/lists/*

# Install linuxdeploy for AppImage packaging.
RUN curl https://github.com/linuxdeploy/linuxdeploy/releases/download/1-alpha-20251107-1/linuxdeploy-$(uname -m).AppImage \
	-Lo /opt/linuxdeploy \
	&& chmod +x /opt/linuxdeploy
# Necessary to run linuxdeploy, which itself is an AppImage.
# Otherwise there is a FUSE error
# <https://github.com/appimage/appimagekit/wiki/fuse#docker>
# (accessed 2026-01-05)
ENV APPIMAGE_EXTRACT_AND_RUN=1

# Make GCC 9 the default.
RUN update-alternatives --install /usr/bin/gcc gcc /usr/bin/gcc-9 9 \
	&& update-alternatives --install /usr/bin/g++ g++ /usr/bin/g++-9 9

# Create volume for the repo and mark it as safe.
RUN mkdir -p /src/robikzinputtest
RUN git config --global --add safe.directory /src/robikzinputtest
VOLUME /src/robikzinputtest
WORKDIR /src/robikzinputtest
