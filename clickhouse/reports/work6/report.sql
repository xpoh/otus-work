-- Найти жанры для каждого фильма
    SELECT m.name, groupArray(g.genre) FROM imdb.movies m
    JOIN imdb.genres g on m.id = g.movie_id
    GROUP BY m.name
    LIMIT 10;

-- Query id: 02744041-d470-4e67-9841-d4c3c7e6d57f
--
--     ┌─name─────┬─groupArray(genre)────────────────────────┐
--  1. │ Merlusse │ ['Comedy']                               │
--  2. │ Agency   │ ['Drama']                                │
--  3. │ Eclosion │ ['Short']                                │
--  4. │ 421      │ ['Short']                                │
--  5. │ Cube     │ ['Horror','Mystery','Sci-Fi','Thriller'] │
--  6. │ Turksib  │ ['Documentary']                          │
--  7. │ Kiyida   │ ['Short']                                │
--  8. │ Qin song │ ['Drama']                                │
--  9. │ Wealth   │ ['Drama']                                │
-- 10. │ Zvenoto  │ ['Documentary','Short']                  │
--     └──────────┴──────────────────────────────────────────┘
--
-- 10 rows in set. Elapsed: 0.071 sec. Processed 783.39 thousand rows, 19.95 MB (11.08 million rows/s., 282.08 MB/s.)
--     Peak memory usage: 122.46 MiB.


-- Запросить все фильмы, у которых нет жанра
    SELECT m.name FROM imdb.movies m
    LEFT ANTI JOIN imdb.genres g on m.id = g.movie_id
    LIMIT 10;
--
-- Query id: f3bc65ce-c811-4bc1-8767-3176010c37b6
--
--     ┌─name────────────┐
--  1. │ Lakshmi         │
--  2. │ Lakshmi         │
--  3. │ Lakshmi Narayan │
--  4. │ Lakshmi Vijayam │
--  5. │ Lakshmich Khel  │
--  6. │ Lakshminivasam  │
--  7. │ Lakshyam        │
--  8. │ Lal Americayil  │
--  9. │ Lal Bangla      │
-- 10. │ Lal Batti       │
--     └─────────────────┘
--
-- 10 rows in set. Elapsed: 0.025 sec. Processed 783.39 thousand rows, 13.87 MB (31.05 million rows/s., 549.61 MB/s.)
--     Peak memory usage: 21.03 MiB.

-- Объединить каждую строку из таблицы “Фильмы” с каждой строкой из таблицы “Жанры”
    SELECT m.name, g.genre FROM imdb.movies m
    CROSS JOIN imdb.genres g
    LIMIT 10;
-- Query id: e823870a-af1e-4cb2-a75b-0dee0e11756c
--
--     ┌─name─┬─genre───┐
--  1. │ #28  │ Fantasy │
--  2. │ #28  │ Horror  │
--  3. │ #28  │ Comedy  │
--  4. │ #28  │ Short   │
--  5. │ #28  │ Drama   │
--  6. │ #28  │ Drama   │
--  7. │ #28  │ Comedy  │
--  8. │ #28  │ Drama   │
--  9. │ #28  │ Family  │
-- 10. │ #28  │ Romance │
--     └──────┴─────────┘
--
-- 10 rows in set. Elapsed: 0.012 sec. Processed 750.62 thousand rows, 15.93 MB (63.23 million rows/s., 1.34 GB/s.)
--     Peak memory usage: 32.28 MiB.

-- Найти жанры для каждого фильма, НЕ используя INNER JOIN
    SELECT m.name, groupArray(g.genre) FROM imdb.movies m, imdb.genres g
    WHERE m.id = g.movie_id
    GROUP BY m.name
    LIMIT 10;
-- Query id: 26fc6dd1-bfaa-4628-aa73-cab95afd52c1
--
--     ┌─name─────┬─groupArray(genre)────────────────────────┐
--  1. │ Merlusse │ ['Comedy']                               │
--  2. │ Agency   │ ['Drama']                                │
--  3. │ Eclosion │ ['Short']                                │
--  4. │ 421      │ ['Short']                                │
--  5. │ Cube     │ ['Horror','Mystery','Sci-Fi','Thriller'] │
--  6. │ Turksib  │ ['Documentary']                          │
--  7. │ Kiyida   │ ['Short']                                │
--  8. │ Qin song │ ['Drama']                                │
--  9. │ Wealth   │ ['Drama']                                │
-- 10. │ Zvenoto  │ ['Documentary','Short']                  │
--     └──────────┴──────────────────────────────────────────┘
--
-- 10 rows in set. Elapsed: 0.071 sec. Processed 783.39 thousand rows, 19.95 MB (11.04 million rows/s., 281.07 MB/s.)
--     Peak memory usage: 117.48 MiB.

-- Найти всех актеров и актрис, снявшихся в фильме в N году
    SELECT m.name, groupArray( concat(a.first_name, ' ', a.last_name)) FROM imdb.movies m
    JOIN imdb.roles r on m.id = r.movie_id
    JOIN imdb.actors a on r.actor_id = a.id
    PREWHERE m.year = 2002 -- N
    GROUP BY m.name
    LIMIT 10;
