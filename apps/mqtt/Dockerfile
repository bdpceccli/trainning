FROM debian:latest

RUN apt-get update -yq \
&& apt-get install libmosquitto1 \
&& apt-get install libmariadb3

ADD ./bus /usr/bin

CMD bus -i 1


