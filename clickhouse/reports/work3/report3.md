# Отчет
по ДЗ №3

1. [x] Создайте новую базу данных и перейдите в неё.
2. [x] Разработайте таблицу для бизнес-кейса "Меню ресторана" с минимум пятью полями. Наполните таблицу данными, 
используя модификаторы (например, Nullable, LowCardinality), где это необходимо. Не забудьте добавить комментарии к полям.

```clickhouse
CREATE TABLE menu
(
    `menu_id` UInt32,
    `dish_id` UInt32,
    `dish_name` String,
    `description` String,
    `category` Enum('Appetizer' = 1, 'Main Course' = 2, 'Dessert' = 3, 'Beverage' = 4, 'Side Dish' = 5),
    `price` Decimal(10, 2),
    `cost` Decimal(10, 2),
    `is_vegetarian` UInt8,
    `is_vegan` UInt8,
    `is_gluten_free` UInt8,
    `calories` Nullable(UInt16),
    `preparation_time` Nullable(UInt8) COMMENT 'Preparation time in minutes',
    `is_available` UInt8 DEFAULT 1,
    `ingredients` Array(String),
    `allergens` Array(Enum('Dairy' = 1, 'Eggs' = 2, 'Peanuts' = 3, 'Tree Nuts' = 4, 'Soy' = 5, 'Wheat' = 6, 'Fish' = 7, 'Shellfish' = 8)),
    `created_at` DateTime DEFAULT now(),
    `updated_at` DateTime DEFAULT now(),
    `version` UInt32 DEFAULT 1
)
    ENGINE = ReplacingMergeTree(version)
        ORDER BY (menu_id, dish_id)
        PRIMARY KEY (menu_id, dish_id)
        PARTITION BY menu_id
        SETTINGS index_granularity = 8192;

INSERT INTO menu
(menu_id, dish_id, dish_name, description, category, price, is_vegetarian, ingredients, allergens)
VALUES
    (1, 101, 'Pasta Carbonara', 'Classic Roman pasta with eggs, pecorino, pancetta, and black pepper.', 'Main Course', 12.90, 0, ['Spaghetti', 'Pancetta', 'Eggs', 'Pecorino Romano', 'Parmesan', 'Black pepper'], ['Eggs', 'Dairy']),
    (1, 102, 'Risotto ai Funghi', 'Creamy risotto with wild mushrooms and Parmesan.', 'Main Course', 14.50, 1, ['Arborio rice', 'Mushrooms', 'Onion', 'White wine', 'Parmesan', 'Broth'], ['Dairy']),
    (2, 103, 'Pizza Margherita', 'Traditional pizza with tomato sauce, mozzarella, and basil.', 'Main Course', 10.90, 1, ['Pizza dough', 'Tomato sauce', 'Mozzarella', 'Basil', 'Olive oil'], ['Wheat', 'Dairy']),
    (2, 104, 'Tiramisù', 'Coffee-flavored dessert with layers of savoiardi and mascarpone cream.', 'Dessert', 7.50, 1, ['Savoiardi', 'Coffee', 'Mascarpone', 'Eggs', 'Sugar', 'Cocoa'], ['Eggs', 'Dairy']),
    (2, 105, 'Bruschetta al Pomodoro', 'Toasted bread topped with tomatoes, garlic, and basil.', 'Appetizer', 6.50, 1, ['Ciabatta bread', 'Tomatoes', 'Garlic', 'Basil', 'Olive oil'], ['Wheat']);

```

4. [x] Протестируйте выполнение операций CRUD на созданной таблице.
```clickhouse
-- CREATE
INSERT INTO menu (menu_id, dish_id, dish_name, description, category, price, is_vegetarian, ingredients, allergens) 
VALUES (1, 105, 'Create item 3', 'testing create', 'Appetizer', 6.50, 1, ['Ciabatta bread', 'Tomatoes', 'Garlic', 'Basil', 'Olive oil'], ['Wheat']);

-- READ
SELECT * from menu where dish_id = 101;

--UPDATE
INSERT INTO menu (menu_id, dish_id, dish_name)
VALUES (3, 105, 'Updated name 1');
INSERT INTO menu (menu_id, dish_id, dish_name)
VALUES (3, 105, 'Updated name 2');
INSERT INTO menu (menu_id, dish_id, dish_name)
VALUES (3, 105, 'Updated name 3');

--DELETE 
DELETE from menu WHERE dish_id = 105;
```

