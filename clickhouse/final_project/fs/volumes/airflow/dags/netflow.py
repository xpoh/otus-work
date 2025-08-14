from airflow import DAG
from airflow.providers.clickhouse.operators.clickhouse import ClickHouseOperator
from datetime import datetime

with DAG(
    'clickhouse_data_collection',
    schedule_interval='@daily',
    start_date=datetime(2023, 1, 1),
    catchup=False
) as dag:
    
    extract_data = ClickHouseOperator(
        task_id='extract_data',
        database='default',
        sql='SELECT * FROM netflow_raw WHERE Date > {{ ds }}',
        clickhouse_conn_id='clickhouse_default'
    )