-- Query id: 182dcb95-744b-4ff6-ad7a-62f1c2e3d5b0
--
--     ┌─name───────────┬─groupArray(concat(first_name, ' ', last_name))─────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────┐
--  1. │ 421            │ ['Keith Myers','Mikel Farber','Jesse Shannon','Kevin (VII) Campbell','Teresa Dahl-Bredine','Jon Graf','Michael Thyer','Steven Stedman']                                                                                                                    │
--  2. │ Deadline       │ ['Dirk Beemster','Wende Snijders','Henny Stoel','Anjali Taneja','Floris Bakker','Dorus van der Meer','Michel Bonset','Willem Emo','Cas Jansen','Javier Guzman','Ruben Taneja']                                                                             │
--  3. │ Respiro        │ ['Andrea Barreca','Gasparo Stallone','Bruno Stracusa','Francesco Casisa','Aldo Chessari','Salvatore Taranto','Loredana Solina','Angela Veronica Tarento','Alfonso Vicari','Guiseppe Sola','Filippo Solina','Matteo Solina','Elio Germano','Francesca Dell\'Imperio','Maria Pucillo','Muzzi Loffredo','Giovanna Maggiore','Ioland Maggiore','Angela Maraventano','Guiseppe Del Volgo','Vincenzo Cantalla','Flora Miotti','Veronica D\'Agostino','Vincenza Davi','Antonio Laterzo','Giacomo Galozzo','Filippo Pucillo','Pasquale Pucillo','Roberto Pucillo','Guiseppe Rupelli','Orlando Rupelli','Pasquale De Rubels','Roberto De Simone','Nunzio Lucifora','Antonino Maggiore','Giovanni Mannino','Nicola Mannino','Denise Capogrosso','Angelo Pucillo','Fabio Pucillo','Donatella Battiali','Maria Catia Belviso','Debora Billeci','Gaspare Sanguedolce','Rosario Sanguedolce','Ermanno Lubich','Attillo Lucia','Francesco Edoardo Anglieri','Antonio (I) Costa','Francesco Costanza','Vincenzo Davi','Guiseppe Palmisano','Avy Marciano','Domenico Martello','Vincenzo Barreca','Fernando Bartifeci','Valeria Golino','Giovan Battista Martello','Vincenzo Bellici','Domenico Amato','Vincenzo (II) Amato'] │
--  4. │ Forfrt         │ ['Sathya Sai Baba K.']                                                                                                                                                                                                                                     │
--  5. │ Haze           │ ['Alina Kudina','Michel Jean-Philippe']                                                                                                                                                                                                                    │
--  6. │ Sound of Pain  │ ['Don (VI) Nelson','Veronika Hadrava','Andrew (II) Jackson','Angela Cruikshank','Leslie (III) Wilson','Nelson Tomé']                                                                                                                                       │
--  7. │ Naked Betrayal │ ['Micah Bradshaw','Michael Kirst','Jason Riann','Jacklyn Lick','Mia Zottoli']                                                                                                                                                                              │
--  8. │ """Rods!"""    │ ['Stacy Keach']                                                                                                                                                                                                                                            │
--  9. │ Last Words     │ ['Mike Randleman','Eric Jorgenson','Annie Willett','David Starwalt','Art Cohan','Maria Russell','Catharine Scott']                                                                                                                                         │
-- 10. │ Undying Love   │ ['Joe Korzenik','Stephen Spreekmeester','Molly (I) Parker','Jonah Wexler','Hayley Smirnow','Erika Rosenbaum','Tania Rozmaryn','Zenia Ryboski','Ilona Traeger','Michele Tredger','Wim Vleesschouwer','Bluma Klodawski','Fanny La Croix','Leah Vineberg','Elly Vleesschouwer','Michelle Wallis','Helen Schwartz','Eva Slimowitz','David Rybowski','Denis Longpré','Andrew Simms','Adrien Burhop','Anna Newman','Jean Nicolai','David (IV) Newman','Erica Danya Goldblatt','Blake Bernett','Howard Bilerman','Kyle Allatt','Michel Amand','Miranda Handford','Victoria Barkoff','Moshe Kraus','Mark Antony Krupa','Elan Kunin','Harald Winter','Luba Drew','Holly G. Frankel','Lilly Friedman'] │
--     └────────────────┴────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────┘
--
-- 10 rows in set. Elapsed: 0.234 sec. Processed 4.64 million rows, 70.07 MB (19.84 million rows/s., 299.70 MB/s.)
--     Peak memory usage: 321.98 MiB.


-- Запросить все фильмы, у которых нет жанра, через ANTI JOIN
    SELECT m.name FROM imdb.movies m
    LEFT ANTI JOIN imdb.genres g on m.id = g.movie_id
    LIMIT 10;
-- Query id: f981537f-8576-46ee-859e-a0dcfccd2f9d
--
--     ┌─name───────────────┐
--  1. │ Flaming Tongues 2  │
--  2. │ Flaming Trail, The │
--  3. │ Flaming Waters     │
--  4. │ Flamingo Estates   │
--  5. │ Flamman            │
--  6. │ Flamme empor       │
--  7. │ Flamme, Die        │
--  8. │ Flamme, La         │
--  9. │ Flammen lgen, Die  │
-- 10. │ Flammende Herzen   │
--     └────────────────────┘
--
-- 10 rows in set. Elapsed: 0.025 sec. Processed 783.39 thousand rows, 13.87 MB (31.53 million rows/s., 558.20 MB/s.)
--     Peak memory usage: 24.92 MiB.


