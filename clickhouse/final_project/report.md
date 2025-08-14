# Report

## Анализ и визуализация данных по трафику сетевого устройства, используя Netflow протокол и БД Clickhouse

Инструкция по запуску:

* Создать сеть docker для объединения сетей двух docker-compose:
```shell
docker network create final_project_default
```

* Установить Apache Superset:
```shell
git clone https://github.com/apache/superset.git
# В файле `docker-compose-light.yml` прописать секцию network (или скопировать из текущего репозитория [docker-compose-light.yml](docker-compose-light.yml)) :
network:
  name: final_project_default

docker compose up -f docker-compose-light.yml

# Выполнить в контейнере superset:
docker compose exec superset `pip install clickhouse-connect`
```

* Выполнить команду `docker-compose up -d` в текущем репозитории
* Настроить сетевое устройство для отправки Netflow данных на порт 6343 сервиса goflow.
* зайти на панель superset http://localhost:8080/
* Выполнить миграцию базы: [create.sh](fs/volumes/clickhouse/create.sh)

```plantuml
@startuml
left to right direction

package "Docker Services" {
  [clickhouse] as clickhouse
  [clickhouse-keeper] as clickhouse_keeper
  [goflow] as goflow
  [kafka] as kafka
  [kafka-ui] as kafka_ui
  [postgresql] as postgresql
  [redis] as redis
  [airflow-scheduler] as airflow_scheduler
  [airflow-triggerer] as airflow_triggerer
  [airflow-dag-processor] as airflow_dag_processor
  [airflow-worker] as airflow_worker
  [airflow] as airflow
}
  [Mikrotik] as mikrotik

  mikrotik --> goflow
  goflow --> kafka
  kafka --> clickhouse
  clickhouse --> airflow  
  clickhouse ..> clickhouse_keeper : depends_on
  kafka_ui -left-> kafka

  airflow_scheduler -[hidden]-> airflow_triggerer
  airflow_triggerer -[hidden]-> airflow_dag_processor
  airflow_dag_processor -[hidden]-> airflow_worker
  airflow_worker -[hidden]-> airflow
  
  airflow_scheduler ..> postgresql : uses
  airflow_triggerer ..> postgresql : uses
  airflow_dag_processor ..> postgresql : uses
  airflow_worker ..> postgresql : uses
  airflow ..> postgresql : uses
  
  airflow_scheduler ..> redis : uses
  airflow_worker ..> redis : uses
@enduml

```
