package api

import (
	"net/http"

	"github.com/gin-gonic/gin"
	"github.com/google/uuid"
	"github.com/sirupsen/logrus"
	"github.com/xpoh/otus-work/pkg/api/models"
	dialogsV1 "github.com/xpoh/otus-work/pkg/grpc/dialogs/v1"
)

// DialogUserIdListGetV2 Get /v2/dialog/:user_id/list
func (i *Instance) DialogUserIdListGetV2(c *gin.Context) {
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

	messages, err := i.dialogsClient.List(c, &dialogsV1.ListRequest{
		UserFrom: userID,
		UserTo:   friendID,
	})
	if err != nil {
		logrus.Errorf("error dialog list: %v", err)

		c.JSON(http.StatusNotFound, gin.H{"status": "error send message"})
	}

	c.JSON(http.StatusOK, messages)
}

// DialogUserIdSendPostV2 Post /v2/dialog/:user_id/send
func (i *Instance) DialogUserIdSendPostV2(c *gin.Context) {
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

	if _, err := i.dialogsClient.Send(c, &dialogsV1.SendRequest{
		UserFrom: userID,
		UserTo:   friendID,
		Text:     request.Text,
	}); err != nil {
		logrus.Errorf("error send message: %v", err)

		c.JSON(http.StatusNotFound, gin.H{"status": "error send message"})
	}

	c.JSON(200, gin.H{"status": "OK"})
}
