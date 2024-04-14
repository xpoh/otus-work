package database

import (
	"context"
	"fmt"
	fakeit "github.com/brianvoe/gofakeit"
	_ "github.com/golang-migrate/migrate/v4/database/postgres" // for postgres supporting
	_ "github.com/golang-migrate/migrate/v4/source/file"       // for file format supporting
	"github.com/jackc/pgx/v4"
	log "github.com/sirupsen/logrus"
)

type Instance struct {
	conn *pgx.Conn
	cfg  Config
}

func NewInstance(cfg Config) *Instance {
	return &Instance{cfg: cfg}
}

const (
	mockDataCount = uint64(1000)
	mockDataSize  = uint64(1000)
)

func createMockData(ctx context.Context, conn *pgx.Conn) error {
	log.Infof("Creating mock...")
	defer log.Infof("Done")

	for range mockDataSize {
		tx, err := conn.Begin(ctx)
		if err != nil {
			return err
		}

		for range mockDataCount {
			if _, err := conn.Exec(
				ctx,
				"INSERT INTO postgres.public.\"User\" VALUES ($1,$2,$3,$4,$5,$6,$7)",
				fakeit.UUID(),
				fakeit.HackerVerb(),
				fakeit.FirstName(),
				fakeit.LastName(),
				fakeit.Date().String(),
				fakeit.BeerAlcohol(),
				fakeit.City(),
			); err != nil {
				return err
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
		if err := createMockData(ctx, i.conn); err != nil {
			return fmt.Errorf("createMockData(): %w", err)
		}
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
