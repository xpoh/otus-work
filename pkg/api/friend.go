package api

import (
	"context"
	"net/http"

	"github.com/gin-gonic/gin"
	"github.com/google/uuid"
	"github.com/sirupsen/logrus"
)

func (i *Instance) deleteFriend(ctx context.Context, id string, friendID string) error {
	c := i.db.GetConn()

	_, err := c.Exec(
		ctx,
		"DELETE FROM Friend WHERE user_id=$1 AND friend_id=$2", id, friendID)

	return err
}

// FriendDeleteUserIdPut Put /friend/delete/:user_id
func (i *Instance) FriendDeleteUserIdPut(c *gin.Context) {
	friendID := c.Param("user_id")

	userID, err := getUserIDFromHeader(c)
	if err != nil {
		c.JSON(http.StatusBadRequest, gin.H{"status": err})
	}

	logrus.Infof("User %v remove friend %v", userID, friendID)

	if err := i.deleteFriend(c, userID, friendID); err != nil {
		logrus.Errorf("error removing friend %v: %v", friendID, err)

		c.JSON(http.StatusNotFound, gin.H{"status": "not find"})
	}

	c.JSON(http.StatusOK, gin.H{"status": "OK"})
}

func (i *Instance) addFriend(ctx context.Context, id string, friendID string) error {
	c := i.db.GetConn()

	_, err := c.Exec(
		ctx,
		"INSERT INTO Friend (id, user_id, friend_id) VALUES ($1, $2, $3)", uuid.New().String(), id, friendID)

	return err
}

// FriendSetUserIdPut Put /friend/set/:user_id
func (i *Instance) FriendSetUserIdPut(c *gin.Context) {
	friendID := c.Param("user_id")

	userID, err := getUserIDFromHeader(c)
	if err != nil {
		c.JSON(http.StatusBadRequest, gin.H{"status": err})
	}

	logrus.Infof("User %v remove friend %v", userID, friendID)

	if err := i.addFriend(c, friendID, userID); err != nil {
		logrus.Errorf("error removing friend %v: %v", friendID, err)

		c.JSON(http.StatusNotFound, gin.H{"status": "not find"})
	}

	c.JSON(http.StatusOK, gin.H{"status": "OK"})
}
