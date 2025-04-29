**Условия ДЗ:**

1. По заданным описаниям таблиц и вставки данных определить используемый движок
2. Заполнить пропуски, запустить код
3. Сравнить полученный вывод и результат из условия

---
1. Движок **ReplacingMergeTree**, т.к. происходит замещение записей по первичному ключу **UserID**:

```clickhouse
CREATE TABLE tbl1  (  
    UserID UInt64,  
    PageViews UInt8,  
    Duration UInt8,  
    Sign Int8,  
    Version UInt8  
)  
ENGINE = ReplacingMergeTree()  
ORDER BY UserID;

INSERT INTO tbl1 VALUES (4324182021466249494, 5, 146, -1, 1);  
INSERT INTO tbl1 VALUES (4324182021466249494, 5, 146, 1, 1),(4324182021466249494, 6, 185, 1, 2);

SELECT * FROM tbl1;
-- 
-- ┌──────────────UserID─┬─PageViews─┬─Duration─┬─Sign─┬─Version─┐
-- │ 4324182021466249494 │         5 │      146 │   -1 │       1 │
-- │ 4324182021466249494 │         6 │      185 │    1 │       2 │
-- └─────────────────────┴───────────┴──────────┴──────┴─────────┘

SELECT * FROM tbl1 final;
-- ┌──────────────UserID─┬─PageViews─┬─Duration─┬─Sign─┬─Version─┐
-- │ 4324182021466249494 │         6 │      185 │    1 │       2 │
-- └─────────────────────┴───────────┴──────────┴──────┴─────────┘
```

2. Используем движок SummingMergeTree, т.к. происходит суммирование по ключу **key**
```clickhouse
CREATE TABLE tbl2  (  
    key UInt32,  
    value UInt32  
)  
ENGINE SummingMergeTree()  
ORDER BY key;

INSERT INTO tbl2 Values(1,1),(1,2),(2,1);

select * from tbl2;
-- ┌─key─┬─value─┐
-- │   1 │     3 │
-- │   2 │     1 │
-- └─────┴───────┘
```

3. ReplacingMergeTree - т.к. происходит замещение последней записью
```clickhouse
CREATE TABLE tbl3  (  
    `id` Int32,  
    `status` String,  
    `price` String,  
    `comment` String  
)  
ENGINE = ReplacingMergeTree()  
PRIMARY KEY (id)  
ORDER BY (id, status);

INSERT INTO tbl3 VALUES (23, 'success', '1000', 'Confirmed');  
INSERT INTO tbl3 VALUES (23, 'success', '2000', 'Cancelled');

SELECT * from tbl3 WHERE id=23;
-- ┌─id─┬─status──┬─price─┬─comment───┐
-- │ 23 │ success │ 2000  │ Cancelled │
-- │ 23 │ success │ 1000  │ Confirmed │
-- └────┴─────────┴───────┴───────────┘

SELECT * from tbl3 FINAL WHERE id=23;
-- ┌─id─┬─status──┬─price─┬─comment───┐
-- │ 23 │ success │ 2000  │ Cancelled │
-- └────┴─────────┴───────┴───────────┘
```

4. tbl4 - MergeTree т.к. просто является источником данных, tbl5 - AggregatingMergeTree т.к. использует агрегации
```clickhouse
CREATE TABLE tbl4  (
    CounterID UInt8,  
    StartDate Date,  
    UserID UInt64  
) ENGINE MergeTree()  
PARTITION BY toYYYYMM(StartDate)   
ORDER BY (CounterID, StartDate);

INSERT INTO tbl4 VALUES(0, '2019-11-11', 1);  
INSERT INTO tbl4 VALUES(1, '2019-11-12', 1);

CREATE TABLE tbl5 (
    CounterID UInt8,
    StartDate Date,
    UserID AggregateFunction(uniq, UInt64)
) ENGINE AggregatingMergeTree()  
PARTITION BY toYYYYMM(StartDate)   
ORDER BY (CounterID, StartDate);

INSERT INTO tbl5
select CounterID, StartDate, uniqState(UserID)
from tbl4
group by CounterID, StartDate;

INSERT INTO tbl5 VALUES (1,'2019-11-12',1);
-- Error on processing query: Code: 53. DB::Exception: Cannot convert UInt64 to AggregateFunction(uniq, UInt64): While executing ValuesBlockInputFormat: data for INSERT was parsed from query. (TYPE_MISMATCH) (version 25.3.2.39 (official build))

SELECT uniqMerge(UserID) AS state
FROM tbl5
GROUP BY CounterID, StartDate;
-- ┌─state─┐
-- │     1 │
-- │     1 │
-- └───────┘
```
---
6. ReplacingMergeTree - т.к. после финала остается последнее значение
```clickhouse
CREATE TABLE tbl6  (  
    `id` Int32,  
    `status` String,  
    `price` String,  
    `comment` String,  
    `sign` Int8  
)  
ENGINE ReplacingMergeTree()  
PRIMARY KEY (id)  
ORDER BY (id, status);

INSERT INTO tbl6 VALUES (23, 'success', '1000', 'Confirmed', 1);  
INSERT INTO tbl6 VALUES (23, 'success', '1000', 'Confirmed', -1), (23, 'success', '2000', 'Cancelled', 1);

SELECT * FROM tbl6;
-- ┌─id─┬─status──┬─price─┬─comment───┬─sign─┐
-- │ 23 │ success │ 1000  │ Confirmed │    1 │
-- │ 23 │ success │ 2000  │ Cancelled │    1 │
-- └────┴─────────┴───────┴───────────┴──────┘

SELECT * FROM tbl6 FINAL;
-- ┌─id─┬─status──┬─price─┬─comment───┬─sign─┐
-- │ 23 │ success │ 2000  │ Cancelled │    1 │
-- └────┴─────────┴───────┴───────────┴──────┘
-- 

```
---
