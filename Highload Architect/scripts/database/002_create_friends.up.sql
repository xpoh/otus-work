CREATE TABLE IF NOT EXISTS "Friend"
(
    id  uuid not null constraint id_pk primary key,
    user_id   uuid not null constraint Friend_User_id_fk references "User",
    friend_id uuid not null constraint Friend_User_id_fk_2 references "User"
);