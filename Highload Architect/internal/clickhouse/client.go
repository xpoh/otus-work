package clickhouse

import (
	"database/sql"
	"os"
	"time"

	"github.com/ClickHouse/clickhouse-go/v2"
	_ "github.com/ClickHouse/clickhouse-go/v2"                   // clickhouse driver
	_ "github.com/golang-migrate/migrate/v4/database/clickhouse" // clickhouse migrate support
	_ "github.com/golang-migrate/migrate/v4/source/file"         // migrate scheme
)

type Config interface {
	GetClickhouseUser() string
	GetClickhouseAddress() string
}

type Client struct {
	cfg  Config
	conn *sql.DB
}

func (c *Client) GetConn() *sql.DB {
	return c.conn
}

func New(cfg Config) *Client {
	c := &Client{cfg: cfg}
	c.conn = clickhouse.OpenDB(&clickhouse.Options{
		Addr: []string{c.cfg.GetClickhouseAddress()},
		Auth: clickhouse.Auth{
			Database: "default",
			Username: c.cfg.GetClickhouseUser(),
		},
		Settings: clickhouse.Settings{
			"max_execution_time": 60,
		},
		DialTimeout: 30 * time.Second,
		Protocol:    clickhouse.HTTP,
	})

	return c
}

func execFile(conn *sql.DB, filename string) error {
	query, err := os.ReadFile(filename)
	if err != nil {
		return err
	}

	if _, err := conn.Exec(string(query)); err != nil {
		return err
	}

	return nil
}

func (c *Client) Open() error {
	if err := c.conn.Ping(); err != nil {
		return err
	}

	return nil
}

func (c *Client) Close() error {
	return c.conn.Close()
}

func (c *Client) Migrate() error {
	if err := c.conn.Ping(); err != nil {
		return err
	}

	if err := execFile(c.conn, "./scripts/clickhouse/001_schema_up.sql"); err != nil {
		return err
	}

	if err := execFile(c.conn, "./scripts/clickhouse/002_cluster_up.sql"); err != nil {
		return err
	}

	return nil
}
