package api

import (
	"context"
	"crypto/sha512"
	"encoding/hex"
	"net/http"

	"github.com/gin-gonic/gin"
	"github.com/sirupsen/logrus"
)

func passHash(pass string) string {
	hash := sha512.New()
	hash.Write([]byte(pass))
	return hex.EncodeToString(hash.Sum(nil))
}

func (i *Instance) login(ctx context.Context, id string, password string) (string, error) {
	hash := passHash(password)
	c := i.db.GetConn()

	rows, err := c.Query(ctx, "SELECT count(1) FROM postgres.public.\"User\" WHERE pass_hash=$1 AND id=$2", hash, id)
	if err != nil {
		return "", err
	}

	defer rows.Close()

	var count uint64

	for rows.Next() {
		if err := rows.Scan(&count); err != nil {
			return "", err
		}
	}

	return hash, nil
}

// LoginPost Post /login
func (i *Instance) LoginPost(c *gin.Context) {
	request := LoginPostRequest{}

	if err := c.Bind(&request); err != nil {
		c.JSON(400, gin.H{"status": "Невалидные данные"})
		logrus.Errorf("Невалидные данные: %v", c.Request)

		return
	}

	logrus.Debugf("Request: %v", request)

	hash, err := i.login(context.Background(), request.Id, request.Password)
	if err != nil {
		c.JSON(404, gin.H{"status": "Пользователь не найден"})
		logrus.Debugf("Пользователь не найден: %v", c.Request)

		return
	}

	response := LoginPost200Response{Token: hash}

	c.JSON(http.StatusOK, response)
}
