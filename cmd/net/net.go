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
	"os"

	log "github.com/sirupsen/logrus"
	"github.com/xpoh/otus-work/internal/config"
	"github.com/xpoh/otus-work/internal/database"
	sw "github.com/xpoh/otus-work/pkg/api"
)

func main() {
	cfg := config.New()
	initLogger(cfg)

	ctx := context.Background()

	db := database.NewInstance(cfg)
	if err := db.Run(ctx); err != nil {
		log.Panic(err)
	}
	defer func(db *database.Instance, ctx context.Context) {
		err := db.Stop(ctx)
		if err != nil {
			log.Error(err)
		}
	}(db, ctx)

	routes := sw.ApiHandleFunctions{
		DefaultAPI: sw.NewInstance(db, cfg),
	}

	log.Printf("Server started")

	router := sw.NewRouter(routes)

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