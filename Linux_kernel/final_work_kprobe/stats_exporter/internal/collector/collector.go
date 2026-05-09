package collector

import (
	"bufio"
	"context"
	"errors"
	"fmt"
	"log"
	"net/http"
	"os"
	"strconv"
	"strings"
	"time"

	"github.com/prometheus/client_golang/prometheus"
	"github.com/prometheus/client_golang/prometheus/promauto"
	"github.com/prometheus/client_golang/prometheus/promhttp"
	"golang.org/x/sync/errgroup"
)

const (
	sysfsTopAddrs = "/sys/module/kprobe_traffic/parameters/top_addrs"
	sysfsTrackSrc = "/sys/module/kprobe_traffic/parameters/track_src"
	namespace     = "kprobe_traffic"
	subsystem     = "stats"
)

type Collector struct {
	srv *http.Server

	traffic  *prometheus.GaugeVec
	topAddrs prometheus.Gauge
}

const addr = ":2112"

func New() *Collector {
	c := &Collector{
		traffic: promauto.NewGaugeVec(
			prometheus.GaugeOpts{
				Namespace: namespace,
				Subsystem: subsystem,
				Name:      "traffic_bytes",
				Help:      "Total traffic bytes per IP address (dst or src depending on track_src)",
			},
			[]string{"ip", "direction"},
		),
		topAddrs: promauto.NewGauge(
			prometheus.GaugeOpts{
				Namespace: namespace,
				Subsystem: subsystem,
				Name:      "top_addrs_scrape_count",
				Help:      "Number of top addresses returned in the last scrape",
			},
		),
		srv: &http.Server{
			Addr: addr,
		}}

	return c
}

func (c *Collector) Start(ctx context.Context) error {
	http.Handle("/metrics", http.HandlerFunc(func(w http.ResponseWriter, r *http.Request) {
		promhttp.Handler().ServeHTTP(w, r)
	}))

	http.HandleFunc("/", func(w http.ResponseWriter, r *http.Request) {
		_, err := w.Write([]byte(`<html><body><h1>Kprobe Traffic Exporter</h1>
<p><a href="/metrics">Metrics</a></p></body></html>`))
		if err != nil {
			log.Printf("write error: %v", err)
		}
	})

	errGr := errgroup.Group{}

	errGr.Go(func() error {
		log.Printf("starting kprobe_traffic exporter on %s/metrics", addr)

		err := c.srv.ListenAndServe()
		if !errors.Is(err, http.ErrServerClosed) {
			return fmt.Errorf("ListenAndServe(): %w", err)
		}

		return nil
	})

	errGr.Go(func() error {
		t := time.NewTicker(5 * time.Second)
		for {
			select {
			case <-ctx.Done():
				log.Printf("shutting down kprobe_traffic exporter on %s", addr)

				return nil
			case <-t.C:
				if err := c.Update(); err != nil {
					log.Printf("update error: %v", err)
				}
			}
		}
	})

	return errGr.Wait()
}

func (c *Collector) Stop(ctx context.Context) {
	log.Printf("stopping kprobe_traffic exporter on %s/metrics", addr)

	err := c.srv.Shutdown(ctx)
	if err != nil {
		log.Printf("error stopping kprobe_traffic exporter on %s/metrics: %v", addr, err)
	}
}

func (c *Collector) Update() error {
	c.traffic.Reset()

	entries, err := readTopAddrs()
	if err != nil {
		return err
	}

	direction := readDirection()

	for _, e := range entries {
		c.traffic.WithLabelValues(e.addr, direction).Set(float64(e.bytes))
	}

	c.topAddrs.Set(float64(len(entries)))
	return nil
}

type entry struct {
	addr  string
	bytes uint64
}

func readTopAddrs() ([]entry, error) {
	f, err := os.Open(sysfsTopAddrs)
	if err != nil {
		return nil, fmt.Errorf("open %s: %w", sysfsTopAddrs, err)
	}
	defer func(f *os.File) {
		_ = f.Close()
	}(f)

	var entries []entry
	scanner := bufio.NewScanner(f)
	for scanner.Scan() {
		line := strings.TrimSpace(scanner.Text())
		if line == "" {
			continue
		}

		parts := strings.SplitN(line, ": ", 2)
		if len(parts) != 2 {
			log.Printf("malformed line: %s", line)
			continue
		}

		addr := strings.TrimSpace(parts[0])
		bytesStr := strings.TrimSpace(parts[1])

		bytes, err := strconv.ParseUint(bytesStr, 10, 64)
		if err != nil {
			log.Printf("parse bytes error: %v", err)
			continue
		}

		entries = append(entries, entry{addr: addr, bytes: bytes})
	}

	if err := scanner.Err(); err != nil {
		return nil, fmt.Errorf("read %s: %w", sysfsTopAddrs, err)
	}

	return entries, nil
}

func readDirection() string {
	data, err := os.ReadFile(sysfsTrackSrc)
	if err != nil {
		return "dst"
	}

	val := strings.TrimSpace(string(data))
	if val == "Y" || val == "1" {
		return "src"
	}

	return "dst"
}
