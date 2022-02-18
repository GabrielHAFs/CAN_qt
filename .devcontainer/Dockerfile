# if you get error: 
# "pull access denied for sha256, repository does not exist or may require 'docker login' : denied: requested access to the resource is denied"
# when opening the project, this may mean that base SDK container has been deleted 
# or that the project has been moved to a different machine where such container does not exist, 
# you can re-build it by pressing F1 in Visual Studio Code and selecting:
# "Torizon: Rebuild SDK and reload in container"
# after this operation you should be able to re-open your project inside a container
FROM qt_isobus_arm64v8-qt5-vivante-no-ssh_bullseye_debug_5abaa0c4-ced0-4b45-be54-92516aa2ab55_sdk_image

# This is required to allow regular user inside the container to access docker socket
RUN addgroup docker && groupmod --non-unique --gid 135 docker && adduser torizon docker
RUN apt-get -y update && apt-get install -y \ 
    nano \ 
    python3 \ 
    python3-pip \ 
    python3-setuptools \ 
    git \ 
    iproute2 \ 
    can-utils \ 
    python3-can \ 
    qml-module-qtquick2 \
    qml-module-qtquick-window2 \
    qml-module-qtquick-controls2 \
    qtvirtualkeyboard-plugin \
    qml-module-qt-labs-folderlistmodel \ 
    qml-module-qtquick-virtualkeyboard \
    qml-module-qtqml \
    python3-can \
    && apt-get clean && apt-get autoremove && rm -rf /var/lib/apt/lists/*  