4. [x] Добавьте несколько новых полей в таблицу и удалите два-три существующих.
```clickhouse
ALTER TABLE menu 
    ADD COLUMN added_colum1 String COMMENT 'test add column 1',
    ADD COLUMN added_colum2 UInt64 COMMENT 'test add column 2';

ALTER TABLE menu 
    DROP COLUMN added_colum1,
    DROP COLUMN added_colum2;
```

5. [x] Выполните выборку данных (select) из любой таблицы из sample dataset
```clickhouse
-- Будем работать с YouTube dataset of dislikes (https://clickhouse.com/docs/getting-started/example-datasets/youtube-dislikes)
DESCRIBE s3(
        'https://clickhouse-public-datasets.s3.amazonaws.com/youtube/original/files/*.zst',
        'JSONLines'
         );

CREATE TABLE youtube
(
    `id` String,
    `fetch_date` DateTime,
    `upload_date_str` String,
    `upload_date` Date,
    `title` String,
    `uploader_id` String,
    `uploader` String,
    `uploader_sub_count` Int64,
    `is_age_limit` Bool,
    `view_count` Int64,
    `like_count` Int64,
    `dislike_count` Int64,
    `is_crawlable` Bool,
    `has_subtitles` Bool,
    `is_ads_enabled` Bool,
    `is_comments_enabled` Bool,
    `description` String,
    `rich_metadata` Array(Tuple(call String, content String, subtitle String, title String, url String)),
    `super_titles` Array(Tuple(text String, url String)),
    `uploader_badges` String,
    `video_badges` String
)
    ENGINE = MergeTree
        ORDER BY (uploader, upload_date);

INSERT INTO youtube
SELECT
    id,
    parseDateTimeBestEffortUSOrZero(toString(fetch_date)) AS fetch_date,
    upload_date AS upload_date_str,
    toDate(parseDateTimeBestEffortUSOrZero(upload_date::String)) AS upload_date,
    ifNull(title, '') AS title,
    uploader_id,
    ifNull(uploader, '') AS uploader,
    uploader_sub_count,
    is_age_limit,
    view_count,
    like_count,
    dislike_count,
    is_crawlable,
    has_subtitles,
    is_ads_enabled,
    is_comments_enabled,
    ifNull(description, '') AS description,
    rich_metadata,
    super_titles,
    ifNull(uploader_badges, '') AS uploader_badges,
    ifNull(video_badges, '') AS video_badges
FROM s3(
        'https://clickhouse-public-datasets.s3.amazonaws.com/youtube/original/files/*.zst',
        'JSONLines'
     );

-- посмотрим количество записей:
SELECT formatReadableQuantity(count()) FROM youtube;

-- селект
SELECT
    title,
    like_count,
    dislike_count
FROM youtube
WHERE uploader = 'ClickHouse'
ORDER BY dislike_count DESC;

```
6. [x] Материализуйте выбранную таблицу, создав её копию в виде отдельной таблицы.
```clickhouse
CREATE TABLE youtube_copy ENGINE = MergeTree()
ORDER BY id
PARTITION BY  toMonth(fetch_date)
AS SELECT * FROM youtube LIMIT 1000;

```
7. [x] Попрактикуйтесь с партициями: выполните операции ATTACH, DETACH и DROP. После этого добавьте новые данные в первоначально созданную таблицу.
```clickhouse
SELECT count(1) FROM youtube; -- 74062339
ALTER TABLE youtube DETACH PART 'all_1_32_2';
SELECT count(1) FROM youtube; -- 61804919
ALTER TABLE youtube DROP PART 'all_71_102_2';
SELECT count(1) FROM youtube; -- 49799614
ALTER TABLE youtube ATTACH PART 'all_1_32_2';
SELECT count(1) FROM youtube; -- 62057034

```
