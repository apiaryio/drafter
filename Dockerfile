FROM apiaryio/cpp
MAINTAINER Apiary "sre@apiary.io"

ADD ./ /drafter

WORKDIR /drafter

# It's tempting to put ./configure into RUN, but then you have timestamp issues

RUN ./configure && make all && make install

CMD /usr/local/bin/drafter
