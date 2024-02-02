FROM alpine:3.14

WORKDIR /app
COPY . /app

RUN apk add make g++ boost1.76-dev nlohmann-json libpcap-dev cppcheck
RUN make devops
RUN make clean

WORKDIR /root

EXPOSE 3000
ENTRYPOINT ["devops", "--port=3000"]