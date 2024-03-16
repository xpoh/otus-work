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
IMAGE_TAG ?= v0.1.0
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
	go build -ldflags "-X gitlab.rdp.ru/tt/cmetrics/pkg/buildinfo.version=$(VERSION) \
		-X gitlab.rdp.ru/tt/cmetrics/pkg/buildinfo.commit=$(COMMIT) \
		-X gitlab.rdp.ru/tt/cmetrics/pkg/buildinfo.branch=$(BRANCH) \
		-X gitlab.rdp.ru/tt/cmetrics/pkg/buildinfo.buildUnixTimestamp=$(BUILD_TIME) \
		-X gitlab.rdp.ru/tt/cmetrics/pkg/buildinfo.builder=$(BUILDER)" -o bin/$(TARGET) ./cmd/$(TARGET)

image:
	docker build \
		-f deploy/Dockerfile \
		--tag $(IMAGE_NAME) \
		--build-arg=VERSION=$(VERSION) \
		--build-arg=COMMIT=$(COMMIT) \
		--build-arg=BRANCH=$(BRANCH) \
		--build-arg=BUILDER=$(BUILDER) .

push:
	docker push $(IMAGE_NAME)

################################################################################
### Generators
###

buf-build:
	buf generate -o pkg/grpc

$(MOCKGEN_INSTALL_LOCK):
	go install github.com/golang/mock/mockgen@v1.6.0
	touch $(MOCKGEN_INSTALL_LOCK)

$(MOCKGEN_LOCK):
	# mockgen ...
	touch $(MOCKGEN_LOCK)

mocks: $(MOCKGEN_INSTALL_LOCK) $(MOCKGEN_LOCK)

################################################################################
### Uploads & pushes
###

imagetar: image
	buildah push $(IMAGE_NAME) docker-archive:./$(TARGET).tar:$(IMAGE_NAME)

loadpush:
	echo -n "$$CI_DOCKER_PASSWORD" | buildah login -u "$$CI_DOCKER_LOGIN" --password-stdin harbor.tteam.dev
	buildah rmi $(IMAGE_NAME) || echo "image not found"
	buildah pull docker-archive:./$(TARGET).tar:$(IMAGE_NAME)
	buildah push $(IMAGE_NAME) docker://$(IMAGE_NAME)
	if test -n "$$COSIGN_PASSWORD"; then \
		echo $$(echo -n "$$COSIGN_PASSWORD" | \
		podman run --rm -i harbor.tteam.dev/tt/cosign:2.0.1-1 \
		cosign sign -y --key /root/cosign.key $(IMAGE_NAME)); \
	fi

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

linters: golangci-lint buf-lint

golangci-lint:
	find -type f -name "*.go" | grep -v '.*\.pb\.go' | grep -v '\/[0-9a-z_]*.go' && echo "Files should be named in snake case" && exit 1 || echo "All files named in snake case"
	test `grep -Rh "image: " . | grep -v "harbor.tteam.dev" | wc -l` -ne '0' && grep -Rh "image: " . | grep -v "harbor.tteam.dev" && echo 'Found image not from harbor.tteam.dev' && exit 1 || true
	// test `grep -Rh "FROM " . | grep -v "harbor.tteam.dev" | wc -l` -ne '0' && grep -Rh "FROM " . | grep -v "harbor.tteam.dev" && echo 'Found image not from harbor.tteam.dev' && exit 1 || true
	golangci-lint version
	golangci-lint run

buf-lint:
	buf lint

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

buf-download:
	go install github.com/grpc-ecosystem/grpc-gateway/v2/protoc-gen-grpc-gateway@v2.19.1
	go install google.golang.org/protobuf/cmd/protoc-gen-go@v1.32.0
	go install google.golang.org/grpc/cmd/protoc-gen-go-grpc@v1.3.0
	go install github.com/bufbuild/buf/cmd/buf@v1.4.0 \
		github.com/bufbuild/buf/cmd/protoc-gen-buf-breaking@v1.4.0 \
		github.com/bufbuild/buf/cmd/protoc-gen-buf-lint@v1.4.0

################################################################################
### Other Helpers
###

clean:
	rm -rf bin/$(TARGET)

strip: build
	strip bin/$(TARGET)

.PHONY: build

serve:
	go run -ldflags "-X gitlab.rdp.ru/tt/cmetrics/pkg/buildinfo.version=$(VERSION) \
		-X gitlab.rdp.ru/tt/cmetrics/pkg/buildinfo.commit=$(COMMIT) \
		-X gitlab.rdp.ru/tt/cmetrics/pkg/buildinfo.branch=$(BRANCH) \
		-X gitlab.rdp.ru/tt/cmetrics/pkg/buildinfo.buildUnixTimestamp=$(BUILD_TIME) \
		-X gitlab.rdp.ru/tt/cmetrics/pkg/buildinfo.builder=$(BUILDER)" ./cmd/device-manager

# TODO: добавить в CI проверку на генерацию
# # Инженеринг моделей по существующей структуре БД
models:
	go install github.com/volatiletech/sqlboiler/v4@v4.16.1
	go install github.com/volatiletech/sqlboiler/v4/drivers/sqlboiler-psql@v4.16.1
	sqlboiler -c sqlboiler.yaml -p models -o internal/database/models --no-auto-timestamps --no-tests --wipe psql
.PHONY: models
