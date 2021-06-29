# syntax=docker/dockerfile:1

FROM debian:latest

ARG ID
ENV BUSID $ID

RUN apt-get update -yq \
&& apt-get install libmosquitto1 -yq

COPY ./bus /usr/bin/

CMD bus -i $BUSID -h mqtt_c

