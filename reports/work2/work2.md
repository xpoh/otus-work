# Отчет по домашней работе №2
## Анализ производительности запросов до оптимизации по индексам

Рисунок 1. Transactions per Second
![img.png](img.png)

Рисунок 2. Response Latencies Over Time
![img_1.png](img_1.png)

От количества конкурентных запросов производительность не поменялась

## Анализ производительности запросов с индексом btree

Создаем индекс btree:
```sql
create index user_first_name_second_name_index
    on public."User" (first_name, second_name);
```
Рисунок 1. Transactions per Second
![img_2.png](img_2.png)

Рисунок 2. Response Latencies Over Time
![img_3.png](img_3.png)

Вывод: ситуация не изменилась

## Анализ производительности запросов с индексом gin

Создаем поле tsvector
```sql
create index User_name_index
    on "User" USING GIN (to_tsvector('english', first_name || ' ' || second_name));
```
Рисунок 1. Transactions per Second
![img_4.png](img_4.png)

Рисунок 2. Response Latencies Over Time
![img_5.png](img_5.png)

Вывод: индекс gin лучше подходит под полнотекстовый поиск 