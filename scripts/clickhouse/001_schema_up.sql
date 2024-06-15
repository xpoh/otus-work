CREATE TABLE IF NOT EXISTS default.DialogMessage_single ON CLUSTER "cluster_2S_1R"
(
    `id` UInt64 COMMENT 'время отправки сообщения',
    `userFrom` UUID COMMENT 'uuid пользователя, который отправил сообщение',
    `userTo` UUID COMMENT 'uuid пользователя, который получил сообщение сообщение',
    `text` varchar  COMMENT 'text'
)

    ENGINE = MergeTree() PRIMARY KEY (id)
        ORDER BY id
        SETTINGS index_granularity = 8192, merge_with_ttl_timeout = 7200
        COMMENT 'Таблица сообщений пользователей'
    ;
