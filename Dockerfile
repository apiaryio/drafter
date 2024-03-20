FROM debian:bullseye-slim AS drafter-build
MAINTAINER Apiary "sre@apiary.io"

COPY . /usr/src/drafter

WORKDIR /usr/src/drafter

RUN apt-get install --yes cmake g++

RUN cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
RUN cmake --build build
RUN cmake --install build

FROM debian:bullseye-slim AS drafter
COPY --from=drafter-build /usr/local/bin/drafter /usr/local/bin/drafter
ADD LICENSE /usr/local/share/licenses/drafter/LICENSE

CMD drafter
