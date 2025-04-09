package api

import (
	"fmt"
	"net/http"

	"github.com/gin-gonic/gin"
	"github.com/google/uuid"
	"github.com/sirupsen/logrus"
	"github.com/xpoh/otus-work/pkg/api/models"
)

func (i *Instance) dialogList(ctx *gin.Context, userFrom, userTo string) ([]*models.DialogMessage, error) {
	c := i.click.GetConn()

	rows, err := c.Query(
		`select from_user_id, to_user_id, text from "DialogMessage"
          where from_user_id=$1 AND to_user_id=$2`, userFrom, userTo)
	if err != nil {
		return nil, fmt.Errorf("error get messages: %w", err)
	}

	defer rows.Close()

	messages := make([]*models.DialogMessage, 0)

	for rows.Next() {
		var message models.DialogMessage

		if err = rows.Scan(
			&message.From,
			&message.To,
			&message.Text,
		); err != nil {
			return nil, fmt.Errorf("error list query: %w", err)
		}

		messages = append(messages, &message)
	}

	return messages, nil
}

// DialogUserIdListGet Get /dialog/:user_id/list
func (i *Instance) DialogUserIdListGet(c *gin.Context) {
	userID, err := getUserIDFromHeader(c)
	if err != nil {
		c.JSON(http.StatusBadRequest, gin.H{"status": err})

		return
	}

	friendID := c.Param("user_id")
	if _, err := uuid.Parse(friendID); err != nil {
		c.JSON(http.StatusBadRequest, gin.H{"status": err})

		return
	}

	messages, err := i.dialogList(c, userID, friendID)
	if err != nil {
		logrus.Errorf("error dialog list: %v", err)

		c.JSON(http.StatusNotFound, gin.H{"status": "error send message"})
	}

	c.JSON(http.StatusOK, messages)
}

func (i *Instance) dialogSend(ctx *gin.Context, userFrom, userTo, text string) error {
	c := i.click.GetConn()

	shardKey := int(userFrom[0]+userTo[0]) % i.cfg.GetShardsCount()

	if _, err := c.Exec(
		`INSERT INTO "DialogMessage" VALUES ($1,$2,$3,$4)`,
		shardKey,
		userFrom,
		userTo,
		text,
	); err != nil {
		return err
	}

	return nil
}

// DialogUserIdSendPost Post /dialog/:user_id/send
func (i *Instance) DialogUserIdSendPost(c *gin.Context) {
	userID, err := getUserIDFromHeader(c)
	if err != nil {
		c.JSON(http.StatusBadRequest, gin.H{"status": err})

		return
	}

	friendID := c.Param("user_id")
	if _, err := uuid.Parse(friendID); err != nil {
		c.JSON(http.StatusBadRequest, gin.H{"status": err})

		return
	}

	request := models.DialogUserIdSendPostRequest{}

	if err := c.Bind(&request); err != nil {
		c.JSON(400, gin.H{"status": "Невалидные данные"})
		logrus.Debugf("Невалидные данные: %v", c.Request)

		return
	}

	if err := i.dialogSend(c, userID, friendID, request.Text); err != nil {
		logrus.Errorf("error send message: %v", err)

		c.JSON(http.StatusNotFound, gin.H{"status": "error send message"})
	}

	c.JSON(200, gin.H{"status": "OK"})
}
