with subq as
(
select
	number,
	number%2 as odd
from system.numbers
limit 15
)
select
	number,
	odd,
	sum(number) over (partition by odd order by number asc) as cumulative_sum_oddnoodd
from subq
---order by number


CREATE or replace TABLE wnd_example (group Int32, id Int32, value Int32) ENGINE =  Memory;

INSERT INTO wnd_example FORMAT Values
(1,1,10), (1,1,20), (1,2,30), (1,5,40), (1,5,50),
(2,1,1), (2,2,2), (2,3,3), (2,3,4), (2,3,5), (2,4,6), (2,5,7), (2,6,8), (2,7,9), (2,8,10),
(3,1,100), (3,2,200), (3,3,300);

select * from wnd_example;

/*
SELECT <window_function_name>([<argument_list>]) OVER {() | <window_name> | (<window_spec>)}
FROM <table_name>
--WINDOW <window_name> AS (<window_spec>);

[<window_name>]
[PARTITION BY <partition_column> [, ...]]
[ORDER BY <order_column> [ASC|DESC] [, ...]]
[{RANGE | ROWS} {<frame_start> | BETWEEN <frame_start> AND <frame_end>}]
*/

SELECT
    group,
    id,
    value,
    sum(value) OVER (w) AS sum,
    groupArray(value) OVER (w) AS frame_values
FROM wnd_example
WINDOW w AS (PARTITION BY group);--1);


SELECT
    group,
    id,
    value,
    row_number() OVER (w) AS number,
    sum(value) OVER (w) AS sum,
    groupArray(value) OVER (w) AS frame_values
FROM wnd_example
WINDOW w AS (PARTITION BY `group` order by id);


SELECT
    group,
    id,
    value,
    row_number() OVER w AS number,
    sum(value) OVER w AS sum,
    groupArray(value) OVER w AS frame_values
FROM wnd_example
WINDOW 
	w AS (PARTITION BY `group` 
             order by id asc
			 ROWS CURRENT ROW);

SELECT
    group,
    id,
    value,
    rank() OVER w AS rank,
    dense_rank() OVER w AS dense_rank,
    --first_value(value) OVER w AS first_value,
    --last_value(value) OVER w AS last_value,
    --nth_value(value, 3) OVER w AS nth_value,
    --lagInFrame(value, 1) OVER w AS lagInFrame,
    --leadInFrame(value, 1) OVER w AS leadInFrame,
    row_number() OVER w AS number,
    sum(value) OVER w AS sum,
    groupArray(value) OVER w AS frame_values
FROM wnd_example
WINDOW 
	w AS (PARTITION BY `group` 
             order by id asc
			 RANGE CURRENT ROW);     



