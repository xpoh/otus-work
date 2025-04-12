# Развертывание и базовая конфигурация, интерфейсы и инструменты 

## 1. Clichouse установлен через docker-compose.yaml

```bash
user@RDP-NB-141 ~/P/m/o/c/work2 (work2)> dc ps
NAME                IMAGE                                        COMMAND            SERVICE             CREATED         STATUS         PORTS
clickhouse          clickhouse/clickhouse-server:latest          "/entrypoint.sh"   clickhouse          7 minutes ago   Up 7 minutes   127.0.0.1:8123->8123/tcp, 127.0.0.1:9000->9000/tcp, 9009/tcp
clickhouse-keeper   clickhouse/clickhouse-keeper:latest-alpine   "/entrypoint.sh"   clickhouse-keeper   7 minutes ago   Up 7 minutes   2181/tcp, 10181/tcp, 44444/tcp, 127.0.0.1:9181->9181/tcp
```

```clickhouse
ch-1S_1K :) select version()

SELECT version()

Query id: 9bd13c3f-a86b-4938-bc0e-22d2b6ca4304

   ┌─version()─┐
1. │ 25.3.2.39 │key to toggle the display of the progress table.
   └───────────┘

1 row in set. Elapsed: 0.001 sec. 
```

## 2. Загрузили тестовый датасет:

```clickhouse
ch-1S_1K :) select count() from trips where payment_type = 1

SELECT count()
FROM trips
WHERE payment_type = 1

Query id: c93f32c1-3a79-4698-a193-576d4491ec1a

   ┌─count()─┐
1. │ 1850287 │ -- 1.85 millionthe display of the progress table.
   └─────────┘

1 row in set. Elapsed: 0.004 sec. Processed 3.00 million rows, 3.00 MB (806.64 million rows/s., 806.64 MB/s.)
Peak memory usage: 85.14 KiB.

```

## 3. Провели тестирование производительности до изменения конфига кликхауса:

```clickhouse
clickhouse-benchmark --query "select count() from trips where payment_type = 1"
```
```clickhouse
localhost:9000, queries: 36917, QPS: 281.348, RPS: 844132007.279, MiB/s: 805.027, result RPS: 281.348, result MiB/s: 0.002.

0%              0.002 sec.      
10%             0.003 sec.      
20%             0.003 sec.      
30%             0.003 sec.      
40%             0.003 sec.      
50%             0.003 sec.      
60%             0.003 sec.      
70%             0.003 sec.      
80%             0.003 sec.      
90%             0.004 sec.      
95%             0.004 sec.      
99%             0.005 sec.      
99.9%           0.009 sec.      
99.99%          0.012 sec.      
```

Изменили следующие настройки в `users.xml`:
`log_queries` - выключили логирование запросов
`use_uncompressed_cache` включили использование несжатого кэша

```clickhouse
localhost:9000, queries: 1232, QPS: 242.909, RPS: 728802559.822, MiB/s: 695.040, result RPS: 242.909, result MiB/s: 0.002.

0%              0.003 sec.      
10%             0.003 sec.      
20%             0.003 sec.      
30%             0.003 sec.      
40%             0.003 sec.      
50%             0.003 sec.      
60%             0.004 sec.      
70%             0.004 sec.      
80%             0.004 sec.      
90%             0.004 sec.      
95%             0.005 sec.      
99%             0.005 sec.      
99.9%           0.008 sec.      
99.99%          0.009 sec. 
```

Видим совсем небольшое увеличение производительности от включения кэширования:
Было: QPS: 281.348
Стало: QPS: 242.909

## Выводы
Поставили clickhouse, поигрались с настройками и запросами на тестовых данных.