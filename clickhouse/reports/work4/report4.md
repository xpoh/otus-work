# ClickHouse Homework. UDF, Aggregate Functions and working with data types

# Вариант 1:

## Цель:

Цель этого домашнего задания \- помочь вам понять и применить агрегатные функции, функции, работающие с типами данных, и функции, определяемые пользователем (UDF) в ClickHouse.

## Задачи:

Вам предстоит выполнить следующие задания:

1. Используйте агрегатные функции для обобщения данных.  
2. Применять функции, работающие с различными типами данных.  
3. Создавать и использовать функции, определяемые пользователем (UDF), в ClickHouse.

## Набор данных:

Для выполнения этого домашнего задания вы будете использовать пример набора данных, представляющего транзакции электронной коммерции. Предположим, что у вас есть таблица \`transactions\` со следующей схемой:
```clickhouse
CREATE TABLE transactions (  
    transaction_id UInt32,  
    user_id UInt32,  
    product_id UInt32,  
    quantity UInt8,  
    price Float32,  
    transaction_date Date  
) ENGINE = MergeTree()  
ORDER BY (transaction_id);

-- наполним случкайными данными:
INSERT INTO transactions
SELECT
    number AS transaction_id,
    toUInt32(rand() % 50 + 100) AS user_id,
    toUInt32(rand() % 10 + 1000) AS product_id,
    toUInt8(rand() % 5 + 1) AS quantity,
    round(rand() * 50 + 5, 2) AS price,
    toDate('2023-01-01') + rand() % 30 AS transaction_date
FROM system.numbers
LIMIT 1000;

```

## Задание:

1. **Агрегатные функции**  
   1. Рассчитайте общий доход от всех операций.  
```clickhouse
SELECT sum(price*quantity) from transactions; -- 307508958243776
```
   2. Найдите средний доход с одной сделки.  
```clickhouse
SELECT avg(price*quantity) from transactions; -- 307508958243.776
```
   3. Определите общее количество проданной продукции.  
```clickhouse
SELECT sum(quantity) from transactions; -- 2976
```
   4. Подсчитайте количество уникальных пользователей, совершивших покупку.
```clickhouse
SELECT uniq(user_id) from transactions; -- 50
```

2. **Функции для работы с типами данных**  
   1. Преобразуйте \`transaction_date\` в **строку** формата \`YYYY-MM-DD\`.
```clickhouse
SELECT toString(transaction_date) FROM transactions LIMIT 1; -- 2023-01-02
```
   2. Извлеките год и месяц из \`transaction_date\`.  
```clickhouse
SELECT toYear(transaction_date), toMonth(transaction_date) FROM transactions LIMIT 1; -- 2023,1
```
   3. Округлите \`price\` до ближайшего целого числа.
```clickhouse
SELECT round(price) FROM transactions LIMIT 1; -- 89259120000
```
   4. Преобразуйте \`transaction_id\` в строку.  
```clickhouse
SELECT toString(transaction_id) FROM transactions LIMIT 1; -- 0
```
3. **User-Defined Functions (UDFs)**  
   1. Создайте простую UDF для расчета общей стоимости транзакции. 
```clickhouse
CREATE FUNCTION transaction_total AS (quantity, price) -> quantity * price;
```
   2. Используйте созданную UDF для расчета общей цены для каждой транзакции.  
```clickhouse
SELECT transaction_total(quantity, price) FROM default.transactions;
-- 178518245376
-- 190141431808
-- 197837242368
-- 7550595072
-- 70526320640
-- 65015943168
-- ....
```
   3. Создайте UDF для классификации транзакций на «высокоценные» и «малоценные» на основе порогового значения (например, 100). 
```clickhouse
CREATE FUNCTION transaction_cost AS (quantity, price, level) -> 
    if(quantity*price < level, 'малоценные', 'высокоценные');
```
   4. Примените UDF для категоризации каждой транзакции.
```clickhouse
SELECT transaction_id, transaction_cost(quantity, price, 100) FROM transactions;
-- 0,высокоценные
-- 1,высокоценные
-- 2,высокоценные
-- 3,высокоценные
-- 4,высокоценные

DROP FUNCTION transaction_cost;
DROP FUNCTION transaction_total;
```

# Вариант 2:

## Цель:

Цель этого домашнего задания \- помочь вам понять и применить исполняемые пользовательские функции (EUDF) в ClickHouse. EUDF позволяют расширить функциональность ClickHouse путем написания пользовательских функций на внешних языках программирования, таких как Python.

## Задачи:

Вы выполните следующие задачи:

1. Настроить среду для использования EUDF.  
2. Создайте и зарегистрируйте EUDF в ClickHouse.  
3. Используйте EUDF для выполнения пользовательских преобразований данных и вычислений.

## Набор данных:

Для выполнения этого домашнего задания вы будете использовать пример набора данных, представляющего транзакции электронной коммерции. Предположим, что у вас есть таблица \`transactions\` со следующей схемой:

CREATE TABLE transactions (  
    transaction_id UInt32,  
    user_id UInt32,  
    product_id UInt32,  
    quantity UInt8,  
    price Float32,  
    transaction_date Date  
) ENGINE = MergeTree()  
ORDER BY (transaction_id);

## Задание:

1. **Настройка среды для EUDF**  
   1. Установка необходимого программного обеспечения. Убедитесь, что у вас установлен Python и необходимые библиотеки. Используйте следующие команды для настройки среды:

   sudo apt-get install python3 python3-pip  
   pip3 install clickhouse-driver

2. Настройте ClickHouse для EUDF. Убедитесь, что ClickHouse настроен на разрешение EUDF. Измените конфигурационный 
файл ClickHouse (обычно находится по адресу \`/etc/clickhouse-server/config.xml\`), чтобы включить следующие настройки:

   \<clickhouse\>  
       \<user_defined_executable_functions_config\>  
           \<allow_functions\>true\</allow_functions\>  
           \<execution_path\>/path/to/your/udf/script\</execution_path\>  
       \</user_defined_executable_functions_config\>  
   \</clickhouse\>

3. Создание каталога для сценариев EUDF

   mkdir /path/to/your/udf

2. **Создание и применение EUDF**  
   1. Создайте простой скрипт Python UDF. Напишите сценарий Python для расчета общей цены транзакции. Сохраните этот скрипт под именем \`total_price.py\` в вашей директории EUDF:
```python
   import sys  
   import json

   def total_price(quantity, price):  
       return quantity \* price

   if __name__ == "__main__":  
       data = json.load(sys.stdin)  
       quantity = data\['quantity'\]  
       price = data\['price'\]  
       print(total_price(quantity, price))
```

2. Применение EUDF в ClickHouse. Используйте следующую команду SQL для регистрации EUDF:
```clickhouse
   CREATE FUNCTION total_price AS
    '/etc/clickhouse-server/config.d/udf/total_price.py'
    RETURNS Float32   
   EXECUTE ON HOST;

Expected one of: OR, AND, IS NOT DISTINCT FROM, IS NULL, IS NOT NULL, BETWEEN, NOT BETWEEN, LIKE, ILIKE, NOT LIKE, NOT ILIKE, REGEXP, IN, NOT IN, GLOBAL IN, GLOBAL NOT IN, MOD, DIV, ParallelWithClause, PARALLEL WITH, end of query
```
