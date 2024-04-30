package api

import (
	"github.com/segmentio/kafka-go"
	"net/http"

	"github.com/gin-gonic/gin"
	"github.com/gorilla/websocket"
	log "github.com/sirupsen/logrus"
)

var upgrader = websocket.Upgrader{
	ReadBufferSize:  1024,
	WriteBufferSize: 1024,
	CheckOrigin: func(r *http.Request) bool {
		return true
	},
}

func (i *Instance) Index(c *gin.Context) {
	c.HTML(
		http.StatusOK,
		"index.html",
		gin.H{
			"title": "Feed",
		},
	)
}

func (i *Instance) WsHandler(c *gin.Context) {
	id := c.Param("id")
	log.Infof("connect user %v to web socket", id)

	conn, err := upgrader.Upgrade(c.Writer, c.Request, nil)
	if err != nil {
		// panic(err)
		log.Printf("%s, error while Upgrading websocket connection\n", err.Error())
		c.AbortWithError(http.StatusInternalServerError, err)
		return
	}
	defer conn.Close()

	kafkaClient := kafka.NewReader(
		kafka.ReaderConfig{
			Brokers: i.cfg.GetKafkaBrokers(),
			GroupID: "feed",
			Topic:   id,
		},
	)

	defer kafkaClient.Close()

	if _, ok := i.usersOnline[id]; !ok {
		i.usersOnline[id] = kafkaClient
	}

	for {
		message, err := i.usersOnline[id].ReadMessage(c)
		if err != nil {
			log.Errorf("Error reading message: %v", err)

			return
		}

		if err = conn.WriteMessage(1, message.Value); err != nil {
			log.Printf("%s, error while writing message\n", err.Error())

			return
		}

		log.Infof("send message to user %v: %s", id, string(message.Value))
	}
}
