FROM gcc
MAINTAINER Apiary "sre@apiary.io"

ADD . /usr/src/drafter
WORKDIR /usr/src/drafter

RUN ./configure
RUN make install

CMD /usr/local/bin/drafter
