package tarantool

import (
	"context"
	"fmt"
	"github.com/tarantool/go-tarantool/v2"
	"time"
)

type Config interface {
	GetAddress() string
	GetUSer() string
}

type Client struct {
	Conn *tarantool.Connection
}

func NewClient(ctx context.Context, cfg Config) (*Client, error) {
	dialer := tarantool.NetDialer{
		Address: cfg.GetAddress(),
		User:    cfg.GetUSer(),
	}

	opts := tarantool.Opts{
		Timeout: 10 * time.Second,
	}

	conn, err := tarantool.Connect(ctx, dialer, opts)
	if err != nil {
		fmt.Println("Connection refused:", err)
		return nil, err
	}

	return &Client{
		Conn: conn,
	}, nil
}

func (c *Client) Close() error {
	return c.Conn.Close()
}
