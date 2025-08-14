# Домашнее задание
## Репликация и удаление

### Цель:
- Преобразовывать таблицу в реплицируемую
- Настраивать реплики в ClickHouse и работать с данными в распределённой системе

### Описание/Пошаговая инструкция выполнения домашнего задания:
Для выполнения задания следуйте следующим шагам:

1. Возьмите любой демонстрационный DATASET:  
   [https://clickhouse.com/docs/en/getting-started/example-datasets](https://clickhouse.com/docs/en/getting-started/example-datasets)

* Используем https://clickhouse.com/docs/getting-started/example-datasets/environmental-sensors 

2. Конвертируйте таблицу в реплицируемую, используя макрос `replica`

3. Добавьте 2 реплики

4. Выполните запросы и отдайте результаты как 2 файла:

```clickhouse
   SELECT
      getMacro('replica'),
      *
   FROM remote('разделенный запятыми список реплик', system.parts)
   FORMAT JSONEachRow;

    SELECT * FROM system.replicas FORMAT JSONEachRow;
```
5. Добавьте или выберите колонку с типом Date в таблице, добавьте TTL на таблицу «хранить последние 7 дней»

6. На проверку отправьте результат запроса:
```clickhouse
SHOW CREATE TABLE таблица;
```