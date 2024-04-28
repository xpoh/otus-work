package api

import (
	"encoding/json"
	"github.com/xpoh/otus-work/pkg/api/models"
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

	if _, ok := i.usersOnline[id]; !ok {
		i.usersOnline[id] = make(chan models.Post, 1024)
	}

	for {
		select {
		case <-c.Done():
			close(i.usersOnline[id])

			log.Infof("disconnect user %v", id)

			return
		case post := <-i.usersOnline[id]:
			message, _ := json.Marshal(post)
			err = conn.WriteMessage(1, message)
			if err != nil {
				log.Printf("%s, error while writing message\n", err.Error())
				c.AbortWithError(http.StatusInternalServerError, err)
				break
			}

			log.Infof("send message to user %v: %s", id, message)
		}
	}
}
