package api

import (
	"context"
	"github.com/gin-gonic/gin"
	"github.com/google/uuid"
	"github.com/sirupsen/logrus"
	"net/http"
)

func (i *Instance) register(ctx context.Context, user UserRegisterPostRequest) (string, error) {
	hash := passHash(user.Password)
	c := i.db.GetConn()
	rows, err := c.Query(
		ctx,
		"INSERT INTO User VALUES ($1,$2,$3,$4,$5,$6,$7,$8)",
		uuid.New().String(),
		user.
	)
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

// UserRegisterPost Post /user/register
func (i *Instance) UserRegisterPost(c *gin.Context) {
	request := UserRegisterPostRequest{}

	if err := c.Bind(request); err != nil {
		c.JSON(400, gin.H{"status": "Невалидные данные"})
		logrus.Debugf("Невалидные данные: %v", c.Request)
	}

	logrus.Debugf("Request: %v", request)

	hash, err := i.login(context.Background(), request.Id, request.Password)
	if err != nil {
		c.JSON(404, gin.H{"status": "Пользователь не найден"})
		logrus.Debugf("Пользователь не найден: %v", c.Request)
	}

	response := LoginPost200Response{Token: hash}

	c.JSON(http.StatusOK, response)
}
