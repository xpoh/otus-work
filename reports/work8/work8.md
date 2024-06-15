# Разделение монолита на сервисы

1. Создан сервис диалогов, вынесен в отдельный репозиторий: https://github.com/xpoh/otus-work-dialogs
2. Взаимодействие организовано через gRPC, апи описано тут:https://github.com/xpoh/otus-work-dialogs/blob/main/api/grpc/dialogs/v1/dialogs.proto
3. В основном сервисе (https://github.com/xpoh/otus-work) добавлены новые ручки для диалогов (Send, List) с префиксом v2, старые продолжают работать напрямую с кластерос Кликхауса
4. Из-за того что, кластер кликхауса поднят в отдельно докер композе и имеет статическую адресацию порядок запуска стенда такой:
   - поднимается кластер кликхауса в [clickhouse_cluster](..%2F..%2Fclickhouse_cluster)
   - запускается приложение otus-work-dialogs (gRPC сервер) локально go run ./cmd/dialogs/dialogs.go
   - запускается docker-compose основного сервиса
   - основной сервис выключается в композе `docker compose stop service`
   - основной сервис запускается, как приложение `go run ./cmd/dialogs/dialogs.go`
   - для запуска сервисов можно воспользоваться бинарниками в папке bin