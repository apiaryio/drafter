FROM alpine:3.18 as build
MAINTAINER Apiary "sre@apiary.io"

WORKDIR /usr/src/drafter

ADD CMakeLists.txt DefaultBuildType.cmake ./
ADD packages packages

WORKDIR /tmp/drafter

RUN apk add --no-cache cmake make g++
RUN cmake -S "." -B "build" -DCMAKE_INSTALL_PREFIX=/usr/local -DCMAKE_BUILD_TYPE=Release

RUN cmake --build build
RUN cmake --install build

FROM alpine:3.18

RUN apk add --no-cache gcc
COPY --from=build /usr/local/bin/drafter /usr/local/bin/drafter
ADD LICENSE /usr/local/share/licenses/drafter/LICENSE

CMD drafter
