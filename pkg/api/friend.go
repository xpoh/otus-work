package api

import (
	"context"
	jwt "github.com/appleboy/gin-jwt"
	"github.com/gin-gonic/gin"
	"github.com/google/uuid"
	"github.com/sirupsen/logrus"
	"net/http"
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
	claims := jwt.ExtractClaims(c)
	id := c.Param("id")

	logrus.Infof("User %v remove friend %v", id, claims["user_id"])

	if err := i.deleteFriend(context.Background(), id, claims["user_id"].(string)); err != nil {
		logrus.Errorf("error removing friend %v: %v", id, err)

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
	claims := jwt.ExtractClaims(c)
	id := c.Param("id")

	logrus.Infof("User %v remove friend %v", id, claims["user_id"])

	if err := i.addFriend(context.Background(), id, claims["user_id"].(string)); err != nil {
		logrus.Errorf("error removing friend %v: %v", id, err)

		c.JSON(http.StatusNotFound, gin.H{"status": "not find"})
	}

	c.JSON(http.StatusOK, gin.H{"status": "OK"})
}
