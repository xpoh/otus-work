# Инструкция по выполнению задания ClickHouse

## Шаги выполнения

### 1. Создание таблицы

Создайте таблицу `sales` со следующими полями:
- `id` (UInt32) — уникальный идентификатор продажи
- `product_id` (UInt32) — идентификатор продукта
- `quantity` (UInt32) — количество проданных единиц
- `price` (Float32) — цена за единицу
- `sale_date` (DateTime) — дата продажи
```clickhouse
CREATE TABLE sales (
    id UInt32,
    product_id UInt32,
    quantity UInt32,
    price Float32,
    sale_date DateTime
) ENGINE = MergeTree()
ORDER BY (product_id, sale_date);
-- Query id: 7c8417de-c278-4381-85d7-7d9994c8f10a
```
Заполните таблицу тестовыми данными.
```clickhouse
INSERT INTO sales VALUES
    (1, 101, 2, 19.99, '2023-01-01 10:00:00'),
    (2, 102, 1, 49.99, '2023-01-01 11:30:00'),
    (3, 101, 3, 18.50, '2023-01-02 09:15:00'),
    (4, 103, 5, 9.99, '2023-01-02 14:20:00'),
    (5, 102, 2, 45.00, '2023-01-03 16:45:00'),
    (6, 101, 1, 20.00, '2023-01-04 12:10:00');
-- 6 rows in set. Elapsed: 0.002 sec
```

### 2. Создание проекции

Создайте проекцию для таблицы `sales`, которая будет:
- Агрегировать данные по `product_id`
- Считать общую сумму продаж (количество и сумма по цене) для каждого продукта
```clickhouse
ALTER TABLE sales ADD PROJECTION sales_projection (
    SELECT 
        product_id,
        sum(quantity) AS total_quantity,
        sum(quantity * price) AS total_sales
    GROUP BY product_id
);
ALTER TABLE sales MATERIALIZE PROJECTION sales_projection;
-- [2025-07-05 11:00:59] completed in 11 ms

SET optimize_use_projections=1;
-- [2025-07-08 08:14:27] 1 row affected in 5 ms
```

### 3. Создание материализованного представления

Создайте материализованное представление `sales_mv`, которое:
- Автоматически обновляется при вставке новых данных в `sales`
- Хранит общие продажи по продуктам с полями:
  - `product_id`
  - `total_quantity`
  - `total_sales`
```clickhouse
CREATE MATERIALIZED VIEW sales_mv
ENGINE = MergeTree()
ORDER BY product_id
POPULATE AS
SELECT 
    product_id,
    sum(quantity) AS total_quantity,
    sum(quantity * price) AS total_sales
FROM sales
GROUP BY product_id;
-- Query id: 1e8b150c-cfc0-4866-9cbe-e9975d0037f0
-- Ok.
-- 0 rows in set. Elapsed: 0.025 sec.

```

### 4. Запросы к данным

Выполните следующие запросы:
1. Извлечение данных из проекции `sales_projection`
```clickhouse
SELECT
  product_id,
  sum(quantity) AS total_quantity,
  sum(quantity * price) AS total_sales
FROM sales
GROUP BY product_id
  SETTINGS optimize_use_projections = 1
  SETTINGS log_queries = 1

-- Query id: 0e4f4bb4-9678-45a5-b087-38e7d47b36b6
-- 
--    ┌─product_id─┬─total_quantity─┬────────total_sales─┐
-- 1. │        101 │             12 │ 230.95999908447266 │ss table.
-- 2. │        103 │             10 │  99.89999771118164 │
-- 3. │        102 │              6 │  279.9800033569336 │
--    └────────────┴────────────────┴────────────────────┘
-- 
-- 3 rows in set. Elapsed: 0.004 sec. 
```

Видно, что проекция используется при выполнении запроса к данным
```clickhouse
SELECT
    query,
    projections
FROM system.query_log

-- Query id: 67791642-a9f6-42b2-9290-11d7bc84f9d9

--    ┌─query────────────────────────────────────┬─projections────────────────────────┐
-- 1. │ SELECT                                  ↴│ ['default.sales.sales_projection'] │
--    │↳    product_id,                         ↴│                                    │
--    │↳    sum(quantity) AS total_quantity,    ↴│                                    │
--    │↳    sum(quantity * price) AS total_sales↴│                                    │
--    │↳FROM sales                              ↴│                                    │
--    │↳GROUP BY product_id                     ↴│                                    │
--    │↳SETTINGS optimize_use_projections=1     ↴│                                    │
--    │↳SETTINGS log_queries=1                   │                                    │
--    └──────────────────────────────────────────┴────────────────────────────────────┘
```

3. Извлечение данных из материализованного представления `sales_mv`
```clickhouse
SELECT
  SELECT
    product_id,
  total_quantity,
  total_sales
FROM sales_mv

--        Query id: c53356d5-c713-4173-8abc-cf9cfc5200a8
-- 
--    ┌─product_id─┬─total_quantity─┬────────total_sales─┐
-- 1. │        101 │             12 │ 230.95999908447266 │ss table.
-- 2. │        102 │              6 │  279.9800033569336 │
-- 3. │        103 │             10 │  99.89999771118164 │
--    └────────────┴────────────────┴────────────────────┘
-- 
-- 3 rows in set. Elapsed: 0.001 sec. 
```

4. Извлечение данных из таблицы `sales`
```clickhouse
SELECT
    product_id,
    sum(quantity) AS total_quantity,
    sum(quantity * price) AS total_sales
FROM sales
GROUP BY product_id
-- 
--     Query id: 3bf799e3-792f-4fcb-8b9f-074dcbf16a00
-- 
--    ┌─product_id─┬─total_quantity─┬────────total_sales─┐
-- 1. │        101 │             12 │ 230.95999908447266 │ss table.
-- 2. │        103 │             10 │  99.89999771118164 │
-- 3. │        102 │              6 │  279.9800033569336 │
--    └────────────┴────────────────┴────────────────────┘
-- 3 rows in set. Elapsed: 0.002 sec.
```

### 5. Сравнение производительности

Сравните время выполнения:
- Запроса к основной таблице `sales`
- Запроса к проекции `sales_projection`
- Запроса к материализованному представлению `sales_mv`

Обратите внимание на разницу в производительности.

| Запрос                                     | Время выполнения |
|--------------------------------------------|------------------|
| Запрос к основной таблице                  | ~2 мс            |
| Запрос к проекции                          | ~2 мс            |
| Запрос к материализованному представлению  | ~1 мс            |

