package database

import (
	"context"
	"fmt"
	tar "github.com/tarantool/go-tarantool/v2"
	"github.com/xpoh/otus-work/internal/tarantool"
	"math/rand"

	fakeit "github.com/brianvoe/gofakeit"
	_ "github.com/golang-migrate/migrate/v4/database/postgres" // for postgres supporting
	_ "github.com/golang-migrate/migrate/v4/source/file"       // for file format supporting
	"github.com/jackc/pgx/v4"
	log "github.com/sirupsen/logrus"
)

type Instance struct {
	conn *pgx.Conn
	tcl  *tarantool.Client
	cfg  Config
}

func NewInstance(cfg Config, tcl *tarantool.Client) *Instance {
	return &Instance{cfg: cfg, tcl: tcl}
}

const (
	mockDataCount          = uint64(100)
	mockDataSize           = uint64(100)
	mockDataPostsPerUser   = 20
	mockDataFriendsPerUser = 20
)

func (i *Instance) createMockData(ctx context.Context, conn *pgx.Conn) error {
	users := make([]string, 0, mockDataSize*mockDataCount)

	fmt.Printf("Creating mock...")
	defer fmt.Printf("Done.\n")

	var index int
	for range mockDataSize {
		tx, err := conn.Begin(ctx)
		if err != nil {
			return err
		}

		for range mockDataCount {
			userID := fakeit.UUID()
			users = append(users, userID)

			if _, err := conn.Exec(
				ctx,
				"INSERT INTO postgres.public.\"User\" VALUES ($1,$2,$3,$4,$5,$6,$7)",
				userID,
				fakeit.HackerVerb(),
				fakeit.FirstName(),
				fakeit.LastName(),
				fakeit.Date().String(),
				fakeit.BeerAlcohol(),
				fakeit.City(),
			); err != nil {
				return err
			}

			for range rand.Intn(mockDataPostsPerUser) {
				if _, err := conn.Exec(
					ctx,
					"INSERT INTO postgres.public.\"Post\" VALUES ($1,$2,$3)",
					fakeit.UUID(),
					fakeit.Sentence(rand.Intn(50)),
					userID,
				); err != nil {
					return err
				}
			}

			for range rand.Intn(mockDataFriendsPerUser) {
				randomUser := users[rand.Intn(len(users))]
				if _, err := conn.Exec(
					ctx,
					"INSERT INTO postgres.public.\"Friend\" VALUES ($1,$2,$3)",
					fakeit.UUID(),
					userID,
					randomUser,
				); err != nil {
					return err
				}

				if _, err := i.tcl.Conn.Do(
					tar.NewInsertRequest("friends").Tuple([]interface{}{index, userID, randomUser}),
				).Get(); err != nil {
					log.Errorf("i=%d: %v", index, err)
				}

				index++
			}
		}

		if err := tx.Commit(ctx); err != nil {
			return err
		}

		fmt.Printf(".")
	}

	return nil
}

func (i *Instance) Run(ctx context.Context) error {
	if err := i.Connect(ctx); err != nil {
		return fmt.Errorf("s.storage.Connect(): %w", err)
	}

	if i.cfg.GetMockData() {
		go func() {
			if err := i.createMockData(ctx, i.conn); err != nil {
				log.Errorf("createMockData(): %v", err)
			}
		}()
	}

	return nil
}

func (i *Instance) Stop(ctx context.Context) error {
	if err := i.Disconnect(ctx); err != nil {
		return fmt.Errorf("i..Disconnect(): %w", err)
	}

	return nil
}

func (i *Instance) getConnectionURL() string {
	return fmt.Sprintf( //nolint: nosprintfhostport
		"postgres://%s:%s@%s:%d/%s?sslmode=disable",
		i.cfg.GetDBUser(),
		i.cfg.GetDBPassword(),
		i.cfg.GetDBHost(),
		i.cfg.GetDBPort(),
		i.cfg.GetDBName(),
	)
}

func (i *Instance) Connect(ctx context.Context) error {
	conn, err := pgx.Connect(ctx, i.getConnectionURL())
	if err != nil {
		return fmt.Errorf("%w: %w", ErrUnableConnect, err)
	}

	i.conn = conn

	return nil
}

func (i *Instance) Disconnect(ctx context.Context) error {
	if i.conn == nil {
		return fmt.Errorf("%w", ErrNotConnected)
	}

	if err := i.conn.Close(ctx); err != nil {
		return fmt.Errorf("%w: %w", ErrCloseConnection, err)
	}

	return nil
}

func (i *Instance) GetConn() *pgx.Conn {
	return i.conn
}
