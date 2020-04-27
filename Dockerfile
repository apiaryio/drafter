FROM alpine:3.10 as build
MAINTAINER Apiary "sre@apiary.io"

WORKDIR /usr/src/drafter

ADD CMakeLists.txt DefaultBuildType.cmake ./
ADD packages packages

WORKDIR /tmp/drafter

RUN apk add --no-cache cmake make g++
RUN cmake /usr/src/drafter

RUN make drafter
RUN make install

FROM alpine:3.10

RUN apk add --no-cache gcc
COPY --from=build /usr/local/bin/drafter /usr/local/bin/drafter
ADD LICENSE /usr/local/share/licenses/drafter/LICENSE

CMD drafter
