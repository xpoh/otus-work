package database

import "errors"

var (
	ErrDirtyVersion    = errors.New("dirty version")
	ErrUnableConnect   = errors.New("unable to connect to database")
	ErrNotConnected    = errors.New("not connected to database")
	ErrHeathCheckError = errors.New("db health check error")
	ErrCloseConnection = errors.New("error close connection to database")
)
