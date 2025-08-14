CREATE or replace TABLE cities (
  id UInt64, 
  parentId UInt64, 
  name String) 
ENGINE=Memory;

insert into cities values (3, 5,'Paris');
insert into cities values (4, 6,'Paris');
insert into cities values (5, 0,'Texas');
insert into cities values (6, 0,'Idaho');


CREATE or replace DICTIONARY dict_injective
  (id UInt16, 
   name String injective, 
   parentId UInt64 hierarchical) 
PRIMARY KEY id SOURCE(CLICKHOUSE(TABLE cities USER 'click' PASSWORD 'click' DB 'default')) 
LIFETIME(MIN 0 MAX 0) LAYOUT(HASHED());


CREATE or replace DICTIONARY dict_not_injective
  (id UInt16, 
   name String, 
   parentId UInt64 hierarchical ) 
PRIMARY KEY id SOURCE(CLICKHOUSE(TABLE cities USER 'click' PASSWORD 'click' )) 
LIFETIME(MIN 0 MAX 0) LAYOUT(HASHED());


select 
  count(), 
  dictGet('dict_injective', 'name', id) name
 --, any(id) as id1
 --, dictGet('dict_injective', 'name', dictGetHierarchy('dict_injective', id1)[-1]) parentName
from (select toUInt64(arrayJoin([3,4])) id )
group by name;
  
select 
  count(), 
  dictGet('dict_not_injective', 'name', id) name
  --, any(id) as id1
  --, dictGet('dict_not_injective', 'name', dictGetHierarchy('dict_not_injective', id1)[-1]) parentName
from (select toUInt64(arrayJoin([3,4])) id )
group by name;


select toUInt64(arrayJoin([3,4])) id;