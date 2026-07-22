FROM alpine:latest AS builder

RUN apk update && apk add --no-cache \
    build-base \
    autoconf \
    automake \
    git

WORKDIR /app
COPY . .

RUN ./autogen.sh || autoreconf -i
RUN ./configure
RUN make

FROM alpine:latest

WORKDIR /app

COPY --from=builder /app/oe /usr/local/bin/oe

ENTRYPOINT ["oe"]
