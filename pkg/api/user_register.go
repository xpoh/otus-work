package api

import (
	"context"
	"net/http"

	"github.com/gin-gonic/gin"
	"github.com/google/uuid"
	"github.com/sirupsen/logrus"
	"github.com/xpoh/otus-work/pkg/api/models"
)

func (i *Instance) register(ctx context.Context, user models.UserRegisterPostRequest) (string, error) {
	c := i.db.GetConn()

	newID := uuid.New().String()
	hash := passHash(user.Password)

	_, err := c.Exec(
		ctx,
		"INSERT INTO postgres.public.\"User\" VALUES ($1,$2,$3,$4,$5,$6,$7)",
		newID,
		hash,
		user.FirstName,
		user.SecondName,
		user.Birthdate,
		user.Biography,
		user.City,
	)
	if err != nil {
		return "", err
	}

	return newID, nil
}

// UserRegisterPost Post /user/register
func (i *Instance) UserRegisterPost(c *gin.Context) {
	request := models.UserRegisterPostRequest{}

	if err := c.Bind(&request); err != nil {
		c.JSON(400, gin.H{"status": "Невалидные данные"})
		logrus.Debugf("Невалидные данные: %v", c.Request)

		return
	}

	logrus.Debugf("Request: %v", request)

	id, err := i.register(c, request)
	if err != nil {
		c.JSON(http.StatusInternalServerError, gin.H{"status": "Internal Server Error"})
		logrus.Errorf("Internal Server Error: %v", err)

		return
	}

	response := models.UserRegisterPost200Response{UserId: id}

	c.JSON(http.StatusOK, response)
}
