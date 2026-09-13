FROM mcr.microsoft.com/devcontainers/cpp:2-debian13

RUN apt-get update && apt-get upgrade -y && export DEBIAN_FRONTEND=noninteractive \
    && apt-get -y install --no-install-recommends python-is-python3 python3-pip libwayland-client0 libxkbcommon0 clangd mesa-utils libfontconfig


# Install latest objdiff
ADD --chmod=755 https://github.com/encounter/objdiff/releases/latest/download/objdiff-linux-x86_64 /usr/local/bin/objdiff

# Make python work by creating venv
RUN apt install -y python3-venv
RUN python3 -m venv /venv
ENV VIRTUAL_ENV=/venv
ENV PATH=/venv/bin:$PATH

# Copying ../requirements.txt because it can't be referenced directly
RUN python -m pip install pyelftools sortedcontainers pyyaml "splat64[mips]>=0.36.0,<1.0.0" "spimdisasm>=1.36.1,<2.0.0" "rabbitizer==1.13.0" tqdm pygfxd

# Install nfsmw match extension
ADD --chmod=644 https://github.com/dbalatoni13/nfsmw/releases/latest/download/nfsmw-match-annotations.vsix /tmp/nfsmw-match-annotations.vsix 
