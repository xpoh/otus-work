package config

import (
	"fmt"
	"os"
	"reflect"
	"regexp"

	"github.com/davecgh/go-spew/spew"
	"github.com/ilyakaznacheev/cleanenv"
	log "github.com/sirupsen/logrus"
)

const (
	envConfigFileName = ".env"
)

type Config struct {
	env *EnvSetting
}

type EnvSetting struct {
	Host string `env:"HOST" env-default:"0.0.0.0" env-description:"Host"`
	Port string `env:"HTTP_PORT" env-default:"8000" env-description:"Http port"`

	DBHost     string `env:"DB_HOST" env-default:"127.0.0.1" env-description:"IP or hostname where DB resides"`
	DBPort     uint16 `env:"DB_PORT" env-default:"5432" env-description:"DB's port"`
	DBName     string `env:"DB_NAME" env-default:"postgres" env-description:"Database name"`
	DBUser     string `env:"DB_USER" env-default:"postgres" env-description:"Username to connect to DB"`
	DBPassword string `env:"DB_PASSWORD" env-default:"postgres" env-description:"Password to connect to DB"`
	MockData   bool   `env:"DB_MOCK_DATA" env-default:"false"`

	LogLevel string `env:"LOG_LEVEL" env-default:"info" env-description:"log level: trace, debug, info, warn, error, fatal, panic"` //nolint:lll
}

func issetEnvConfigFile() bool {
	_, err := os.Stat(envConfigFileName)

	return err == nil
}

func (e *EnvSetting) GetHelpString() (string, error) {
	customHeader := "options which can be set via env: "

	helpString, err := cleanenv.GetDescription(e, &customHeader)
	if err != nil {
		return "", fmt.Errorf("get help string failed: %w", err)
	}

	return helpString, nil
}

func New() *Config {
	envSetting := &EnvSetting{} //nolint:exhaustruct

	helpString, err := envSetting.GetHelpString()
	if err != nil {
		log.Panic("getting help string of env settings failed: ", err)
	}

	log.Info(helpString)

	if issetEnvConfigFile() {
		if err := cleanenv.ReadConfig(envConfigFileName, envSetting); err != nil {
			log.Panicf("read env cofig file failed: %s", err)
		}
	} else if err := cleanenv.ReadEnv(envSetting); err != nil {
		log.Panicf("read env config failed: %s", err)
	}

	return &Config{
		env: envSetting,
	}
}

func (c *Config) PrintDebug() {
	envReflect := reflect.Indirect(reflect.ValueOf(c.env))
	envReflectType := envReflect.Type()

	exp := regexp.MustCompile("([Tt]oken|[Pp]assword)")

	for i := 0; i < envReflect.NumField(); i++ {
		key := envReflectType.Field(i).Name

		if exp.MatchString(key) {
			val, _ := envReflect.Field(i).Interface().(string)
			log.Debugf("%s: len %d", key, len(val))

			continue
		}

		log.Debugf("%s: %v", key, spew.Sprintf("%#v", envReflect.Field(i).Interface()))
	}

	log.Infof("config loaded: %+v", *c.env)
}

func (c *Config) GetHost() string {
	return c.env.Host
}

func (c *Config) GetPort() string {
	return c.env.Port
}

func (c *Config) GetDBHost() string { return c.env.DBHost }

func (c *Config) GetDBPort() uint16 {
	return c.env.DBPort
}

func (c *Config) GetDBName() string {
	return c.env.DBName
}

func (c *Config) GetDBUser() string {
	return c.env.DBUser
}

func (c *Config) GetDBPassword() string {
	return c.env.DBPassword
}

func (c *Config) GetMockData() bool {
	return c.env.MockData
}

func (c *Config) GetLogLevel() log.Level {
	lvl, err := log.ParseLevel(c.env.LogLevel)
	if err != nil {
		log.Error(err)

		return log.InfoLevel
	}

	return lvl
}
