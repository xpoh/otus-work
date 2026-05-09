FROM golang:1.25-alpine AS builder
WORKDIR /app
COPY go.mod go.sum ./
RUN go mod download
COPY . .
RUN CGO_ENABLED=0 go build -o /stats_exporter ./cmd/stats_exporter

FROM alpine:3.21
COPY --from=builder /stats_exporter /stats_exporter
EXPOSE 2112
ENTRYPOINT ["/stats_exporter"]
