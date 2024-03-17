-- Create default schema
CREATE SCHEMA IF NOT EXISTS public;

CREATE EXTENSION IF NOT EXISTS "uuid-ossp"
WITH
  SCHEMA public;

COMMENT ON EXTENSION "uuid-ossp" IS 'generate universally unique identifiers (UUIDs)';

CREATE TABLE IF NOT EXISTS "DialogMessage" (
                                 "id" uuid PRIMARY KEY DEFAULT (public.uuid_generate_v4()),
                                 "from_user_id" uuid NOT NULL,
                                 "to_user_id" uuid NOT NULL,
                                 "text" TEXT NOT NULL
);

CREATE TABLE IF NOT EXISTS "Post" (
                        "id" uuid PRIMARY KEY DEFAULT (public.uuid_generate_v4()),
                        "text" TEXT DEFAULT null,
                        "author_user_id" uuid NOT NULL
);

CREATE TABLE IF NOT EXISTS "User" (
                        "id" uuid PRIMARY KEY DEFAULT (public.uuid_generate_v4()),
                        "pass_hash" TEXT NOT NULL,
                        "first_name" TEXT DEFAULT null,
                        "second_name" TEXT DEFAULT null,
                        "birthdate" TEXT DEFAULT null,
                        "biography" TEXT DEFAULT null,
                        "city" TEXT DEFAULT null
);

COMMENT ON COLUMN "DialogMessage"."from_user_id" IS 'Идентификатор пользователя';

COMMENT ON COLUMN "DialogMessage"."to_user_id" IS 'Идентификатор пользователя';

COMMENT ON COLUMN "DialogMessage"."text" IS 'Текст сообщения';

COMMENT ON TABLE "Post" IS 'Пост пользователя';

COMMENT ON COLUMN "Post"."text" IS 'Текст поста';

COMMENT ON COLUMN "Post"."author_user_id" IS 'Идентификатор пользователя';

COMMENT ON COLUMN "User"."first_name" IS 'Имя';

COMMENT ON COLUMN "User"."second_name" IS 'Фамилия';

COMMENT ON COLUMN "User"."birthdate" IS 'Дата рождения';

COMMENT ON COLUMN "User"."biography" IS 'Интересы';

COMMENT ON COLUMN "User"."city" IS 'Город';

ALTER TABLE "DialogMessage" ADD FOREIGN KEY ("from_user_id") REFERENCES "User" ("id");

ALTER TABLE "DialogMessage" ADD FOREIGN KEY ("to_user_id") REFERENCES "User" ("id");

ALTER TABLE "Post" ADD FOREIGN KEY ("author_user_id") REFERENCES "User" ("id");
