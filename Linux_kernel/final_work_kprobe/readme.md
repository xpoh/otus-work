# Тема проектной работы

## "Разработка модуля ядра сбора статистики сетевых подключений - N адресов с самым большим количеством проходящего трафика»"

Ядро: **7.0.1**
Исходный код ядра: [https://git.kernel.org/pub/scm/linux/kernel/git/torvalds/linux.git/](https://git.kernel.org/pub/scm/linux/kernel/git/torvalds/linux.git/)
Пример kprobe: [https://git.kernel.org/pub/scm/linux/kernel/git/torvalds/linux.git/tree/samples/kprobes](https://git.kernel.org/pub/scm/linux/kernel/git/torvalds/linux.git/tree/samples/kprobes)

## Make цели

| Цель          | Описание                                                     |
|---------------|--------------------------------------------------------------|
| `build`       | Сборка модуля ядра (`make -C KERNEL_DIR M=$PWD modules`)     |
| `run`         | Загрузка модуля через `insmod`                               |
| `remove`      | Выгрузка модуля через `rmmod`                                |
| `install`     | Копирование модуля в `/lib/modules/`, `depmod` + `modprobe`  |
| `uninstall`   | `modprobe -r`, удаление `.ko`, `depmod`                      |
| `clean`       | Очистка артефактов сборки                                    |
| `format`      | Форматирование `.c`/`.h` файлов через `clang-format`         |
| `check`       | Проверка успешной сборки модуля                              |
| `restart`     | Цикл `remove` → `build` → `run` → `clean`                   |
| `test`        | Загрузка модуля, вывод `dmesg`, выгрузка модуля              |
| `test-params` | Тест параметров `top_n` и `track_src`, проверка `dmesg`    |
| `deploy`      | Загрузка модуля + запуск `stats_exporter` docker-compose     |
| `deploy-down` | Остановка `stats_exporter` docker-compose                    |

## Параметры модуля

| Параметр      | Тип    | Доступ | По умолчанию | Описание                                         |
|---------------|--------|--------|-------------|--------------------------------------------------|
| `top_n`       | int    | 0644   | 10          | Количество top адресов для вывода                 |
| `top_addrs`   | string | 0444   | —           | Список top N адресов (только чтение)              |
| `track_src`   | bool   | 0644   | false       | Если true — трафик считается по IP-источнику,<br/>иначе — по IP-назначения |


## Архитектура модуля

```plantuml
@startuml
left to right direction
skinparam packageStyle rectangle

package "Пространство ядра" {
  package "kprobe_traffic.ko" {
    component "kprobe_handler.c" as KH <<kretprobes>>
    component "stats.c\nХеш-таблица + спинлок" as ST
    component "params.c\nSysfs параметры" as PM

    component "tcp_sendmsg" as SEND <<entry + ret handler>>
    component "tcp_cleanup_rbuf" as RECV <<entry + ret handler>>

    SEND --> ST
    RECV --> ST
    KH --> ST
    ST --> PM
  }
}

package "Пространство пользователя" {
  component "/sys/module/kprobe_traffic/\nparameters/top_addrs" as SYSFS
  component "cat / echo" as CAT
}

PM --> SYSFS
SYSFS --> CAT
@enduml
```

## Архитектура развёртывания

```plantuml
@startuml
left to right direction
skinparam packageStyle rectangle

package "Хост" {
  component "kprobe_traffic.ko\nМодуль ядра" as KM

  package "Docker Compose" {
    component "stats_exporter\nGo :2112" as EXP
    component "Prometheus\n:9090" as PROM
    component "Grafana\n:3000" as GRAF
  }

  component "Browser\nhttp://localhost:3000" as USER
}

KM ..> EXP : sysfs bind mount (ro)
EXP --> PROM : scrape /metrics
PROM --> GRAF : query
GRAF --> USER
@enduml
```

