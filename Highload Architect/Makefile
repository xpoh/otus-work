export GO111MODULE=on
export GOOS=linux
export GOARCH=amd64
export DOCKER_BUILDKIT=1
export CGO_ENABLED=0

SHELL := /bin/bash

TARGET ?= net

VERSION ?= manual_version
COMMIT_SH ?= $(shell git rev-list -1 HEAD --abbrev-commit)
COMMIT ?= $(if $(COMMIT_SH),$(COMMIT_SH),no_commit)
BRANCH ?= manual_branch
BUILD_TIME := $(shell date +%s)
BUILDER ?= $(shell hostname)

IMAGE_BASE_NAME := docker.io/akaddr/$(TARGET)
IMAGE_TAG ?= v0.5.0
IMAGE_NAME := $(IMAGE_BASE_NAME):$(IMAGE_TAG)

MOCKS_DIR := mocks
MOCKGEN_INSTALL_LOCK := mockgen_install.lock
MOCKGEN_LOCK := mockgen.lock

################################################################################
### API
###
api-gen:
	./openapi-generator-cli generate \
		-i ./api/net.json \
		-g go-gin-server \
		-o ./pkg/api
	./openapi-generator-cli generate \
		-i ./api/net.json \
		-g mysql-schema \
		-o ./scripts/mysql
	./openapi-generator-cli generate \
		-i ./api/net.json \
		-g wsdl-schema \
		-o ./scripts/wsdl

################################################################################
### Builds
###

build:
	go build -o bin/$(TARGET) ./cmd/$(TARGET)

################################################################################
### Uploads & pushes
###

image:
	docker build \
		-f deploy/Dockerfile \
		--tag $(IMAGE_NAME) \
		--build-arg=VERSION=$(VERSION) \
		--build-arg=COMMIT=$(COMMIT) \
		--build-arg=BRANCH=$(BRANCH) \
		--build-arg=BUILDER=$(BUILDER) .

push: image
	docker push $(IMAGE_NAME)

################################################################################
### Tests
###

test: mocks
	go test -v ./... -bench=.

test-coverage:
	go test ./... -coverprofile=coverage.txt -covermode atomic
	go tool cover -func=coverage.txt | grep 'total'
	gocover-cobertura < coverage.txt > coverage.xml

################################################################################
### Linters
###

lint: tidy linters

linters: golangci-lint

golangci-lint:
	find -type f -name "*.go" | grep -v '.*\.pb\.go' | grep -v '\/[0-9a-z_]*.go' && echo "Files should be named in snake case" && exit 1 || echo "All files named in snake case"
	golangci-lint version
	golangci-lint run

################################################################################
### Golang helpers
###

tidy:
	gofumpt -w .
	gci write . --skip-generated -s standard -s default
	go mod tidy

download:
	go mod download

modup: tidy
	go get -u ./...
	go mod tidy

################################################################################
### Other Helpers
###

clean:
	rm -rf bin/$(TARGET)

strip: build
	strip bin/$(TARGET)

.PHONY: build
