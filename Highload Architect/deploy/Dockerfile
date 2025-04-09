FROM golang:1.22.1 as builder

ARG BIN_NAME=otus-work

WORKDIR /opt/app

COPY . .

ARG VERSION
ARG COMMIT
ARG BRANCH
ARG BUILDER

RUN test -n "$VERSION" && test -n "$COMMIT" && test -n "$BRANCH" && test -n "$BUILDER"

RUN make strip VERSION=$VERSION COMMIT=$COMMIT BRANCH=$BRANCH BUILDER=$BUILDER

FROM alpine:3.19.1

WORKDIR /srv

COPY scripts scripts

COPY --from=builder /opt/app/bin/net /srv/net

COPY ./index.html /srv/index.html

ENTRYPOINT ["/srv/net"]
