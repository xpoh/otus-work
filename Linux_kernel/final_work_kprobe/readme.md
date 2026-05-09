# Тема выпускной работы

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
| `test-params` | Тест параметра `top_n` (чтение/запись) и проверка `dmesg`    |
| `deploy`      | Загрузка модуля + запуск `stats_exporter` docker-compose     |
| `deploy-down` | Остановка `stats_exporter` docker-compose                    |

## Архитектура модуля

```mermaid
flowchart TB
    subgraph Kernel["Пространство ядра"]
        subgraph KM["kprobe_traffic.ko"]
            KH["kprobe_handler.c<br/>kretprobes"] --> ST["stats.c<br/>Хеш-таблица + спинлок"]
            ST --> PM["params.c<br/>Sysfs параметры"]

            subgraph Kprobes["kretprobes"]
                SEND["tcp_sendmsg<br/>entry + ret handler"]
                RECV["tcp_cleanup_rbuf<br/>entry + ret handler"]
            end

            SEND --> ST
            RECV --> ST
        end
    end

    subgraph User["Пространство пользователя"]
        SYSFS["/sys/module/kprobe_traffic/<br/>parameters/top_addrs"]
        CAT["cat / echo"]
    end

    PM --> SYSFS
    SYSFS --> CAT
```

## Архитектура развёртывания

```mermaid
flowchart TB
    subgraph Host["Хост"]
        KM["kprobe_traffic.ko<br/>Модуль ядра"]

        subgraph Docker["Docker Compose"]
            EXP["stats_exporter<br/>Go :2112"]
            PROM["Prometheus<br/>:9090"]
            GRAF["Grafana<br/>:3000"]
        end

        USER["Браузер<br/>http://localhost:3000"]
    end

    KM -. "sysfs bind mount (ro)" .-> EXP
    EXP -->|"scrape /metrics"| PROM
    PROM -->|"query"| GRAF
    GRAF --> USER

    style KM fill:#4a6,color:#fff
    style EXP fill:#48f,color:#fff
    style PROM fill:#f84,color:#fff
    style GRAF fill:#f4a,color:#fff
```

