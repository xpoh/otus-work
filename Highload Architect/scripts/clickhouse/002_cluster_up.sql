CREATE TABLE IF NOT EXISTS default.DialogMessage
    ON CLUSTER 'cluster_2S_1R' as default.DialogMessage_single
    ENGINE = Distributed(`cluster_2S_1R`, `default`,  DialogMessage_single, id);