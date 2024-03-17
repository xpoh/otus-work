package database

import (
	"context"
	"fmt"

	_ "github.com/golang-migrate/migrate/v4/database/postgres" // for postgres supporting
	_ "github.com/golang-migrate/migrate/v4/source/file"       // for file format supporting
	"github.com/jackc/pgx/v4"
)

type Instance struct {
	conn *pgx.Conn
	cfg  Config
}

func NewInstance(cfg Config) *Instance {
	return &Instance{cfg: cfg}
}

func (i *Instance) Run(ctx context.Context) error {
	if err := i.Connect(ctx); err != nil {
		return fmt.Errorf("s.storage.Connect(): %w", err)
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
