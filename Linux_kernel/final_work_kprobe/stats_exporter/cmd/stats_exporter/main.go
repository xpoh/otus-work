package main

import (
	"context"
	"os/signal"
	"syscall"

	"github.com/xpoh/otus-work/Linux_kernel/final_work_kprobe/stats_exporter/internal/collector"
)

func main() {
	ctx, cancel := signal.NotifyContext(context.Background(), syscall.SIGINT, syscall.SIGTERM)
	defer cancel()

	col := collector.New()

	go func() {
		if err := col.Start(ctx); err != nil {
			return
		}
	}()

	<-ctx.Done()
	col.Stop(context.Background())
}
