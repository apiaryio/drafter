FROM apiaryio/base-dev-cpp:1.0.0
MAINTAINER Apiary "sre@apiary.io"


RUN locale-gen en_US.UTF-8
RUN echo "LC_ALL=en_US.UTF-8" >> /etc/default/locale
RUN dpkg-reconfigure locales

ADD ./ /drafter

WORKDIR /drafter

# It's tempting to put ./configure into RUN, but then you have timestamp issues

RUN ./configure && make all && make install

CMD /usr/local/bin/drafter
