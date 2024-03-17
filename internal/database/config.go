package database

type Config interface {
	GetDBHost() string
	GetDBPort() uint16
	GetDBName() string
	GetDBUser() string
	GetDBPassword() string
}
