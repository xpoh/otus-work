# Описание/Пошаговая инструкция выполнения домашнего задания:
* Создайте таблицу с полями:
    ```
        user_id UInt64,
        action String,
        expense UInt64.
    ```                         
  
```clickhouse
CREATE TABLE user_actions (
    user_id UInt64,
    action String,
    expense UInt64,
) ENGINE = MergeTree()
ORDER BY user_id;
-- [2025-07-05 10:21:46] completed in 25 ms
```

* Создайте словарь, где ключ — user_id, атрибут — email (String), источник словаря выберите любой удобный, например, файл.
```clickhouse
CREATE TABLE user_emails (
    user_id UInt64,
    email String
) ENGINE = MergeTree()
ORDER BY user_id;
-- [2025-07-05 10:24:38] completed in 35 ms

INSERT INTO user_emails VALUES
    (1, 'user1@example.com'),
    (2, 'user2@example.com'),
    (3, 'user3@example.com'),
    (4, 'user4@example.com');
-- [2025-07-05 10:25:08] 4 rows affected in 9 ms
  
CREATE DICTIONARY user_emails_dict (
    user_id UInt64,
    email String
)
PRIMARY KEY user_id
SOURCE(CLICKHOUSE(
    TABLE 'user_emails'
    HOST 'localhost'
    PORT 9000
    USER 'default'
    PASSWORD ''
    DB 'default'))
LIFETIME(MIN 300 MAX 360)
LAYOUT(HASHED());
-- [2025-07-05 10:26:24] completed in 20 ms
```

* Наполните таблицу и источник данными с низкоардинальными значениями для поля action и несколькими повторяющимися строками для каждого user_id.
```clickhouse
INSERT INTO user_actions (user_id, action, expense) VALUES
    (1, 'purchase', 100),
    (1, 'purchase', 150),
    (1, 'refund', 50),
    (2, 'purchase', 200),
    (2, 'subscription', 50),
    (3, 'purchase', 300),
    (3, 'purchase', 100),
    (3, 'refund', 100),
    (4, 'subscription', 30),
    (4, 'purchase', 250),
    (4, 'purchase', 70);
-- [2025-07-05 10:27:28] 11 rows affected in 8 ms
```

* Напишите SELECT, который возвращает:
  * email с помощью dictGet,
  * аккумулятивную сумму expense с окном по action,
  * сортировку по email.
  ```clickhouse
  SELECT
  dictGet('user_emails_dict', 'email', user_id) AS email,
  sum(expense) OVER (PARTITION BY action) AS cumulative_expense_by_action
  FROM user_actions
  ORDER BY email;
  --   user1@example.com	1170
  --   user1@example.com	1170
  --   user1@example.com	150
  --   user2@example.com	1170
  --   user2@example.com	80
  --   user3@example.com	1170
  --   user3@example.com	1170
  --   user3@example.com	150
  --   user4@example.com	1170
  --   user4@example.com	1170
  --   user4@example.com	80
  ```

