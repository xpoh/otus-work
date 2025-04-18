/*
 * OTUS Highload Architect
 *
 * No description provided (generated by Openapi Generator https://github.com/openapitools/openapi-generator)
 *
 * API version: 1.2.0
 * Generated by: OpenAPI Generator (https://openapi-generator.tech)
 */

package main

import (
	"context"
	"fmt"
	"github.com/google/uuid"
	"os"

	"github.com/gin-contrib/requestid"
	log "github.com/sirupsen/logrus"
	"github.com/xpoh/otus-work/internal/clickhouse"
	"github.com/xpoh/otus-work/internal/config"
	"github.com/xpoh/otus-work/internal/database"
	"github.com/xpoh/otus-work/internal/tarantool"
	sw "github.com/xpoh/otus-work/pkg/api"
)

func main() {
	cfg := config.New()
	cfg.PrintDebug()

	initLogger(cfg)

	click := clickhouse.New(cfg)
	if err := click.Open(); err != nil {
		log.Panic(err)
	}

	defer click.Close()

	if err := click.Migrate(); err != nil {
		log.Panicf("db.Migrate(...): %v", err)
	}

	ctx := context.Background()

	client, err := tarantool.NewClient(ctx, cfg)
	if err != nil {
		log.Panic(err)
	}

	defer func() {
		if err := client.Close(); err != nil {
			log.Error(err)
		}
	}()

	db := database.NewInstance(cfg, client)
	if err := db.Run(ctx); err != nil {
		log.Panic(err)
	}

	defer func() {
		err := db.Stop(ctx)
		if err != nil {
			log.Error(err)
		}
	}()

	routes := sw.ApiHandleFunctions{
		DefaultAPI: sw.NewInstance(db, client, cfg, click),
	}

	log.Printf("Server started")

	router := sw.NewRouter(routes)
	router.LoadHTMLGlob("index.html")
	router.Use(
		requestid.New(
			requestid.WithGenerator(func() string {
				return uuid.New().String()
			}),
			requestid.WithCustomHeaderStrKey("x-request-id"),
		),
	)

	log.Panic(router.Run(fmt.Sprintf("%s:%s", cfg.GetHost(), cfg.GetPort())))
}

func initLogger(cfg *config.Config) {
	log.SetReportCaller(true)
	log.SetOutput(os.Stdout)

	log.SetLevel(cfg.GetLogLevel())

	log.SetFormatter(
		&log.TextFormatter{
			FullTimestamp: true,
		},
	)
}
