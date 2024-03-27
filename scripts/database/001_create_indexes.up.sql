create index User_name_index
    on "User" USING GIN (to_tsvector('english', first_name || ' ' || second_name));